#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "map.h"

#define MAX_MAP_DIMENSION 200

static void reset_map_positions(Game *game)
{
    game->player_pos.row = -1;
    game->player_pos.col = -1;
    game->snake_pos.row = -1;
    game->snake_pos.col = -1;
    game->treasure_pos.row = -1;
    game->treasure_pos.col = -1;
    game->lantern_pos.row = -1;
    game->lantern_pos.col = -1;
}

static int consume_trailing_whitespace(FILE *file)
{
    int ch;

    while ((ch = fgetc(file)) != EOF) {
        if (!isspace((unsigned char)ch)) {
            return 0;
        }
    }
    return 1;
}

void free_map(Game *game)
{
    int row;

    if (game == NULL || game->map == NULL) {
        return;
    }

    for (row = 0; row < game->map_rows; row++) {
        free(game->map[row]);
    }
    free(game->map);
    game->map = NULL;
    game->map_rows = 0;
    game->map_cols = 0;
    game->wall_count = 0;
}

int load_map(const char *filename, Game *game)
{
    FILE *file;
    int rows;
    int cols;
    int declared_walls;
    int actual_walls;
    int value;
    int row;
    int col;
    int counts[6];
    int valid;

    if (filename == NULL || game == NULL) {
        return 0;
    }

    free_map(game);
    reset_map_positions(game);
    game->lantern_picked = 0;
    game->is_game_over = 0;
    game->game_won = 0;
    game->visibility_range = 3;

    file = fopen(filename, "r");
    if (file == NULL) {
        return 0;
    }

    if (fscanf(file, "%d %d", &rows, &cols) != 2 ||
        fscanf(file, "%d", &declared_walls) != 1 ||
        rows <= 0 || cols <= 0 ||
        rows > MAX_MAP_DIMENSION || cols > MAX_MAP_DIMENSION ||
        declared_walls < 0) {
        fclose(file);
        return 0;
    }

    game->map_rows = rows;
    game->map_cols = cols;
    game->map = (char **)calloc((size_t)rows, sizeof(char *));
    if (game->map == NULL) {
        fclose(file);
        game->map_rows = 0;
        game->map_cols = 0;
        return 0;
    }

    for (value = 0; value < 6; value++) {
        counts[value] = 0;
    }
    actual_walls = 0;
    valid = 1;

    for (row = 0; row < rows && valid; row++) {
        game->map[row] = (char *)malloc((size_t)cols * sizeof(char));
        if (game->map[row] == NULL) {
            valid = 0;
            break;
        }

        for (col = 0; col < cols; col++) {
            if (fscanf(file, "%d", &value) != 1 || value < 0 || value > 5) {
                valid = 0;
                break;
            }

            counts[value]++;
            if (value == 0) {
                game->map[row][col] = ' ';
            } else if (value == 1) {
                game->map[row][col] = 'O';
                actual_walls++;
            } else if (value == 2) {
                game->map[row][col] = '@';
                game->lantern_pos.row = row;
                game->lantern_pos.col = col;
            } else if (value == 3) {
                game->map[row][col] = ' ';
                game->player_pos.row = row;
                game->player_pos.col = col;
            } else if (value == 4) {
                game->map[row][col] = ' ';
                game->snake_pos.row = row;
                game->snake_pos.col = col;
            } else {
                game->map[row][col] = '$';
                game->treasure_pos.row = row;
                game->treasure_pos.col = col;
            }
        }
    }

    if (valid && !consume_trailing_whitespace(file)) {
        valid = 0;
    }
    fclose(file);

    if (valid && (counts[2] != 1 || counts[3] != 1 ||
                  counts[4] != 1 || counts[5] != 1 ||
                  actual_walls != declared_walls)) {
        valid = 0;
    }

    if (!valid) {
        free_map(game);
        reset_map_positions(game);
        return 0;
    }

    game->wall_count = actual_walls;
    return 1;
}
