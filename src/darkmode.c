#include <stdio.h>
#include <stdlib.h>
#include "darkmode.h"

int manhattan_distance(Position first, Position second)
{
    return abs(first.row - second.row) + abs(first.col - second.col);
}

static char visible_cell(const Game *game, int row, int col)
{
    if (row == game->player_pos.row && col == game->player_pos.col) {
        return 'P';
    }
    if (row == game->snake_pos.row && col == game->snake_pos.col) {
        return '~';
    }
    if (game->map[row][col] == '@' && game->lantern_picked) {
        return ' ';
    }
    return game->map[row][col];
}

void print_darkmode_map(const Game *game)
{
    int row;
    int col;
    Position current;
    char cell;

    for (row = 0; row < game->map_rows; row++) {
        putchar('*');
        for (col = 0; col < game->map_cols; col++) {
            current.row = row;
            current.col = col;
            if (manhattan_distance(game->player_pos, current) <= game->visibility_range) {
                cell = visible_cell(game, row, col);
                putchar(cell == ' ' ? '.' : cell);
            } else {
                putchar(' ');
            }
        }
        puts("*");
    }

    printf("\nDark mode: visibility range %d\n", game->visibility_range);
    printf("W/A/S/D: move | U: undo | Q or Ctrl+C: quit\n");
    printf("Enter your option: ");
    fflush(stdout);
}
