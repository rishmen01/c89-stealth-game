#ifndef GAME_H
#define GAME_H

struct Stack;

typedef struct {
    int row;
    int col;
} Position;

typedef struct Game {
    char **map;
    int map_rows;
    int map_cols;
    Position player_pos;
    Position snake_pos;
    Position treasure_pos;
    Position lantern_pos;
    int is_game_over;
    int game_won;
    int lantern_picked;
    int visibility_range;
    int wall_count;
    struct Stack *undo_stack;
} Game;

void initialise_game(Game *game);
int handle_input(Game *game);
void update_game(Game *game);
int move_player(Game *game, int row_delta, int col_delta);
void move_snake(Game *game);
void check_game_state(Game *game);
void clear_screen(void);
void print_map(const Game *game);
void free_game(Game *game);
int game_over(const Game *game);

#endif
