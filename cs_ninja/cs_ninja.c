// cs_ninja.c
// Written by Ethan Tong (z5691989) on 14/07/2025
//
// Description: CS Ninja is a small game where the player enters commands to 
// move our ninja on the board to paint any unpainted tiles and is inspired by 
// Ninja Painter.

// Provided Libraries
#include <stdio.h>
#include <ctype.h>
#include <string.h>
// Add your own #include statements below this line


// Provided constants
#define ROWS 10 
#define COLS 10

#define NO_NINJA -1

// Add your own #define constants below this line
#define TRUE 1
#define FALSE 0
#define MAX_SIZE 100

// Provided Enums
// Enum for features on the game board
enum entity {
    WALL,
    PAINT_BUCKET,
    EXIT_LOCKED,
    PAINTED,
    UNPAINTED,
    EXIT_UNLOCKED,
    LADDER,
    STAR,
    GLASS,
    TRAMPOLINE,
    EMPTY
};

enum colour {
    RED,
    YELLOW,
    GREEN,
    BLUE,
    PURPLE,
    NO_COLOUR
};

// Add your own enums below this line
enum direction {
    NORTH,
    EAST,
    SOUTH,
    WEST
};

// Represents a tile/cell on the game board
struct tile {
    enum entity entity;
    enum colour colour;
    enum direction direction1;
    enum direction direction2;
};

// Add your own structs below this line
struct ninja {
    int row;
    int col;
    int points;
    char paintbrush[MAX_SIZE];
    int win;
    int slamtoggle;
    char command;
    int total_unpainted;
    int initial_unpainted;
    int trampoline_num;
    int print_toggle;
};

struct feature {
    int row, col, colour, length;
    char direction;
};

struct stars {
    int num_stars;
};

struct statistics {
    int red_unpainted;
    int yellow_unpainted;
    int green_unpainted;
    int blue_unpainted;
    int purple_unpainted;
    int starpoints, unpaintedpoints;
    int points_remaining;
    int num_painted;
    double completion;
};

// Provided Function Prototypes
void initialise_board(struct tile board[ROWS][COLS]);
void print_board(
    struct tile board[ROWS][COLS], 
    int player_row, 
    int player_col
);
void print_board_line(void);
void print_tile_spacer(void);
void print_board_header(void);
char colour_to_char(enum colour colour);
void print_board_statistics (
    int num_red_unpainted_tiles,
    int num_yellow_unpainted_tiles,
    int num_green_unpainted_tiles, 
    int num_blue_unpainted_tiles,
    int num_purple_unpainted_tiles,
    double completion_percentage,
    int maximum_points_remaining
);

// Add your function prototypes below this line
void feature_spawn(struct tile board[ROWS][COLS]);
void ladder_logic(struct tile board[ROWS][COLS], struct feature feat);
int handling_errors(struct tile board[ROWS][COLS], int row, int col);
void entity_spawn_1(struct tile board[ROWS][COLS], 
                 struct feature feat, char command);
void entity_spawn_2(struct tile board[ROWS][COLS], 
                 struct feature feat, char command);
int handling_long(struct tile board[ROWS][COLS], struct feature feat);
int occupied_long(struct tile board[ROWS][COLS], struct feature feat);
int handling_colour(struct tile board[ROWS][COLS], struct feature feat);
void spawn_stars(struct tile board[ROWS][COLS]);
void star_logic(struct tile board[ROWS][COLS], struct stars stars);
void spawn_ninja(struct tile board[ROWS][COLS]);
void gameplay(struct tile board[ROWS][COLS], struct ninja ninja);
struct ninja ninja_interaction(struct tile board[ROWS][COLS], 
                               struct ninja ninja);
struct ninja ninja_interaction_2(struct tile board[ROWS][COLS], 
                                 struct ninja ninja);
struct ninja paintbrush_colour(struct tile board[ROWS][COLS], 
                               struct ninja ninja);
struct ninja gameplay_2(struct tile board[ROWS][COLS], 
                struct ninja ninja);
int check_paintbrush(struct tile board[ROWS][COLS], struct ninja ninja);
struct ninja check_unpainted(struct tile board[ROWS][COLS], 
                             struct ninja ninja);
void unlock_exits(struct tile board[ROWS][COLS]);
struct statistics stat_calc(struct tile board[ROWS][COLS], 
                            struct ninja ninja);
struct ninja unlock_condition(struct tile board[ROWS][COLS], 
                              struct ninja ninja);
void trampoline_helper(struct tile direction);
void trampoline_spawn(struct tile board[ROWS][COLS], struct feature feat);
struct ninja trampoline_interaction(struct tile board[ROWS][COLS], 
                                    struct ninja ninja);
struct ninja simple_movement(struct tile board[ROWS][COLS], struct ninja ninja);
struct ninja northeast_tramp(struct tile board[ROWS][COLS], struct ninja ninja);
struct ninja northwest_tramp(struct tile board[ROWS][COLS], struct ninja ninja);
struct ninja southeast_tramp(struct tile board[ROWS][COLS], struct ninja ninja);
struct ninja southwest_tramp(struct tile board[ROWS][COLS], struct ninja ninja);
struct ninja trampoline_blocked(struct tile board[ROWS][COLS], 
                                struct ninja ninja);
// Provided sample main() function (you will need to modify this)
int main(void) {
    struct tile board[ROWS][COLS];
    printf("Welcome to CS Ninja!\n\n");
    initialise_board(board);
    feature_spawn(board);
    print_board(board, NO_NINJA, NO_NINJA);
    spawn_stars(board);
    print_board(board, NO_NINJA, NO_NINJA);
    spawn_ninja(board);
    return 0;
}

// Add your function definitions below this line

// This function takes the input from the user. If the input passes
// their respective error tests, another function is called to spawn the 
// entities onto the board.
//
// parameters:
// - board: A 2D array that represents the game board.
//
// returns: nothing.
void feature_spawn(struct tile board[ROWS][COLS]) {
    char command = '\0';
    struct feature feat;
    printf("--- Setup Phase ---\n");
    while (command != 's' && scanf("%c", &command) == 1) {
        if (command == 'b' || command == 'u') {
            scanf(" %d %d %d", &feat.row, &feat.col, &feat.colour);
            if (handling_colour(board, feat)) {
            } else {
                entity_spawn_1(board, feat, command);
            }
        } else if (command == 'w' || command == 'e' || 
                   command == 'l' || command == 'g') {
            scanf(" %d %d", &feat.row, &feat.col);
            if (handling_errors(board, feat.row, feat.col)) {
            } else {
                entity_spawn_2(board, feat, command);
            }
        } else if (command == 'L') {
            scanf(" %c %d %d %d", &feat.direction, &feat.row, &feat.col
                    , &feat.length);
            if (handling_long(board, feat)) {
            } else {
                ladder_logic(board, feat);
            }
        } else if (command == 't') {
            trampoline_spawn(board, feat);
        }
    }
}

void trampoline_spawn(struct tile board[ROWS][COLS], struct feature feat) {
    int dir1, dir2;
    scanf(" %d %d %d %d", &feat.row, &feat.col, &dir1, &dir2);
    if (handling_errors(board, feat.row, feat.col)) {
    } else {
        board[feat.row][feat.col].entity = TRAMPOLINE;
        board[feat.row][feat.col].direction1 = dir1;
        board[feat.row][feat.col].direction2 = dir2;
    }
}
// This function includes if statements for whether the entity 
// is PAINT_BUCKET or UNPAINTED, given the user command. 
// 
// parameters:
// - board: A 2D array that represents the game board.
// - feature: A struct that holds all the details about an entity.
// - command: A character that the user inputted which represents an entity.
// 
// returns: nothing.
void entity_spawn_1(struct tile board[ROWS][COLS], 
                 struct feature feat, char command) {
    if (command == 'b') {
        board[feat.row][feat.col].entity = PAINT_BUCKET;
        board[feat.row][feat.col].colour = feat.colour;
    } else if (command == 'u') {
        board[feat.row][feat.col].entity = UNPAINTED;
        board[feat.row][feat.col].colour = feat.colour;
    }
}

// This function includes if statements for whether the entity is WALL, 
// EXIT_LOCKED or LADDER, given the user command. 
// 
// parameters:
// - board: A 2D array that represents the game board.
// - feature: A struct that holds all the details about an entity.
// - command: A character that the user inputted which represents an entity.
// 
// returns: nothing.
void entity_spawn_2(struct tile board[ROWS][COLS],
                  struct feature feat, char command) {
    if (command == 'w') {
        board[feat.row][feat.col].entity = WALL;
    } else if (command == 'e') {
        board[feat.row][feat.col].entity = EXIT_LOCKED;
    } else if (command == 'l') {
        board[feat.row][feat.col].entity = LADDER;
    } else if (command == 'g') {
        board[feat.row][feat.col].entity = GLASS;
    }
}

// This function adds a ladder to the right or bottom, based on whether the
// user inputted 'h' or 'v' as their direction.
// 
// parameters:
// - board: A 2D array that represents the game board.
// - feature: A struct that holds all the details about an entity.
// 
// returns: nothing. 
void ladder_logic(struct tile board[ROWS][COLS], struct feature feat) {
    int i = 0;
    while (i < feat.length) {
        board[feat.row][feat.col].entity = LADDER;
        if (feat.direction == 'h') {
            feat.col++;
            i++;
        } else {
            feat.row++;
            i++;
        }
    }
}

// This function handles the errors for entities that only require rows and 
// columns to spawn. 
// 
// parameters:
// - board: A 2D array that represents the game board.
// - row: the row the entity spawns in.
// - col: the column the entity spawns in.
// 
// returns: TRUE or FALSE
int handling_errors(struct tile board[ROWS][COLS], int row, int col) {
    if (row > ROWS - 1 || col > COLS - 1 || 
        row < 0 || col < 0) {
        printf("ERROR: Invalid position, %d %d is out of bounds!\n"
                , row, col);
        return TRUE;
    } else if (board[row][col].entity != EMPTY) {
        printf("ERROR: Invalid tile, %d %d is occupied!\n", row
                , col);
        return TRUE;
    } else {
        return FALSE;
    }
}

// This function handles the errors for entities that require rows, columns
// and colours to spawn. 
// 
// parameters:
// - board: A 2D array that represents the game board.
// - feature: A struct that holds all the details about an entity.
// 
// returns: TRUE or FALSE
int handling_colour(struct tile board[ROWS][COLS], struct feature feat) {
    if (feat.row > ROWS - 1 || feat.col > COLS - 1 || 
        feat.row < 0 || feat.col < 0) {
        printf("ERROR: Invalid position, %d %d is out of bounds!\n"
                , feat.row, feat.col);
        return TRUE;
    } else if (board[feat.row][feat.col].entity != EMPTY) {
        printf("ERROR: Invalid tile, %d %d is occupied!\n", feat.row
                , feat.col);
        return TRUE;
    } else if (feat.colour > 5 || feat.colour < 0) {
        printf("ERROR: Invalid colour!\n");
        return TRUE;
    } else {
        return FALSE;
    }
}

// This function handles the out of bounds errors for the long ladder
//
// parameters:
// - board: A 2D array that represents the game board.
// - feature: A struct that holds all the details about an entity.
//
// returns: TRUE or FALSE
int handling_long(struct tile board[ROWS][COLS], struct feature feat) {
    if (feat.row > ROWS - 1|| feat.col > COLS - 1|| 
        feat.row < 0 || feat.col < 0) {
        printf("ERROR: Invalid position, %d %d is out of bounds!\n"
                , feat.row, feat.col);
        return TRUE;
    } else if (feat.direction == 'h' && feat.col + feat.length >= ROWS - 1) {
        printf("ERROR: Invalid position, part of the ladder is out of bounds!");
        printf("\n");
        return TRUE;
    } else if (feat.direction == 'v' && feat.row + feat.length >= COLS - 1) {
        printf("ERROR: Invalid position, part of the ladder is out of bounds!");
        printf("\n");
        return TRUE;
    } else if (occupied_long(board, feat)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

// This function checks if part of the long ladder is occupied
//
// parameters:
// - board: A 2D array that represents the game board.
// - feature: A struct that holds all the details about an entity.
//
// returns: TRUE or FALSE
int occupied_long(struct tile board[ROWS][COLS], struct feature feat) {
    int i = 0;
    while (i < feat.length) {
        if (feat.direction == 'h') {
            feat.col++;
            if (board[feat.row][feat.col].entity != EMPTY) {
                printf("ERROR: Invalid tile, part of the ladder is occupied!");
                printf("\n");
                return TRUE;
            }
            i++;
        } else {
            feat.row++;
            if (board[feat.row][feat.col].entity != EMPTY) {
                printf("ERROR: Invalid tile, part of the ladder is occupied!");
                printf("\n");
                return TRUE;
            }
            i++;
        }
    }
    return FALSE;
}

// This function asks the user to input the number of stars they want to have on
// the board.
//
// parameters:
// - board: A 2D array that represents the game board.
//
// returns: nothing. 
void spawn_stars(struct tile board[ROWS][COLS]) {
    int switch1 = 0;
    struct stars stars;
    printf("--- Adding Stars ---\n");
    printf("Enter the number of stars: ");
    while (switch1 == 0) {
        scanf("%d", &stars.num_stars);
        if (stars.num_stars < 0) {
            printf("ERROR: Invalid number of stars!\n");
        } else {
            star_logic(board, stars);
            switch1 = 1;
        }
    }
    
}

// This function takes the coordinates of the stars from user input and 
// adds them to the board
//
// parameters:
// - board: A 2D array that represents the game board.
// - stars: A struct that contains information on the entity "stars"
//
// returns: nothing.
void star_logic(struct tile board[ROWS][COLS], struct stars stars) {
    struct feature feat;
    int counter = 0;
    while (counter < stars.num_stars) { 
        scanf("%d %d", &feat.row, &feat.col);
        if (handling_errors(board, feat.row, feat.col)) {
        } else {
            board[feat.row][feat.col].entity = STAR;
            counter++;
        }
    }
}

// This function takes coordinates given by the user and spawns the ninja
// if it has no errors.
//
// parameters:
// - board: A 2D array that represents the game board.
//
// returns: nothing.
void spawn_ninja(struct tile board[ROWS][COLS]) {
    struct ninja ninja;
    int switch2 = 0;
    printf("--- Spawning Ninja ---\n");
    while (switch2 == 0) {
        scanf("%d %d", &ninja.row, &ninja.col);
        if (handling_errors(board, ninja.row, ninja.col)) {
        } else {
            print_board(board, ninja.row, ninja.col);
            switch2 = 1;
        }
    }
    ninja.win = 0;
    ninja.points = 0;
    gameplay(board, ninja);
}

// This function manages the main gameplay loop, handling user input, ninja
// movement, game win or loss condition and printing the board.
//
// parameters:
// - board: A 2D array that represents the game board.
// - ninja: A struct representing the ninja, its position, points and state.
//
// returns: nothing.
void gameplay(struct tile board[ROWS][COLS], struct ninja ninja) {
    printf("--- Gameplay Phase ---\n");
    strcpy(ninja.paintbrush, "NO COLOUR");
    int total = 0;

    // Counts the number of unpainted tiles on the board.
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (board[row][col].entity == UNPAINTED) {
                total++;
            }
        }
    }
    ninja.initial_unpainted = total;
    ninja = unlock_condition(board, ninja);

    // Main gameplay loop, continues until player wins, quits or loses.
    while (ninja.win == 0 && scanf(" %c", &ninja.command) == 1 && 
            ninja.command != 'q') {
        ninja.trampoline_num = 0;
        ninja.print_toggle = 1;
        ninja = simple_movement(board, ninja);
        if (ninja.print_toggle == 1) {
            print_board(board, ninja.row, ninja.col);
        }
    }

    // Game end conditions.
    if (ninja.command == 'q') {
        printf("--- Quitting Game ---\n");
    } else if (ninja.win == 1) {
        printf("--- Game Over ---\n");
        printf("Congratulations!\n");
    } else if (ninja.win == 2) {
        printf("--- Game Over ---\n");
        printf("You lost!\n");
        ninja.command = 'm';
        gameplay_2(board, ninja);
    }
}

// This function performs a single movement of the ninja based on the command.
// Updates ninja position, then handles tile interactions and trampoline-blocked
// logic.
//
// parameters:
// - board: A 2D array representing the game board.
// - ninja: A struct representing the ninja, its position, command and state.
//
// returns: the updated ninja struct after handling movement and interactions.
struct ninja simple_movement(struct tile board[ROWS][COLS], 
                             struct ninja ninja) {
    ninja.slamtoggle = 0;

    // Move the ninja in the direction specified by the command.
    if (ninja.command == 'w') {
        ninja.row--;
        ninja = ninja_interaction(board, ninja);
    } else if (ninja.command == 'a') {
        ninja.col--;
        ninja = ninja_interaction(board, ninja);
    } else if (ninja.command == 's') {
        ninja.row++;
        ninja = ninja_interaction(board, ninja);
    } else if (ninja.command == 'd') {
        ninja.col++;
        ninja = ninja_interaction(board, ninja);
    } else {
        // this function handles ninja slam and non-movement commands.
        return gameplay_2(board, ninja);
    }

    // Helps move the ninja before the trampoline, after being blocked by a 
    // wall or glass. 
    if (ninja.slamtoggle == 1 && ninja.win != 2 &&
        board[ninja.row][ninja.col].entity == TRAMPOLINE) {
        ninja = trampoline_blocked(board, ninja);
    }
    return ninja;
}

// This function checks whether all unpainted tiles have been painted.
// If so, it unlocks the exits on the board.
//
// parameters:
// - board: A 2D array representing the game board.
// - ninja: A struct representing the ninja, which tracks the number of 
//          unpainted tiles.
//
// returns: the updated ninja struct with potentially updated unpainted count.
struct ninja unlock_condition(struct tile board[ROWS][COLS], 
                              struct ninja ninja) {
    // Check how many unpainted tiles remain
    ninja = check_unpainted(board, ninja);

    // If there are still unpainted tiles, re-check.
    if (ninja.total_unpainted > 0) {
        ninja = check_unpainted(board, ninja);
    } else {
        // If all tiles are painted, unlock exits.
        unlock_exits(board);
        ninja.total_unpainted = 0;
    }
    return ninja;
}

// This function handles extended gameplay commands, such as ninja slam and
// others. It also manages trampoline behavior after a slam.
//
// parameters:
// - board: A 2D array representing the game board.
// - ninja: A struct representing the ninja's state and current command. 
//
// returns: the updated ninja struct after processing the extended command.
struct ninja gameplay_2(struct tile board[ROWS][COLS], 
                        struct ninja ninja) {
    struct statistics stat;
    ninja.slamtoggle = 0;
    // Handles ninja slam in each direction until the ninja hits something.
    if (ninja.command == 'W') {
        while (ninja.slamtoggle == 0) {
            ninja.row--;
            ninja = ninja_interaction(board, ninja);
        }
    } else if (ninja.command == 'A') {
        while (ninja.slamtoggle == 0) {
            ninja.col--;
            ninja = ninja_interaction(board, ninja);
        }
    } else if (ninja.command == 'S') {
        while (ninja.slamtoggle == 0) {
            ninja.row++;
            ninja = ninja_interaction(board, ninja);
        }
    } else if (ninja.command == 'D') {
        while (ninja.slamtoggle == 0) {
            ninja.col++;
            ninja = ninja_interaction(board, ninja);
        }
        // shows current paintbrush.
    } else if (ninja.command == 'i') {
        printf("Holding ");
        fputs(ninja.paintbrush, stdout);
        printf(" paintbrush!\n");
        ninja.print_toggle = 0;
        // shows amount of points.
    } else if (ninja.command == 'p') {
        printf("You have %d point(s)!\n", ninja.points);
        ninja.print_toggle = 0;
        // prints board statistics.
    } else if (ninja.command == 'm') {
        stat = stat_calc(board, ninja);
        print_board_statistics(stat.red_unpainted, stat.yellow_unpainted,
            stat.green_unpainted, stat.blue_unpainted, stat.purple_unpainted,
            stat.completion, stat.points_remaining);
        ninja.print_toggle = 0;
    }
    if (ninja.slamtoggle == 1 && ninja.win != 2 &&
        board[ninja.row][ninja.col].entity == TRAMPOLINE) {
        ninja = trampoline_blocked(board, ninja);
    }
    return ninja;
}

// This function handles the ninja's interaction with the tile they move onto.
// It checks for collision with walls, glass, boundaries, or special entities
// like exits and ladders, and applies effects accordingly.
//
// parameters:
// - board: A 2D array representing the game board.
// - ninja: A struct representing the ninja's current position, command, 
//          and state.
//
// returns: the updated ninja struct after processing the interaction.
struct ninja ninja_interaction(struct tile board[ROWS][COLS], 
                                struct ninja ninja) {
    if (ninja.row > ROWS - 1|| ninja.col > COLS - 1|| 
        ninja.row < 0 || ninja.col < 0) {
        ninja.win = 2;
        ninja.slamtoggle = 1;
    } else if ((board[ninja.row][ninja.col].entity == WALL || 
        board[ninja.row][ninja.col].entity == GLASS) && 
        (ninja.command == 'w' || ninja.command == 'W')) {
        if (board[ninja.row][ninja.col].entity == GLASS) {
            board[ninja.row][ninja.col].entity = EMPTY;
        }        
        ninja.row++;
        ninja.slamtoggle = 1;
    } else if ((board[ninja.row][ninja.col].entity == WALL || 
        board[ninja.row][ninja.col].entity == GLASS) && 
        (ninja.command == 'a'|| ninja.command == 'A')) {
        if (board[ninja.row][ninja.col].entity == GLASS) {
            board[ninja.row][ninja.col].entity = EMPTY;
        }        
        ninja.col++;    
        ninja.slamtoggle = 1;
    } else if ((board[ninja.row][ninja.col].entity == WALL ||
        board[ninja.row][ninja.col].entity == GLASS) && 
        (ninja.command == 's' || ninja.command == 'S')) {
        if (board[ninja.row][ninja.col].entity == GLASS) {
            board[ninja.row][ninja.col].entity = EMPTY;
        }        
        ninja.row--;
        ninja.slamtoggle = 1;
    } else if ((board[ninja.row][ninja.col].entity == WALL ||
        board[ninja.row][ninja.col].entity == GLASS) && 
        (ninja.command == 'd' || ninja.command == 'D')) {
        if (board[ninja.row][ninja.col].entity == GLASS) {
            board[ninja.row][ninja.col].entity = EMPTY;
        }        
        ninja.col--;
        ninja.slamtoggle = 1;
    } else if (board[ninja.row][ninja.col].entity == EXIT_UNLOCKED) {
        // Ninja wins when unlocked exit reached
        ninja.win = 1; 
        ninja.slamtoggle = 1;
    } else if (board[ninja.row][ninja.col].entity == LADDER) {
        // Stop the slam if ninja lands on ladder
        ninja.slamtoggle = 1;
    } else {
        ninja = ninja_interaction_2(board, ninja);
    }
    return ninja;
}

// This function handles more interactions for the ninja when they land on
// unpainted, paint bucket, star, exit unlocked and trampoline tiles. 
//
// parameters:
// - board: A 2D array representing the game board.
// - ninja: A struct representing the ninja's current state and position.
//
// returns: the updated ninja struct after processing interactions.
struct ninja ninja_interaction_2(struct tile board[ROWS][COLS], 
                                 struct ninja ninja) {
    // Handles painting logic
    if (board[ninja.row][ninja.col].entity == UNPAINTED &&
        check_paintbrush(board, ninja)) {
        board[ninja.row][ninja.col].entity = PAINTED;
        ninja.points++;
        ninja = check_unpainted(board, ninja);
        // If there are no more unpainted tiles, unlock the exits
        if (ninja.total_unpainted) {
        } else {
            unlock_exits(board);
        }
        // If the tile is a paint bucket change the ninja's brush colour
    } else if (board[ninja.row][ninja.col].entity == PAINT_BUCKET) {
        ninja = paintbrush_colour(board, ninja);
        // If the tile is a star, collect it and increase the score
    } else if (board[ninja.row][ninja.col].entity == STAR) {
        ninja.points += 20;
        board[ninja.row][ninja.col].entity = EMPTY;
        // If the ninja reaches an exit, trigger the win condition
    } else if (board[ninja.row][ninja.col].entity == EXIT_UNLOCKED) {
        ninja.win = 1;
        ninja.slamtoggle = 1;
    }
    // If the tile is a trampoline, initiate trampoline interaction logic
    if (board[ninja.row][ninja.col].entity == TRAMPOLINE) { 
        ninja.trampoline_num++;
        ninja = trampoline_interaction(board, ninja);
    }
    return ninja;
}

// This function handles the case where the ninja attempts to move into a
// trampoline but the next move from the trampoline is blocked, thus the ninja
// be moved right before the trampoline.
//
// parameters:
// - board: A 2D array representing the game board.
// - ninja: A struct representing the ninja's current state and position.
//
// returns: the updated ninja struct
struct ninja trampoline_blocked(struct tile board[ROWS][COLS], 
                                struct ninja ninja) {
    int dir1 = board[ninja.row][ninja.col].direction1;
    int dir2 = board[ninja.row][ninja.col].direction2;
    // Check for NORTH EAST trampoline
    if ((dir1 == NORTH && dir2 == EAST) || 
        (dir1 == EAST && dir2 == NORTH)) {
        // undoing the trampoline move
        if (ninja.command == 'd' || ninja.command == 'D') {
            ninja.row--;
        } else if (ninja.command == 'w' || ninja.command == 'W') {
            ninja.col++;
        }
        // Check for NORTH WEST trampoline
    } else if ((dir1 == NORTH && dir2 == WEST) || 
                (dir1 == WEST && dir2 == NORTH)) {
        if (ninja.command == 'a' || ninja.command == 'A') {
            ninja.row--;
        } else if (ninja.command == 'w' || ninja.command == 'W' ) {
            ninja.col--;
        }
        // Check for SOUTH EAST trampoline
    } else if ((dir1 == SOUTH && dir2 == EAST) || 
                (dir1 == EAST && dir2 == SOUTH)) {
        if (ninja.command == 's' || ninja.command == 'S') {
            ninja.col++;
        } else if (ninja.command == 'd' || ninja.command == 'D') {
            ninja.row++;
        }
        // Check for SOUTH WEST trampoline
    } else {
        if (ninja.command == 's' || ninja.command == 'S') {
            ninja.col--;
        } else if (ninja.command == 'a' || ninja.command == 'A') {
            ninja.row++;
        }
    }
    return ninja;
}

// This function handles the trampoline interaction logic based on its 
// directions. It checks the two directions assigned to the trampoline 
// and calls the appropriate functions for each trampoline direction.
//
// parameters:
// - board: A 2D array representing tha game baord.
// - ninja: A struct representing the ninja's current state and position.
//
// returns: the updated ninja struct after applying trampoline movement.
struct ninja trampoline_interaction(struct tile board[ROWS][COLS], 
                                    struct ninja ninja) {
    int dir1 = board[ninja.row][ninja.col].direction1;
    int dir2 = board[ninja.row][ninja.col].direction2;
    // If the trampoline launches NORTH and EAST, call northeast handler
    if ((dir1 == NORTH && dir2 == EAST) || 
        (dir1 == EAST && dir2 == NORTH)) {
        ninja = northeast_tramp(board, ninja);
        return ninja;
        // If the trampoline launched NORTH and WEST, call northwest handler
    } else if ((dir1 == NORTH && dir2 == WEST) || 
                (dir1 == WEST && dir2 == NORTH)) {
        ninja = northwest_tramp(board, ninja);
        return ninja;
        // If the trampoline launched SOUTH and EAST, call southeast handler
    } else if ((dir1 == SOUTH && dir2 == EAST) || 
                (dir1 == EAST && dir2 == SOUTH)) {
        ninja = southeast_tramp(board, ninja);
        return ninja;
        // If the trampoline launches SOUTH and WEST, call southwest handler
    } else {
        ninja = southwest_tramp(board, ninja);
        return ninja;
    }
}

// This function handles the trampoline interaction when the trampoline launches
// the ninja in the north or east direction depending on where the ninja 
// enters from.
//
// parameters:
// - board: A 2D array representing the game board.
// - ninja: A struct representing the ninja's current state and position.
//
// returns: the updated ninja struct after applying the northeast logic.
struct ninja northeast_tramp(struct tile board[ROWS][COLS], 
                             struct ninja ninja) {
    // Stop before the trampoline if 3 trampolines have already been hit
    // (trampoline_num is 1 by default) otherwise, switch the direction that
    // the ninja moves 
    if (ninja.command == 's') {
        if (ninja.trampoline_num == 4) {
            ninja.row--;
        } else {
            ninja.command = 'd';
            ninja = simple_movement(board, ninja);
        }
    } else if (ninja.command == 'S') {
        if (ninja.trampoline_num == 4) {
            ninja.row--;
            ninja.slamtoggle = 1;
        } else {
            ninja.command = 'D';
            ninja = simple_movement(board, ninja);
        }
    } else if (ninja.command == 'a') {
        if (ninja.trampoline_num == 4) {
            ninja.col++;
        } else {
            ninja.command = 'w';
            ninja = simple_movement(board, ninja);
        }
    } else if (ninja.command == 'A') {
        if (ninja.trampoline_num == 4) {
            ninja.col++;
            ninja.slamtoggle = 1;
        } else {
            ninja.command = 'W';
            ninja = simple_movement(board, ninja);
        }
        // If ninja comes from the wrong direction to the trampoline, it acts 
        // like a wall
    } else if (ninja.command == 'w' || ninja.command == 'W') {
        ninja.row++;
        ninja.slamtoggle = 1;
    } else if (ninja.command == 'd' || ninja.command == 'D') {
        ninja.col--;
        ninja.slamtoggle = 1;
    }
    return ninja;
}

// This function handles the trampoline interaction when the trampoline launches
// the ninja in either the north or the west direction depending on where the
// ninja enters from.
//
// parameters:
// - board: A 2D array representing the game board.
// - ninja: A struct representing the ninja's current state and position.
//
// returns: the updated ninja struct after applying the northwest logic.
struct ninja northwest_tramp(struct tile board[ROWS][COLS], 
                             struct ninja ninja) {
    // Stop before the trampoline if 3 trampolines have already been hit
    // (trampoline_num is 1 by default) otherwise, switch the direction that
    // the ninja moves 
    if (ninja.command == 's') {
        if (ninja.trampoline_num == 4) {
            ninja.row--;
        } else {
            ninja.command = 'a';
            ninja = simple_movement(board, ninja);
        }        
    } else if (ninja.command == 'S') {
        if (ninja.trampoline_num == 4) {
            ninja.row--;
            ninja.slamtoggle = 1;
        } else {
            ninja.command = 'A';
            ninja = simple_movement(board, ninja);
        }
    } else if (ninja.command == 'd') {
        if (ninja.trampoline_num == 4) {
            ninja.col--;
        } else {
            ninja.command = 'w';
            ninja = simple_movement(board, ninja);
        }        
    } else if (ninja.command == 'D') {
        if (ninja.trampoline_num == 4) {
            ninja.col--;
            ninja.slamtoggle = 1;
        } else {
            ninja.command = 'W';
            ninja = simple_movement(board, ninja);
        }    
        // If ninja comes from the wrong direction to the trampoline, it acts 
        // like a wall    
    } else if (ninja.command == 'w' || ninja.command == 'W') {
        ninja.row++;
        ninja.slamtoggle = 1;
    } else if (ninja.command == 'a' || ninja.command == 'A') {
        ninja.col++;
        ninja.slamtoggle = 1;
    }
    return ninja;
}

// This function handles the trampoline interaction when the trampoline launches
// the ninja in either the south or the east direction depending on where the
// ninja enters from.
//
// parameters:
// - board: A 2D array representing the game board.
// - ninja: A struct representing the ninja's current state and position.
//
// returns: the updated ninja struct after applying the south east logic.
struct ninja southeast_tramp(struct tile board[ROWS][COLS], 
                             struct ninja ninja) {
    // Stop before the trampoline if 3 trampolines have already been hit
    // (trampoline_num is 1 by default) otherwise, switch the direction that
    // the ninja moves 
    if (ninja.command == 'w') {
        if (ninja.trampoline_num == 4) {
            ninja.row++;
        } else {
            ninja.command = 'd';
            ninja = simple_movement(board, ninja);
        }        
    } else if (ninja.command == 'W') {
        if (ninja.trampoline_num == 4) {
            ninja.row++;
            ninja.slamtoggle = 1;
        } else {
            ninja.command = 'D';
            ninja = simple_movement(board, ninja);
        }        
    } else if (ninja.command == 'a') {
        if (ninja.trampoline_num == 4) {
            ninja.col++;
        } else {
            ninja.command = 's';
            ninja = simple_movement(board, ninja);
        }        
    } else if (ninja.command == 'A') {
        if (ninja.trampoline_num == 4) {
            ninja.col++;
            ninja.slamtoggle = 1;
        } else {
            ninja.command = 'S';
            ninja = simple_movement(board, ninja);
        }        
        // If ninja comes from the wrong direction to the trampoline, it acts 
        // like a wall 
    } else if (ninja.command == 's' || ninja.command == 'S') {
        ninja.row--;
        ninja.slamtoggle = 1;
    } else if (ninja.command == 'd' || ninja.command == 'D') {
        ninja.col--;
        ninja.slamtoggle = 1;
    }
    return ninja;
}

// This function handles the trampoline interaction when the trampoline launches
// the ninja in either the south or the west direction depending on where the
// ninja enters from.
//
// parameters:
// - board: A 2D array representing the game board.
// - ninja: A struct representing the ninja's current state and position.
//
// returns: the updated ninja struct after applying the south west logic.
struct ninja southwest_tramp(struct tile board[ROWS][COLS], 
                             struct ninja ninja) {
    // Stop before the trampoline if 3 trampolines have already been hit
    // (trampoline_num is 1 by default) otherwise, switch the direction that
    // the ninja moves 
    if (ninja.command == 'w') {
        if (ninja.trampoline_num == 4) {
            ninja.row++;
        } else {
            ninja.command = 'a';
            ninja = simple_movement(board, ninja);
        }        
    } else if (ninja.command == 'W') {
        if (ninja.trampoline_num == 4) {
            ninja.row++;
            ninja.slamtoggle = 1;
        } else {
            ninja.command = 'A';
            ninja = simple_movement(board, ninja);
        }        
    } else if (ninja.command == 'd') {
        if (ninja.trampoline_num == 4) {
            ninja.col--;
        } else {
            ninja.command = 's';
            ninja = simple_movement(board, ninja);
        }        
    } else if (ninja.command == 'D') {
        if (ninja.trampoline_num == 4) {
            ninja.col--;
            ninja.slamtoggle = 1;
        } else {
            ninja.command = 'S';
            ninja = simple_movement(board, ninja);
        }        
        // If ninja comes from the wrong direction to the trampoline, 
        // it acts like a wall 
    } else if (ninja.command == 's' || ninja.command == 'S') {
        ninja.row--;
        ninja.slamtoggle = 1;
    } else if (ninja.command == 'a' || ninja.command == 'A') {
        ninja.col++;
        ninja.slamtoggle = 1;
    }
    return ninja;
}

// This function updates the ninja's paintbrush colour based on the colour of
// the paint bucket tile the ninja is standing on.
//
// parameters:
// - board: A 2D array representing the game board.
// - ninja: A struct representing the ninja's current state and position.
//
// returns: the updated ninja struct with the new paintbrush colour.
struct ninja paintbrush_colour(struct tile board[ROWS][COLS], 
                               struct ninja ninja) {
    if (board[ninja.row][ninja.col].colour == RED) {
        strcpy(ninja.paintbrush, "RED");
    } else if (board[ninja.row][ninja.col].colour == YELLOW) {
        strcpy(ninja.paintbrush, "YELLOW");
    } else if (board[ninja.row][ninja.col].colour == GREEN) {
        strcpy(ninja.paintbrush, "GREEN");
    } else if (board[ninja.row][ninja.col].colour == BLUE) {
        strcpy(ninja.paintbrush, "BLUE");
    } else if (board[ninja.row][ninja.col].colour == PURPLE) {
        strcpy(ninja.paintbrush, "PURPLE");
    }
    return ninja;
}

// This function checks if the colour of the tile the ninja is currently
// standing on matches the colour of the ninja's paintbrush.
//
// parameters:
// - board: A 2D array representing the game board.
// - ninja: A struct containing the ninja's current position and paintbrush
//          colour
//
// returns: TRUE if the colours match, FALSE otherwise.
int check_paintbrush(struct tile board[ROWS][COLS], struct ninja ninja) {
    char currentcolour[MAX_SIZE];
    if (board[ninja.row][ninja.col].colour == RED) {
        strcpy(currentcolour, "RED");
    } else if (board[ninja.row][ninja.col].colour == YELLOW) {
        strcpy(currentcolour, "YELLOW");
    } else if (board[ninja.row][ninja.col].colour == GREEN) {
        strcpy(currentcolour, "GREEN");
    } else if (board[ninja.row][ninja.col].colour == BLUE) {
        strcpy(currentcolour, "BLUE");
    } else if (board[ninja.row][ninja.col].colour == PURPLE) {
        strcpy(currentcolour, "PURPLE");
    }
    
    if (strcmp(ninja.paintbrush, currentcolour) == 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}

// This function counts the number of unpainted tiles remaining on the board
//
// parameters:
// - board: A 2D array representing the game board.
// - ninja: A struct representing the ninja.
//
// returns: The updated ninja struct with new total of unpainted tiles.
struct ninja check_unpainted(struct tile board[ROWS][COLS], 
                             struct ninja ninja) {
    ninja.total_unpainted = 0;
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (board[row][col].entity == UNPAINTED) {
                ninja.total_unpainted++;
            } 
        }
    }
    return ninja;
}

// This function unlocks all exit tiles on the board.
//
// parameters:
// - board: A 2D array representing the game board.
//
// returns: nothing. 
void unlock_exits(struct tile board[ROWS][COLS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (board[row][col].entity == EXIT_LOCKED) {
                board[row][col].entity = EXIT_UNLOCKED;
            }
        }
    }
}

// This function calculates various game statistics based on the current state 
// of the board and the ninja's initial unpainted tile count.
//
// parameters:
// - board: A 2D array representing the game board.
// - ninja: A struct containing the ninja's stats, including initial unpainted 
//          tiles.
//
// returns: A statistics struct containing the calculated game statistics.
struct statistics stat_calc(struct tile board[ROWS][COLS], 
                            struct ninja ninja) {
    struct statistics stat = {0};
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (board[row][col].colour == RED && 
                board[row][col].entity == UNPAINTED) {
                stat.red_unpainted++;
            } 
            if (board[row][col].colour == YELLOW &&
                        board[row][col].entity == UNPAINTED) {
                stat.yellow_unpainted++;
            } 
            if (board[row][col].colour == GREEN &&
                        board[row][col].entity == UNPAINTED) {
                stat.green_unpainted++;
            } 
            if (board[row][col].colour == BLUE &&
                        board[row][col].entity == UNPAINTED) {
                stat.blue_unpainted++;
            } 
            if (board[row][col].colour == PURPLE &&
                        board[row][col].entity == UNPAINTED) {
                stat.purple_unpainted++;
            } 
            if (board[row][col].entity == STAR) {
                stat.starpoints += 20;
            } 
            if (board[row][col].entity == PAINTED) {
                stat.num_painted++;
            } 
            if (board[row][col].entity == UNPAINTED) {
                stat.unpaintedpoints++;
            }
        }
    }
    stat.points_remaining = stat.starpoints + stat.unpaintedpoints;
    if (ninja.initial_unpainted == 0 ) {
        stat.completion = 100.0;
    } else {
        stat.completion = 100.0 * stat.num_painted / ninja.initial_unpainted;
    }
    return stat;
}

// This function prints the trampoline tile based on the given directions
//
// parameters:
// - direction: A tile struct containing the trampoline's two directions.
//
// returns: nothing.
void trampoline_helper(struct tile direction) {
    int dir1 = direction.direction1;
    int dir2 = direction.direction2;
    if ((dir1 == NORTH && dir2 == EAST) || 
        (dir1 == EAST && dir2 == NORTH)) {
        printf("|_\\");
    } else if ((dir1 == NORTH && dir2 == WEST) ||
               (dir1 == WEST && dir2 == NORTH)) {
        printf("/_|");
    } else if ((dir1 == SOUTH && dir2 == EAST) || 
               (dir1 == EAST && dir2 == SOUTH)) {
        printf("|*/");
    } else if ((dir1 == SOUTH && dir2 == WEST) || 
               (dir1 == WEST && dir2 == SOUTH)) {
        printf("\\*|");
    }
}

// =============================================================================
// EDITABLE Provided Functions
// =============================================================================

// Given a 2D board array, initialise all tile entities to EMPTY.
void initialise_board(struct tile board[ROWS][COLS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            board[row][col].entity = EMPTY;
            board[row][col].colour = NO_COLOUR;
        }
    }
}

// Prints the game board, showing the player's position and points.
void print_board(
    struct tile board[ROWS][COLS], 
    int player_row, 
    int player_col
) {
    print_board_line();
    print_board_header();
    print_board_line();
    for (int row = 0; row < ROWS; row++) {
        print_tile_spacer();
        for (int col = 0; col < COLS; col++) {
            printf(" ");
            struct tile tile = board[row][col];
            if (row == player_row && col == player_col) {
                printf("^_^");
            } else if (tile.entity == LADDER) {
                printf("|=|");
            } else if (tile.entity == WALL) {
                printf("|||");
            } else if (tile.entity == EXIT_LOCKED) {
                printf("[X]");
            } else if (tile.entity == EXIT_UNLOCKED) {
                printf("[ ]");
            } else if (tile.entity == PAINT_BUCKET) {
                printf("\\%c/", colour_to_char(tile.colour));
            } else if (tile.entity == PAINTED) {
                printf(" %c ", colour_to_char(tile.colour));
            } else if (tile.entity == UNPAINTED) {
                printf(" %c ", tolower(colour_to_char(tile.colour)));
            } else if (tile.entity == STAR) {
                printf(" * ");
            } else if (tile.entity == GLASS) {
                printf(" | ");
            } else if (tile.entity == TRAMPOLINE) {
                trampoline_helper(tile);
            } else {
                printf("   ");
            }
        }
        printf("\n");
    }
    print_tile_spacer();
    return;
}

// =============================================================================
// DO NOT EDIT Provided Functions 
// =============================================================================

// Prints board statistics, including the number of unpainted tiles remaining 
// by colour, the completion percentage, and the maximum points remaining.
void print_board_statistics (
    int num_red_unpainted_tiles,
    int num_yellow_unpainted_tiles,
    int num_green_unpainted_tiles, 
    int num_blue_unpainted_tiles,
    int num_purple_unpainted_tiles,
    double completion_percentage,
    int maximum_points_remaining
) {
    printf("========= Board Statistics =========\n");
    printf("Unpainted Tiles Remaining by Colour:\n");
    printf("  - RED:         %d\n", num_red_unpainted_tiles);
    printf("  - YELLOW:      %d\n", num_yellow_unpainted_tiles);
    printf("  - GREEN:       %d\n", num_green_unpainted_tiles);
    printf("  - BLUE:        %d\n", num_blue_unpainted_tiles);
    printf("  - PURPLE:      %d\n", num_purple_unpainted_tiles);
    printf("Completion Status:\n");
    printf("  - Paint Status: %.1f%%\n", completion_percentage);
    printf("  - Maximum Points Remaining: %d\n", maximum_points_remaining);
    printf("==================================\n");
}

// Helper function for print_board().
void print_board_header(void) {
    printf("|               N I N J A               |\n");
}

// Helper function for print_board(). 
void print_board_line(void) {
    printf("+");
    for (int col = 0; col < COLS; col++) {
        printf("---+");
    }
    printf("\n");
}

// Helper function for print_board(). 
void print_tile_spacer(void) {
    printf("+");
    for (int col = 0; col < COLS; col++) {
        printf("   +");
    }
    printf("\n");
}

// Helper function for print_board().
char colour_to_char(enum colour colour) {
    if (colour == RED) {
        return 'R';
    } else if (colour == YELLOW) {
        return 'Y';
    } else if (colour == GREEN) {
        return 'G';
    } else if (colour == BLUE) {
        return 'B';
    } else if (colour == PURPLE) {
        return 'P';
    }
    return ' ';
}