#include <fenv.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <sys/select.h>

// Constants definitions defined below
#define NUM_COLS 40
#define NUM_ROWS 20
#define true 1 
#define false 0
#define BASE_LENGTH 1

void initialise_board();
void collision_detection();
void spawn_fruit();
void move_snake();
void read_input(int input_char);

// Structs and enums for the game 

typedef enum {
        RIGHT,
        UP, 
        LEFT,
        DOWN
} direction;

typedef struct Coordinates{
        int x_pos;
        int y_pos;
        struct Coordinates *next;
        direction snake_direction;
} snake_struct;

typedef struct Board_Globals{
        int fruit_y;
        int fruit_x;
        int gameover;
        int score;
} board_global;

// Global variables below
snake_struct *snake_head = NULL;
snake_struct *snake_tail = NULL;
snake_struct *snake_part = NULL;
board_global board = {.gameover = 0, .score = 0};
int snake_size = 1; // The snake will start with only its head
int quit = 0;

void spawn_fruit()
{
        int lower_boundary = 1;
        
        state1:
                board.fruit_x = (rand() % (NUM_COLS - lower_boundary + 1)) + lower_boundary;
                board.fruit_y = (rand() % (NUM_ROWS - lower_boundary + 1)) + lower_boundary; 

                // Check if fruit isn't inside snake body, otherwise respawn
                for(snake_struct *tmp = snake_head; tmp != NULL; tmp = tmp->next){
                        if(tmp->y_pos == board.fruit_y && tmp->x_pos == board.fruit_x){
                                goto state1;
                        }
                }

                // Draw fruit on board_global
                attron(COLOR_PAIR(1));
                mvprintw(board.fruit_y, board.fruit_x, "*");
                attroff(COLOR_PAIR(1));
}

void initialise_board(){
        // Hide the cursor 
        //printw("\e[?25l");
        curs_set(0);

        // Render the table

        addch(ACS_ULCORNER);
        for(int col = 0; col < NUM_COLS; col++){
                addch(ACS_HLINE);
        }
        
        addch(ACS_URCORNER); printw("\n");

        for(int row = 0; row < NUM_ROWS; row++){
                addch(ACS_VLINE);
                for(int col = 0; col < NUM_COLS; col++){
                        printw(" ");
                }
                addch(ACS_VLINE); printw("\n");
        }

        addch(ACS_LLCORNER); 

        for(int col = 0; col < NUM_COLS; col++){
                addch(ACS_HLINE);
        } 
      
        addch(ACS_LRCORNER); 
        fflush(stdout);        

        // Draw head
        attron(COLOR_PAIR(2));
        mvprintw(snake_head->y_pos, snake_head->x_pos, ">");
        attroff(COLOR_PAIR(2));
        
        
        // Add the side border containing game info

        move(2, NUM_COLS + 2);
        for(int col = 0; col < 20; col++){
                addch(ACS_HLINE);
        }
        addch(ACS_URCORNER);

        for(int row = 1; row < 15; row++){
                move(row + 2, NUM_COLS + 22);
                addch(ACS_VLINE);
        }
        
        move(17, NUM_COLS + 2);
        for(int col = 0; col < 20; col++){
                addch(ACS_HLINE);
        }
        addch(ACS_LRCORNER);

        attron(COLOR_PAIR(3));
        mvprintw(4, NUM_COLS + 5, "Score: 0");
        mvprintw(6, NUM_COLS + 2, "Snake size increases");
        mvprintw(7, NUM_COLS + 2, "after score 2.");

        mvprintw(10, NUM_COLS + 2, "Made by Caidol");
        attroff(COLOR_PAIR(3));

        refresh(); 
}

void move_snake(){
        int temp_ypos, temp_xpos;
        int next_y; int next_x;
        direction previous_dir, temp_dir;
 
        next_x = snake_head->x_pos;
        next_y = snake_head->y_pos;
        
        attron(COLOR_PAIR(2));
        switch(snake_head->snake_direction){
                case RIGHT: 
                        snake_head->x_pos++;
                        mvprintw(snake_head->y_pos, snake_head->x_pos, ">");
                        break;
                case UP:
                        snake_head->y_pos--;
                        mvprintw(snake_head->y_pos, snake_head->x_pos, "^");
                        break;
                case LEFT:
                        snake_head->x_pos--;
                        mvprintw(snake_head->y_pos, snake_head->x_pos, "<"); 
                        break;
                case DOWN:
                        snake_head->y_pos++;
                        mvprintw(snake_head->y_pos, snake_head->x_pos, "v");
                        break;
        }
        attroff(COLOR_PAIR(2));

        previous_dir = snake_head->snake_direction; 
        
        if(snake_size < 2){
                mvprintw(next_y, next_x, " ");
        }
        else{
                for(snake_struct *tmp = snake_head->next; tmp != NULL; tmp = tmp->next){
                        temp_ypos = tmp->y_pos;
                        temp_xpos = tmp->x_pos;
                        temp_dir = tmp->snake_direction;
                        
                        // update the new position and direction for body 

                        tmp->snake_direction = previous_dir;
                        tmp->x_pos = next_x; 
                        tmp->y_pos = next_y;
                
                        if(tmp->next == NULL){
                                mvprintw(tmp->y_pos, tmp->x_pos, " ");
                        }
                        else{
                                attron(COLOR_PAIR(2));
                                mvprintw(tmp->y_pos, tmp->x_pos, "#");
                                attroff(COLOR_PAIR(2));
                        }

                        // Draw the body (ACS_BLOCK = "#") at the updated position

                        // set the new next position and direction as those prior to being updated 
                        // this informs the next part

                        next_x = temp_xpos;
                        next_y = temp_ypos;
                        previous_dir = temp_dir;
                }     
        }

        //refresh(); // TODO: Add refresh function into the main function
}

void grow_snake(){
        // Check for collision between the snake head and a fruit
        snake_struct *tmp;

        if(snake_head->x_pos == board.fruit_x && snake_head->y_pos == board.fruit_y){                
                snake_part = malloc(sizeof(snake_struct));
                
                if(snake_part != NULL && snake_size == 1){ // This occurs when there is only the snake head 
                        snake_part->snake_direction = snake_head->snake_direction;
                        snake_part->next = NULL;
                        snake_head->next = snake_part;
                        snake_tail = snake_part;
                }
                else if(snake_part != NULL){
                        snake_part->snake_direction = snake_tail->snake_direction;
                        snake_part->next = NULL;
                        snake_tail->next = snake_part;
                        snake_tail = snake_part;
                }

                // generate more snake fruit 
                spawn_fruit();
                
                snake_size++;
                board.score++;

                attron(COLOR_PAIR(3));
                mvprintw(4, NUM_COLS + 5, "Score: %d", board.score);
                //mvprintw(6, NUM_COLS + 4, "Note that size only increases after score two");
                attroff(COLOR_PAIR(3));
        } 

        // TODO: Increase the score  
}

void collision_detection(){
        // Collision detection for the boundary 
        
        if(snake_head->x_pos == 0 || snake_head->y_pos == 0 || snake_head->x_pos == NUM_COLS + 1 || snake_head->y_pos == NUM_ROWS + 1){
                board.gameover = 1;
                quit = 1;
                 
                //mvprintw(NUM_ROWS + 5, 0, "You have collided with the barrier!");
        } 
       
        for(snake_struct *tmp = snake_head->next; tmp != NULL; tmp = tmp->next){
                if(tmp->x_pos == snake_head->x_pos && tmp->y_pos == snake_head->y_pos){
                        board.gameover = 1;
                        quit = 1;
                }
        }
}

void read_input(int input_char){
        fflush(stdout); 
        struct timeval time_val;                        
        fd_set fds;
        
        // Initialise the time val seconds and microseconds
        time_val.tv_sec = 0;
        time_val.tv_usec = 0;

        FD_ZERO(&fds); // initialise the descriptor set pointed to by fd_set to the null set.
        FD_SET(STDIN_FILENO, &fds);
        select(STDIN_FILENO + 1, &fds, NULL, NULL, &time_val);
        
        if(FD_ISSET(STDIN_FILENO, &fds)){
                input_char = getch();                    
               
                switch(input_char){
                        case 27: // or ESC character
                                board.gameover = 1;
                                quit = 1;
                                break;
                        case 119: // w key 
                                if(snake_head->snake_direction != 1 && snake_head->snake_direction != 3){
                                        snake_head->snake_direction = 1;
                                }
                                break;
                        case 97: // a key 
                                if(snake_head->snake_direction != 2 && snake_head->snake_direction != 0){
                                        snake_head->snake_direction = 2;
                                }
                                break;
                        case 115: // s key 
                                if(snake_head->snake_direction != 3 && snake_head->snake_direction != 1){
                                        snake_head->snake_direction = 3;
                                }
                                break;
                        case 100: // d key 
                                if(snake_head->snake_direction != 0 && snake_head->snake_direction != 2){
                                        snake_head->snake_direction = 0;
                                }
                                break;
                        default:
                                break;
                }
        }
}

int main(void)
{
        // Initialise <ncurses.h>         
        initscr();
        cbreak();
        noecho();
        
        srand(time(NULL));

        // Allocate values to the snake head struct pointer
        snake_head = (snake_struct*) malloc(sizeof(snake_struct));
        snake_head->x_pos = NUM_COLS / 2;
        snake_head->y_pos = NUM_ROWS / 2;
        snake_head->snake_direction = RIGHT; 

        int chr;

        // Initialise colours 
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_MAGENTA, COLOR_BLACK);

        // Draw the initial board 
        initialise_board(); 
        
        // Generate initial fruit 
        spawn_fruit(); 
        refresh();

        while(!quit){ // TODO: add a board.gameover
                while(!board.gameover || !quit){
                        read_input(chr);
                        move_snake();
                        grow_snake();
                        collision_detection();
                        
                        refresh();

                        usleep(5 * 1000000 / 45);
                }
                
                if(board.gameover == 1){
                        mvprintw(NUM_ROWS + 2, 0, "GAME OVER!");
                        refresh();
                        sleep(3);
                }

                quit = 1;
        }

        endwin();
        
        // Show the cursor 
        //printf("\e[?25h");
        curs_set(1);
        fflush(stdout);

        free(snake_head); 
        free(snake_part);

        return 0;
}
