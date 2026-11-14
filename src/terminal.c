#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "terminal.h"

static struct termios original_terminal_mode;
static int terminal_mode_saved = 0;
static int terminal_buffer_disabled = 0;

int disable_buffer(void)
{
    struct termios game_mode;

    if (!isatty(STDIN_FILENO)) {
        return 0;
    }

    if (tcgetattr(STDIN_FILENO, &original_terminal_mode) != 0) {
        return 0;
    }
    terminal_mode_saved = 1;

    game_mode = original_terminal_mode;
    game_mode.c_lflag &= (tcflag_t)~(ICANON | ECHO | ISIG);
    game_mode.c_cc[VMIN] = 1;
    game_mode.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &game_mode) != 0) {
        terminal_mode_saved = 0;
        return 0;
    }

    terminal_buffer_disabled = 1;
    return 1;
}

void restore_terminal(void)
{
    if (terminal_mode_saved && terminal_buffer_disabled) {
        tcsetattr(STDIN_FILENO, TCSANOW, &original_terminal_mode);
    }
    terminal_buffer_disabled = 0;
    terminal_mode_saved = 0;
}
