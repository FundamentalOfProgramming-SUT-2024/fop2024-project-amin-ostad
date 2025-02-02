#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <math.h>
#include <locale.h>


#define NUM_ROOMS 6

typedef struct {
    int x;
    int y;
} Position;



typedef struct {
    int startx;
    int starty;
    int height;
    int width;
    Position * weapon;
    Position **Gold;
    Position **traps;
    Position **food;
    Position **doors;
    WINDOW *win;
} room;


typedef struct {
    int xposition;
    int yposition;
    int health;
    int gold;
    char weapon;
} player;

room rooms[NUM_ROOMS]; 

WINDOW *create_newwin(int height, int width, int starty, int startx);
void detail_room(room *r);
void move_character(room *r);
void add_door_1(room *r);
void add_door_2(room *r);
void add_door_3(room *r);
void add_door_4(room *r);
void add_door_5(room *r);
void add_door_6(room *r);
//void create_hallway(Position * door1 , Position * door2);
int check_weapon(int y, int x );


void initialize_map() {
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    refresh();
    int rows, columns;
    getmaxyx(stdscr, rows, columns);
    srand(time(NULL));

    FILE *fptr = fopen("save.txt", "w");

    for (int i = 0; i < NUM_ROOMS; i++) {
        rooms[i].startx = rand() % (columns / 6) + (i % 3) * (columns / 3);
        rooms[i].starty = rand() % (rows / 4) + (i / 3) * (rows / 2) + 1;

        int height_range = (i <= 3) ? rows / 2 - rooms[i].starty - 8 : rows - rooms[i].starty - 8;
        rooms[i].height = (height_range > 0) ? (rand() % height_range) + 6 : 6;

        int width_range;
        if(i == 0 || i == 3){ width_range = (columns / 3) - rooms[i].startx - 8;}
        else if(i == 1 || i == 4){ width_range = 2*(columns / 3) - rooms[i].startx - 8;}
        else if(i == 2 || i == 5) {width_range = (columns) - rooms[i].startx - 8;}
        rooms[i].width = (width_range > 0) ? (rand() % width_range) + 6 : 6;

        fprintf(fptr, "Room %d -> startx: %d, starty: %d, height: %d, width: %d\n",
                i, rooms[i].startx, rooms[i].starty, rooms[i].height, rooms[i].width);

        rooms[i].win = create_newwin(rooms[i].height, rooms[i].width, rooms[i].starty, rooms[i].startx);
        detail_room(&rooms[i]);
    }
    add_door_1(&rooms[0]);
    add_door_2(&rooms[1]);
    add_door_3(&rooms[2]);
    add_door_4(&rooms[3]);
    add_door_5(&rooms[4]);
    add_door_6(&rooms[5]);
    refresh();
/*
    create_hallway(rooms[0].doors[0], rooms[1].doors[2]); // Room 0 (right) → Room 1 (left)
    create_hallway(rooms[1].doors[1], rooms[4].doors[1]); // Room 1 (bottom) → Room 4 (top)
    create_hallway(rooms[1].doors[0], rooms[2].doors[2]); // Room 1 (right) → Room 2 (left)
    create_hallway(rooms[2].doors[1], rooms[3].doors[0]); // Room 2 (bottom) → Room 3 (top)
    create_hallway(rooms[3].doors[1], rooms[4].doors[0]); // Room 3 (right) → Room 4 (left)
    create_hallway(rooms[4].doors[0], rooms[5].doors[0]); // Room 4 (right) → Room 5 (left)



    create_hallway(rooms[0].doors[0], rooms[1].doors[2]); // top left to top middle
    create_hallway(rooms[1].doors[1], rooms[4].doors[1]); // top middle to bottom middle
    create_hallway(rooms[1].doors[0], rooms[2].doors[2]); // top middle to top right
    create_hallway(rooms[2].doors[1], rooms[3].doors[0]); // top right to bottom left (use specific doors)
    create_hallway(rooms[3].doors[1], rooms[4].doors[0]); // bottom left to bottom middle
    create_hallway(rooms[4].doors[0], rooms[5].doors[0]); // bottom middle to bottom right

*/

    fclose(fptr);
    refresh();
    noecho();
    move_character(&rooms[0]);

    getch();
    endwin();
}

void detail_room(room *r) {
    for (int i = r->startx + 1; i < r->startx + r->width - 1; i++) {
        for (int j = r->starty + 1; j < r->starty + r->height - 1; j++) {
            mvaddch(j, i, '.'); 
        }
    }

    // Place pillars
    int num_pillars = (r->height * r->width > 40) ? 2 : 1;
    for (int i = 0; i < num_pillars; i++) {
        int pillar_x, pillar_y;
        do {
            pillar_x = (rand() % (r->width - 4)) + r->startx + 2;
            pillar_y = (rand() % (r->height - 4)) + r->starty + 2;
        } while (mvinch(pillar_y, pillar_x) != '.');
        mvaddch(pillar_y, pillar_x, 'O');
    }


    int num_food = (r->height * r->width > 40) ? 2 : 1;
    r->food = (Position **)malloc(num_food * sizeof(Position *));
    for (int i = 0; i < num_food; i++) {
        r->food[i] = (Position *)malloc(sizeof(Position));
        do {
            r->food[i]->x = (rand() % (r->width - 2)) + r->startx + 1;
            r->food[i]->y = (rand() % (r->height - 2)) + r->starty + 1;
        } while (mvinch(r->food[i]->y, r->food[i]->x) != '.'); 
        mvaddch(r->food[i]->y, r->food[i]->x, 'F');
    }

    int num_traps = (r->height * r->width > 40) ? 4 : 2;
    r->traps = (Position **)malloc(num_traps * sizeof(Position *));
    int i;
    for (i = 0; i < num_traps-1; i++) {
        r->traps[i] = (Position *)malloc(sizeof(Position));
        do {
            r->traps[i]->x = (rand() % (r->width - 4)) + r->startx + 2;
            r->traps[i]->y = (rand() % (r->height - 4)) + r->starty + 2;
        } while (mvinch(r->traps[i]->y, r->traps[i]->x) != '.');
        r->traps[i+1] = NULL;
    }

    int num_gold = (r->height * r->width > 40) ? 2 : 2;
    r->Gold = (Position **)malloc(num_gold * sizeof(Position *));
    for (int i = 0; i < num_gold; i++) {
        r->Gold[i] = (Position *)malloc(sizeof(Position));
        do {
            r->Gold[i]->x = (rand() % (r->width - 2)) + r->startx + 1;
            r->Gold[i]->y = (rand() % (r->height - 2)) + r->starty + 1;
        } while (mvinch(r->Gold[i]->y, r->Gold[i]->x) != '.');
        mvaddch(r->Gold[i]->y, r->Gold[i]->x, 'G');
    }

    r->weapon = (Position *)malloc(sizeof(Position));
    char weapon_array[5] = { 'M' , 'D' , 'W' , 'A' , 'S'};
    do {
            r->weapon->x = (rand() % (r->width - 2)) + r->startx + 1;
            r->weapon->y = (rand() % (r->height - 2)) + r->starty + 1;
        } while (mvinch(r->weapon->y, r->weapon->x) != '.');
    int weapon_num = rand() % 5;
    mvaddch(r->weapon->y , r->weapon->x , weapon_array[weapon_num]);
    refresh();
}



void add_door_1(room *r){
    r->doors = (Position**)malloc(sizeof(Position*));
    r->doors[0] = (Position *)malloc(sizeof(Position));
    
    // right door
    r->doors[0]->x = r->startx + r->width - 1;
    r->doors[0]->y = (rand()% (r->height-2)) + r->starty +1;
    mvwaddch(r->win, r->doors[0]->y -  r->starty ,  r->width - 1 , '+');
    wrefresh(r->win);
}

void add_door_2(room *r){
    r->doors = (Position**)malloc(3 *sizeof(Position*));
    r->doors[0] = (Position *)malloc(sizeof(Position));
    r->doors[1] = (Position *)malloc(sizeof(Position));
    r->doors[2] = (Position *)malloc(sizeof(Position));

    // right door
    r->doors[0]->x = r->startx + r->width - 1;
    r->doors[0]->y = (rand()% (r->height-2)) + r->starty +1;
    mvwaddch(r->win, r->doors[0]->y -  r->starty ,  r->width - 1 , '+');

    // bottom door
    r->doors[1]->x = (rand() % (r->width - 2)) + r->startx + 1;
    r->doors[1]->y = r->starty + r->height - 1;
    mvwaddch(r->win ,  r->height - 1 , r->doors[1]->x - r->startx  , '+');

    // left door
    r->doors[2]->x = r->startx;
    r->doors[2]->y = (rand()% (r->height-2)) + r->starty +1;
    mvwaddch(r->win, r->doors[2]->y -  r->starty , 0 , '+');

    wrefresh(r->win);

}

void add_door_3(room *r){
    r->doors = (Position**)malloc(2 *sizeof(Position*));
    r->doors[0] = (Position *)malloc(sizeof(Position));
    r->doors[1] = (Position *)malloc(sizeof(Position));

    // bottom door
    r->doors[0]->x = (rand() % (r->width - 2)) + r->startx + 1;
    r->doors[0]->y = r->starty + r->height - 1;
    mvwaddch(r->win ,  r->height - 1 , r->doors[0]->x - r->startx  , '+');

    // left door
    r->doors[1]->x = r->startx;
    r->doors[1]->y = (rand()% (r->height-2)) + r->starty +1;
    mvwaddch(r->win, r->doors[1]->y -  r->starty , 0 , '+');

    wrefresh(r->win);
}

void add_door_4(room *r){
    r->doors = (Position**)malloc(2 *sizeof(Position*));
    r->doors[0] = (Position *)malloc(sizeof(Position));
    r->doors[1] = (Position *)malloc(sizeof(Position));

    // upper door
    r->doors[0]->x = (rand() % (r->width - 2)) + r->startx + 1;
    r->doors[0]->y = r-> starty;
    mvwaddch(r->win ,  0 , r->doors[0]->x - r->startx  , '+');

    // right door
    r->doors[1]->x = r->startx + r->width - 1;
    r->doors[1]->y = (rand()% (r->height-2)) + r->starty +1;
    mvwaddch(r->win, r->doors[1]->y -  r->starty ,  r->width - 1 , '+');

    wrefresh(r->win);
}

void add_door_5(room *r){

    r->doors = (Position**)malloc(3 *sizeof(Position*));
    r->doors[0] = (Position *)malloc(sizeof(Position));
    r->doors[1] = (Position *)malloc(sizeof(Position));
    r->doors[2] = (Position *)malloc(sizeof(Position));

    // right door
    r->doors[0]->x = r->startx + r->width - 1;
    r->doors[0]->y = (rand()% (r->height-2)) + r->starty +1;
    mvwaddch(r->win, r->doors[0]->y -  r->starty ,  r->width - 1 , '+');

    // top door
    r->doors[1]->x = (rand() % (r->width - 2)) + r->startx + 1;
    r->doors[1]->y = r->starty;
    mvwaddch(r->win ,  0 , r->doors[1]->x - r->startx  , '+');

    // left door
    r->doors[2]->x = r->startx;
    r->doors[2]->y = (rand()% (r->height-2)) + r->starty +1;
    mvwaddch(r->win, r->doors[2]->y -  r->starty , 0 , '+');

    wrefresh(r->win);
}

void add_door_6(room *r){

    // left door
    r->doors = (Position**)malloc(sizeof(Position*));
    r->doors[0] = (Position *)malloc(sizeof(Position));
    r->doors[0]->x = r->startx;
    r->doors[0]->y = (rand()% (r->height-2)) + r->starty +1;
    mvwaddch(r->win, r->doors[0]->y -  r->starty , 0 , '+');

    wrefresh(r->win);
}

int check_weapon(int y, int x){
    if(mvinch(y , x) == 'M'){
        mvprintw(0 , 2 , "%s" , "You picked up a Mace!                                 ");
        return 1;
    }
    else if(mvinch(y , x) == 'S'){
        mvprintw(0 , 2 , "%s" , "You picked up a Sword!                                 ");
        return 1;
    }
    else if(mvinch(y , x) == 'D'){
        mvprintw(0 , 2 , "%s" , "You picked up a Dagger!                                 ");
        return 1;
    }
    else if(mvinch(y , x) == 'W'){
        mvprintw(0 , 2 , "%s" , "You picked up a Magic Wand!                                 ");
        return 1;
    }
    else if(mvinch(y , x) == 'A'){
        mvprintw(0 , 2 , "%s" , "You picked up an Arrow!                                 ");
        return 1;
    }
    return 0;
}




void move_character(room *r) {
    player user;
    user.xposition = r->startx + 1;
    user.yposition = r->starty + 1;
    user.health = 30;
    user.gold = 0;

    int ch;
    mvaddch(user.yposition, user.xposition, '@');
    refresh();
    int t = 0;
    while ((ch = getch()) != 'q') { 
        if(t == 0){
            mvaddch(user.yposition, user.xposition, '.'); 
        }
        else{
            mvaddch(user.yposition, user.xposition, '^');
            t = 0;
        }
        switch (ch) {
            case KEY_UP:
                if(check_weapon(user.yposition - 1 , user.xposition)){
                    user.yposition--;
                }
                else if (mvinch(user.yposition - 1, user.xposition) == '.'){
                    user.yposition--;
                    for(int i = 0; r->traps[i] != NULL ; i++){
                        if(r->traps[i]->x == user.xposition && r->traps[i]->y == user.yposition){
                            user.health -= 10;
                            mvprintw(0 , 2 , "You stepped on a trap. your health is %d         " , user.health);
                            t = 1;
                        }
                    }
                }
                else if (mvinch(user.yposition - 1, user.xposition) == 'F'){
                    user.yposition--;
                    user.health += 10;
                    mvprintw(0 , 2 ,"You ate food. Your health is %d                 " , user.health);
                }
                else if (mvinch(user.yposition - 1, user.xposition) == 'G'){
                    user.yposition--;
                    user.gold += (rand() % 5) + 1;
                    mvprintw(0 , 2 ,"You gained a bag of gold. You now have %d gold" , user.gold);
                }
                break;
            case KEY_DOWN:
                if(check_weapon(user.yposition + 1 , user.xposition)){
                    user.yposition++;
                }
                else if (mvinch(user.yposition + 1, user.xposition) == '.'){
                    user.yposition++;
                    for(int i = 0; r->traps[i] != NULL ; i++){
                        if(r->traps[i]->x == user.xposition && r->traps[i]->y == user.yposition){
                            user.health -= 10;
                            mvprintw(0 , 2 , "You stepped on a trap. your health is %d        " , user.health);
                            t = 1;
                        }
                    }
                }
                else if (mvinch(user.yposition + 1, user.xposition) == 'F'){
                    user.yposition++;
                    user.health += 10;
                    mvprintw(0 , 2 ,"You ate food. Your health is %d                " , user.health);
                }
                else if (mvinch(user.yposition + 1, user.xposition) == 'G'){
                    user.yposition++;
                    user.gold += (rand() % 5) + 1;
                    mvprintw(0 , 2 ,"You gained a bag of gold. You now have %d gold" , user.gold);
                }
                break;
            case KEY_LEFT:
                if(check_weapon(user.yposition  , user.xposition - 1)){
                    user.xposition--;
                }
                else if (mvinch(user.yposition, user.xposition - 1) == '.'){
                    user.xposition--;
                    for(int i = 0; r->traps[i] != NULL ; i++){
                        if(r->traps[i]->x == user.xposition && r->traps[i]->y == user.yposition){
                            user.health -= 10;
                            mvprintw(0 , 2 , "You stepped on a trap. your health is %d        " , user.health);
                            t = 1;
                        }
                    }
                }
                else if (mvinch(user.yposition , user.xposition - 1) == 'F'){
                    user.xposition--;
                    user.health += 10;
                    mvprintw(0 , 2 ,"You ate food. Your health is %d               " , user.health);
                }
                else if (mvinch(user.yposition , user.xposition - 1) == 'G'){
                    user.xposition--;
                    user.gold += (rand() % 5) + 1;
                    mvprintw(0 , 2 ,"You gained a bag of gold. You now have %d gold" , user.gold);
                }
                break;
            case KEY_RIGHT:
                if(check_weapon(user.yposition  , user.xposition + 1)){
                    user.xposition++;
                }
                else if (mvinch(user.yposition, user.xposition + 1) == '.'){
                    user.xposition++;
                    for(int i = 0; r->traps[i] != NULL ; i++){
                        if(r->traps[i]->x == user.xposition && r->traps[i]->y == user.yposition){
                            user.health -= 10;
                            mvprintw(0 , 2 , "You stepped on a trap. your health is %d        " , user.health);
                            t = 1;
                        }
                    }
                }
                else if (mvinch(user.yposition , user.xposition + 1) == 'F'){
                    user.xposition++;
                    user.health += 10;
                    mvprintw(0 , 2 ,"You ate food. Your health is %d               " , user.health);
                }
                else if (mvinch(user.yposition , user.xposition + 1) == 'G'){
                    user.xposition++;
                    user.gold += (rand() % 5) + 1;
                    mvprintw(0 , 2 ,"You gained a bag of gold. You now have %d gold" , user.gold);
                }
                break;
        }
        mvaddch(user.yposition, user.xposition, '@'); // Draw new position
        refresh();
    }
    FILE *fptr = fopen("save.txt", "a");
    fprintf(fptr , "x_position : %d     y_position: %d     health: %d    Gold: %d" , user.xposition , user.yposition , user.health , user.gold);
    fclose(fptr);
}

WINDOW *create_newwin(int height, int width, int starty, int startx) {
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    wborder(local_win, '|', '|', '-', '-', ' ', ' ', ' ', ' ');
    wrefresh(local_win);
    return local_win;
}
/*


void create_hallway(Position *door1, Position *door2) {
    int dx = abs(door2->x - door1->x);
    int dy = abs(door2->y - door1->y);

    int x = door1->x, y = door1->y;
    mvaddch(y, x, '#');

    if (dx >= dy) {
        // Move along x first
        int half_dx = dx / 2;
        while (half_dx--) {
            x += (door2->x > door1->x) ? 1 : -1;
            mvaddch(y, x, '#');
        }
        // Move fully along y
        while (y != door2->y) {
            y += (door2->y > door1->y) ? 1 : -1;
            mvaddch(y, x, '#');
        }
        // Finish x movement
        while (x != door2->x) {
            x += (door2->x > door1->x) ? 1 : -1;
            mvaddch(y, x, '#');
        }
    } else {
        // Move along y first
        int half_dy = dy / 2;
        while (half_dy--) {
            y += (door2->y > door1->y) ? 1 : -1;
            mvaddch(y, x, '#');
        }
        // Move fully along x
        while (x != door2->x) {
            x += (door2->x > door1->x) ? 1 : -1;
            mvaddch(y, x, '#');
        }
        // Finish y movement
        while (y != door2->y) {
            y += (door2->y > door1->y) ? 1 : -1;
            mvaddch(y, x, '#');
        }
    }
    refresh();
}





void create_hallway(Position * door1 , Position * door2) {
    int dx = abs(door1->x - door2->x);
    int dy = abs(door1->y - door2->y);
    
    int x = door1->x , y = door1->y;
    mvaddch(y , x , '#');

    if (dx >= dy) {
        int half_dx = dx / 2;
        while (half_dx--) {
            x += (door2->x > door1->x) ? 1 : -1;
            mvaddch(y , x , '#');
        }
        while (y != door2->y) {
            y += (door2->y > door2->y) ? 1 : -1;
            mvaddch(y , x , '#');
        }

        while (x != door2->x) {
            x += (door2->x > door1->x) ? 1 : -1;
            mvaddch(y , x , '#');
        }
    } else {
        int half_dy = dy / 2;
        while (half_dy--) {
            y += (door2->y > door1->y) ? 1 : -1;
            mvaddch(y , x , '#');
        }

        while (x != door2->x) {
            x += (door2->x > door1->x) ? 1 : -1;
            mvaddch(y , x , '#');
        }
        while (y != door2->y) {
            y += (door2->y > door1->y) ? 1 : -1;
            mvaddch(y , x , '#');
        }
    }
}

*/
