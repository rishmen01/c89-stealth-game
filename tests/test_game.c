#include <stdio.h>
#include "game.h"
#include "map.h"
#include "linkedlist.h"

static int tests_run = 0;
static int tests_failed = 0;

static void check_result(int condition, const char *description)
{
    tests_run++;
    if (condition) {
        printf("PASS: %s\n", description);
    } else {
        printf("FAIL: %s\n", description);
        tests_failed++;
    }
}

int main(void)
{
    Game game;
    int loaded;
    Position start_player;
    Position start_snake;

    initialise_game(&game);
    loaded = load_map("maps/default-map.txt", &game);
    check_result(loaded, "valid sample map loads");

    if (loaded) {
        check_result(game.map_rows == 15 && game.map_cols == 20,
                     "map dimensions are read correctly");
        check_result(game.wall_count == 36, "declared wall count is validated");

        start_player = game.player_pos;
        start_snake = game.snake_pos;
        check_result(move_player(&game, 0, 1), "player can move onto an empty cell");
        check_result(undo_move(&game), "undo restores a previous move");
        check_result(game.player_pos.row == start_player.row &&
                     game.player_pos.col == start_player.col,
                     "undo restores the player position");
        check_result(game.snake_pos.row == start_snake.row &&
                     game.snake_pos.col == start_snake.col,
                     "undo restores the snake position");

        check_result(!move_player(&game, -2, 0),
                     "movement outside the map is rejected");

        game.player_pos.row = 2;
        game.player_pos.col = 4;
        check_result(!move_player(&game, 0, 1),
                     "movement into an obstacle is rejected");

        game.player_pos.row = start_player.row;
        game.player_pos.col = start_player.col;
        check_result(move_player(&game, 0, 1), "first step towards lantern succeeds");
        check_result(move_player(&game, 1, 0), "second step towards lantern succeeds");
        check_result(move_player(&game, 0, 1), "lantern can be collected");
        check_result(game.lantern_picked, "lantern state is updated");
#ifdef DARK
        check_result(game.visibility_range == 6,
                     "collecting the lantern increases dark-mode visibility");
#endif
        check_result(undo_move(&game), "lantern collection can be undone");
        check_result(!game.lantern_picked,
                     "undo restores the uncollected lantern state");
#ifdef DARK
        check_result(game.visibility_range == 3,
                     "undo restores the original dark-mode visibility range");
#endif

        game.player_pos.row = 4;
        game.player_pos.col = 12;
        check_result(move_player(&game, 0, 1), "player can reach the treasure");
        check_result(game.is_game_over && game.game_won,
                     "reaching the treasure ends the game with a win");
    }
    free_game(&game);

    initialise_game(&game);
    if (load_map("maps/default-map.txt", &game)) {
        game.player_pos.row = game.snake_pos.row;
        game.player_pos.col = game.snake_pos.col - 1;
        check_result(move_player(&game, 0, 1),
                     "player movement into the snake is processed");
        check_result(game.is_game_over && !game.game_won,
                     "collision with the snake ends the game with a loss");
    } else {
        check_result(0, "sample map reloads for collision test");
    }
    free_game(&game);

    initialise_game(&game);
    loaded = load_map("tests/invalid-map.txt", &game);
    check_result(!loaded, "map containing an invalid cell value is rejected");
    free_game(&game);

    initialise_game(&game);
    loaded = load_map("does-not-exist.map", &game);
    check_result(!loaded, "missing map file is reported");
    free_game(&game);

    printf("\nTests run: %d | Passed: %d | Failed: %d\n",
           tests_run, tests_run - tests_failed, tests_failed);
    return tests_failed == 0 ? 0 : 1;
}
