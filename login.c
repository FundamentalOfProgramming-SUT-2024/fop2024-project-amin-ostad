#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <regex.h>
#include <ctype.h>
#include <stdbool.h>
#include "map.h"

#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_EMAIL_LENGTH 50
#define NUM_OPTIONS 4

void first_page();


int user_login() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int height, width;
    getmaxyx(stdscr, height, width);
    WINDOW *login_win = newwin(10, 30, (height - 10) / 2, (width - 30) / 2);
    int login_success = 0;
    while(!login_success){
        box(login_win, 0, 0);
        mvwprintw(login_win, 1, 12, "Login");
        mvwprintw(login_win, 3, 2, "Username:");
        mvwprintw(login_win, 5, 2, "Password:");
        wrefresh(login_win);

        char username[MAX_USERNAME_LENGTH] = "";
        char password[MAX_PASSWORD_LENGTH] = "";

        echo();
        curs_set(1);
        wmove(login_win, 3, 12);
        wscanw(login_win, "%s", username);
        noecho();
        curs_set(0);

        wmove(login_win, 5, 12);
        int ch;
        int i = 0;
        while ((ch = wgetch(login_win)) != '\n' && i < MAX_PASSWORD_LENGTH - 1) {
            if (ch == KEY_BACKSPACE || ch == 127) {
                if (i > 0) {
                    i--;
                    mvwaddch(login_win, 5, 12 + i, ' ');
                    wmove(login_win, 5, 12 + i);
                    wrefresh(login_win);
                }
            } else {
                password[i++] = ch;
                mvwaddch(login_win, 5, 11 + i, '*');
                wrefresh(login_win);
            }
        }
        password[i] = '\0';
        FILE *read_fptr = fopen("userpass.txt", "r");
        char line[500];
        char user_check = 0;
        while(fgets(line , 500 , read_fptr)){
            char *user = strtok(line, ",");
            if(strcmp(user, username)== 0){
                user_check =1;
                char* pass = strtok(NULL , ",");
                if(strcmp(pass , password) == 0){
                    login_success = 1;
                }
                else{
                    mvprintw(1,0,"Wrong Password");
                    refresh();
                    break;
                }
            }
            strtok(NULL, ",");
        }
        if(login_success == 1){
            mvprintw(1 , 0 , "login successful!");
            clear();
            initialize_map();
            return 1;
        }
        if(user_check == 0){
                mvprintw( 1, 0, "Username not found");
                refresh();
                napms(2000);
                user_login();
                return 0;
            }
        if(login_success == 0){
            mvwprintw(login_win, 7, 8, "Invalid credentials.");
            mvprintw(3 ,0, "\n\nPress left to return\nPress F10 to try again");
            wrefresh(login_win);
            int retry_key = getch();
            if (retry_key == KEY_LEFT) {
                delwin(login_win);
                first_page();
                return 0;
            } 
            else if (retry_key == KEY_F(10)) {
                delwin(login_win);
                user_login();
                return 0;
            }
        }
    }
    delwin(login_win);

    return 0;
}


void print_menu(WINDOW *menu_win, int highlight, char *options[] , int options_count) {
    int x = 2, y = 2;

    box(menu_win, 0, 0);
    for (int i = 0; i < options_count; ++i) {
        if (highlight == i + 1) {
            wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, y, x, "%s", options[i]);
            wattroff(menu_win, A_REVERSE);
        } else {
            mvwprintw(menu_win, y, x, "%s", options[i]);
        }
        ++y;
    }
    wrefresh(menu_win);
}


int register_user(){
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int height, width;
    getmaxyx(stdscr, height, width);
    WINDOW *login_win = newwin(10, 30, (height - 10) / 2, (width - 30) / 2);
    int login_success = 0;
    while(!login_success){
        box(login_win, 0, 0);
        mvwprintw(login_win, 1, 12, "Login");
        mvwprintw(login_win, 3, 2, "Username:");
        mvwprintw(login_win, 5, 2, "Email:");
        mvwprintw(login_win, 7, 2, "Password:");
        wrefresh(login_win);

        char username[MAX_USERNAME_LENGTH] = "";
        char password[MAX_PASSWORD_LENGTH] = "";
        char email[MAX_EMAIL_LENGTH] = "";

        echo();
        curs_set(1);
        wmove(login_win, 3, 12);
        wscanw(login_win, "%s", username);
        wmove(login_win , 5 , 8);
        wscanw(login_win , "%s" , email);
        noecho();
        curs_set(0);

        wmove(login_win, 7, 12);
        int ch;
        int i = 0;
        while ((ch = wgetch(login_win)) != '\n' && i < MAX_PASSWORD_LENGTH - 1) {
            if (ch == KEY_BACKSPACE || ch == 127) {
                if (i > 0) {
                    i--;
                    mvwaddch(login_win, 7, 12 + i, ' ');
                    wmove(login_win, 7, 12 + i);
                    wrefresh(login_win);
                }
            } else {
                password[i++] = ch;
                mvwaddch(login_win, 7, 11 + i, '*');
                wrefresh(login_win);
            }
        }
        password[i] = '\0';
        echo();
        FILE *read_fptr = fopen("userpass.txt", "r");
        char line[500];
        int err = 0;
        if(strlen(username) < 7){
            mvprintw( 2,0 , "Username is too short!\n");
            mvprintw(4 ,0, "\n\nPress left to return\nPress F10 to retry");
            int retry_key = getch();
            if(retry_key == KEY_LEFT){
                delwin(login_win);
                first_page();
                return 0;
            }
            else if(retry_key == KEY_F(10)){
                delwin(login_win);
                register_user();
            }
            refresh();
            err = 1;
            napms(2000);
        }
        else{
            while(fgets(line, 500 , read_fptr)){
                char *user = strtok(line , ","); 
                if(username == user){
                    mvprintw( 2 , 0 , "Username already exists!\n");
                    refresh();
                    err = 1;
                }
                
                int a = 0; int b = 0; int d = 0;
                for(int l = 0; password[l] != '\0' ; l++){
                    if(isupper(password[l])) a = 1;
                    if(isdigit(password[l])) d = 1;
                    if(islower(password[l])) b = 1;
                }
                if(a != 1 || b != 1 || d != 1){
                    mvprintw(2 , 0 , "Invalid Password!\n");
                    err = 1;
                }
                regex_t regex;
                int result;
                const char *pattern = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$";
                result = regcomp(&regex, pattern, REG_EXTENDED);
                result = regexec(&regex, email, 0, NULL, 0);
                if(result != 0){
                    mvprintw(2 , 0 , "Invalid Email!\n");
                    err = 1;
                }
                if(err == 1){
                    mvprintw(4 ,0, "\n\nPress left to return\nPress F10 to retry");
                    wrefresh(login_win);
                    int retry_key = getch();
                    if (retry_key == KEY_LEFT) {
                        delwin(login_win);
                        first_page();
                        return 0;
                    } 
                    else if(retry_key == KEY_F(10)){
                        delwin(login_win);
                        register_user();
                    }
                }
                else {
                    mvprintw(2 , 0 , "signed up successfuly!");
                    refresh();
                    FILE* add_ftpr = fopen("userpass.txt" , "a");
                    fprintf( add_ftpr , "%s,%s,%s\n" , username , password , email);
                    fclose(add_ftpr);
                    return 1;
                }
            }
        }
    }
}




void print_scoreboard(){
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int height, width;
    getmaxyx(stdscr, height, width);
    WINDOW *win = newwin(15, 40, (height - 15) / 2, (width - 40) / 2);
    box(win , 0 ,0 );
    FILE* read_fptr = fopen("userpass.txt" , "r");
    char line[500];
    for(int i = 0 ; i < 3 && fgets(line, 500 , read_fptr) ; i++){
        char* user = strtok(line,"," );
        mvwprintw(win ,2*i+2 , 2 , "User: %s Score: %d Rank: %d" , user ,7*i+3, i+1);
    }
    wrefresh(win);
    refresh();
    getch();
    first_page();
    delwin(win);
}

void settings(){
        char *options[2] = {
        "Difficulity",
        "Change color",
    };

    int highlight = 1;
    int choice = 0;
    int c;

    initscr();
    clear();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    int height = 10, width = 30;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *menu_win = newwin(height, width, start_y, start_x);
    keypad(menu_win, TRUE);

    mvprintw(0, 0, "Use arrow keys to navigate. Press Enter to select.");
    refresh();
    print_menu(menu_win, highlight, options , 2);

    while (1) {
        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                if (highlight == 1) 
                    highlight = 2;
                else 
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == 2) 
                    highlight = 1;
                else 
                    ++highlight;
                break;
            case 10:
                choice = highlight;
                break;
            default:
                break;
        }

        print_menu(menu_win, highlight, options , 2);
        if (choice != 0)
            break;
    }

    mvprintw(LINES - 2, 0, "You chose option %d: %s", choice, options[choice - 1]);
    if(choice == 1){
        delwin(menu_win);
        mvprintw(LINES/2 , COLS/2 - 20 , "Enter a number from 1 to 5 to set difficulity");
        int difficulity = getch();
    }
    else if(choice == 2){
        //change_color();
    }
    refresh();
    getch();
    endwin();
}

void first_page() {
    char *options[NUM_OPTIONS] = {
        "Resume Game",
        "Settings",
        "New User",
        "Scoreboard"
    };

    int highlight = 1;
    int choice = 0;
    int c;

    initscr();
    clear();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    int height = 10, width = 30;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *menu_win = newwin(height, width, start_y, start_x);
    keypad(menu_win, TRUE);

    mvprintw(0, 0, "Use arrow keys to navigate. Press Enter to select.");
    refresh();
    print_menu(menu_win, highlight, options , NUM_OPTIONS);

    while (1) {
        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                if (highlight == 1) 
                    highlight = NUM_OPTIONS;
                else 
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == NUM_OPTIONS) 
                    highlight = 1;
                else 
                    ++highlight;
                break;
            case 10:
                choice = highlight;
                break;
            default:
                break;
        }

        print_menu(menu_win, highlight, options , NUM_OPTIONS);
        if (choice != 0)
            break;
    }

    mvprintw(LINES - 2, 0, "You chose option %d: %s", choice, options[choice - 1]);
    if(choice == 1){
        user_login();
    }
    else if(choice == 2){
        settings();
    }
    else if(choice == 3){
        register_user();
    }
    else if(choice == 4){
        print_scoreboard();
    }
    refresh();
    getch();
    endwin();
}

