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
} player;


void initialize_map();
WINDOW *create_newwin(int height, int width, int starty, int startx);
void detail_room(room *r);
void move_character(room *r);
void add_door_1(room *r);
void add_door_2(room *r);
void add_door_3(room *r);
void add_door_4(room *r);
void add_door_5(room *r);
void add_door_6(room *r);
