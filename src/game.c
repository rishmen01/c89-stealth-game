#include <ctype.h>
#include <stdio.h>
#include "game.h"
#include "map.h"
#include "linkedlist.h"
#include "random.h"
#include "terminal.h"
#ifdef DARK
#include "darkmode.h"
#endif

static int position_is_inside(const Game *game, Position position)
{
    return position.row >= 0 && position.row < game->map_rows &&
           position.col >= 0 && position.col < game->map_cols;
}

void initialise_game(Game *game)
{
    if (game == NULL) {
        return;
    }

    game->map = NULL;
    game->map_rows = 0;
    game->map_cols = 0;
    game->player_pos.row = -1;
    game->player_pos.col = -1;
    game->snake_pos.row = -1;
    game->snake_pos.col = -1;
    game->treasure_pos.row = -1;
    game->treasure_pos.col = -1;
    game->lantern_pos.row = -1;
    game->lantern_pos.col = -1;
    game->is_game_over = 0;
    game->game_won = 0;
    game->lantern_picked = 0;
    game->visibility_range = 3;
    game->wall_count = 0;
    game->undo_stack = NULL;
}

int move_player(Game *game, int row_delta, int col_delta)
{
    Position destination;
    char destination_cell;

    if (game == NULL || game->map == NULL || game->is_game_over) {
        return 0;
    }

    destination.row = game->player_pos.row + row_delta;
    destination.col = game->player_pos.col + col_delta;

    if (!position_is_inside(game, destination)) {
        printf("You cannot move outside the map.\n");
        return 0;
    }

    destination_cell = game->map[destination.row][destination.col];
    if (destination_cell == 'O') {
        printf("You cannot move through an obstacle.\n");
        return 0;
    }

    if (!push_game_state(game)) {
        printf("Unable to save the move for undo. The move was cancelled.\n");
        return 0;
    }

    game->player_pos = destination;

    if (destination_cell == '@' && !game->lantern_picked) {
        game->lantern_picked = 1;
#ifdef DARK
        game->visibility_range = 6;
#endif
        printf("You collected the lantern!\n");
    }

    if (destination_cell == '$') {
        game->is_game_over = 1;
        game->game_won = 1;
        printf("You found the treasure. You win!\n");
        return 1;
    }

    if (destination.row == game->snake_pos.row &&
        destination.col == game->snake_pos.col) {
        game->is_game_over = 1;
        game->game_won = 0;
        printf("You ran into the snake. You lose!\n");
    }

    return 1;
}

void move_snake(Game *game)
{
    Position candidates[8];
    Position destination;
    int row_delta;
    int col_delta;
    int candidate_count;
    int index;

    if (game == NULL || game->map == NULL || game->is_game_over) {
        return;
    }

    candidate_count = 0;
    for (row_delta = -1; row_delta <= 1; row_delta++) {
        for (col_delta = -1; col_delta <= 1; col_delta++) {
            if (row_delta == 0 && col_delta == 0) {
                continue;
            }

            destination.row = game->snake_pos.row + row_delta;
            destination.col = game->snake_pos.col + col_delta;
            if (position_is_inside(game, destination) &&
                game->map[destination.row][destination.col] == ' ') {
                candidates[candidate_count] = destination;
                candidate_count++;
            }
        }
    }

    if (candidate_count == 0) {
        return;
    }

    index = random_in_range(0, candidate_count - 1);
    game->snake_pos = candidates[index];

    if (game->snake_pos.row == game->player_pos.row &&
        game->snake_pos.col == game->player_pos.col) {
        game->is_game_over = 1;
        game->game_won = 0;
        printf("The snake caught you. You lose!\n");
    }
}

void check_game_state(Game *game)
{
    if (game == NULL || game->is_game_over) {
        return;
    }

    if (game->player_pos.row == game->treasure_pos.row &&
        game->player_pos.col == game->treasure_pos.col) {
        game->is_game_over = 1;
        game->game_won = 1;
    } else if (game->player_pos.row == game->snake_pos.row &&
               game->player_pos.col == game->snake_pos.col) {
        game->is_game_over = 1;
        game->game_won = 0;
    }
}

void update_game(Game *game)
{
    if (game == NULL || game->is_game_over) {
        return;
    }
    move_snake(game);
    check_game_state(game);
}

int handle_input(Game *game)
{
    int input;

    if (game == NULL) {
        return 0;
    }

    do {
        input = getchar();
    } while (input != EOF && isspace((unsigned char)input));

    if (input == EOF) {
        printf("\nInput ended. Exiting the game.\n");
        game->is_game_over = 1;
        game->game_won = 0;
        return 0;
    }

    switch (tolower((unsigned char)input)) {
        case 'w':
            return move_player(game, -1, 0);
        case 's':
            return move_player(game, 1, 0);
        case 'a':
            return move_player(game, 0, -1);
        case 'd':
            return move_player(game, 0, 1);
        case 'u':
            undo_move(game);
            return 0;
        case 'q':
        case 3:
            game->is_game_over = 1;
            game->game_won = 0;
            printf("You left the game.\n");
            return 0;
        default:
            printf("Invalid key. Use W, A, S, D, U or Q.\n");
            return 0;
    }
}

void clear_screen(void)
{
    printf("\033[2J\033[H");
    fflush(stdout);
}

void print_map(const Game *game)
{
    int row;
    int col;
    char cell;

    if (game == NULL || game->map == NULL) {
        return;
    }

    for (col = 0; col < game->map_cols + 2; col++) {
        putchar('*');
    }
    putchar('\n');

    for (row = 0; row < game->map_rows; row++) {
        putchar('*');
        for (col = 0; col < game->map_cols; col++) {
            if (row == game->player_pos.row && col == game->player_pos.col) {
                cell = 'P';
            } else if (row == game->snake_pos.row && col == game->snake_pos.col) {
                cell = '~';
            } else if (game->map[row][col] == '@' && game->lantern_picked) {
                cell = ' ';
            } else {
                cell = game->map[row][col];
            }
            putchar(cell);
        }
        puts("*");
    }

    for (col = 0; col < game->map_cols + 2; col++) {
        putchar('*');
    }
    printf("\nW/A/S/D: move | U: undo | Q or Ctrl+C: quit\n");
    printf("Enter your option: ");
    fflush(stdout);
}

void free_game(Game *game)
{
    if (game == NULL) {
        return;
    }

    free_map(game);
    free_stack(game->undo_stack);
    game->undo_stack = NULL;
}

int game_over(const Game *game)
{
    return game == NULL || game->is_game_over;
}
