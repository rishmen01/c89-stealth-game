#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "map.h"
#include "newSleep.h"
#include "random.h"
#include "terminal.h"
#ifdef DARK
#include "darkmode.h"
#endif

int main(int argc, char *argv[])
{
    Game game;
    int moved;

    initialise_game(&game);

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <map_file>\n", argv[0]);
        return 1;
    }

    if (!load_map(argv[1], &game)) {
        fprintf(stderr, "Could not load map '%s'. Check its format and contents.\n", argv[1]);
        free_game(&game);
        return 1;
    }

    init_random();
    atexit(restore_terminal);
    disable_buffer();

    while (!game_over(&game)) {
        clear_screen();
#ifdef DARK
        print_darkmode_map(&game);
#else
        print_map(&game);
#endif
        moved = handle_input(&game);
        if (moved && !game_over(&game)) {
            update_game(&game);
            game_sleep(0.15f);
        }
    }

    restore_terminal();
    clear_screen();
    print_map(&game);
    if (game.game_won) {
        printf("Congratulations! You collected the treasure.\n");
    } else {
        printf("Game over. Thanks for playing!\n");
    }

    free_game(&game);
    return 0;
}
