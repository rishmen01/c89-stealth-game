#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

Stack *create_stack(void)
{
    Stack *stack;

    stack = (Stack *)malloc(sizeof(Stack));
    if (stack == NULL) {
        return NULL;
    }
    stack->top = NULL;
    return stack;
}

int is_empty(const Stack *stack)
{
    return stack == NULL || stack->top == NULL;
}

int push(Stack *stack, GameState state)
{
    Node *new_node;

    if (stack == NULL) {
        return 0;
    }

    new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        return 0;
    }

    new_node->state = state;
    new_node->next = stack->top;
    stack->top = new_node;
    return 1;
}

int pop(Stack *stack, GameState *state)
{
    Node *old_top;

    if (is_empty(stack) || state == NULL) {
        return 0;
    }

    old_top = stack->top;
    *state = old_top->state;
    stack->top = old_top->next;
    free(old_top);
    return 1;
}

void free_stack(Stack *stack)
{
    Node *current;
    Node *next_node;

    if (stack == NULL) {
        return;
    }

    current = stack->top;
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    free(stack);
}

int push_game_state(Game *game)
{
    GameState state;

    if (game == NULL) {
        return 0;
    }

    if (game->undo_stack == NULL) {
        game->undo_stack = create_stack();
        if (game->undo_stack == NULL) {
            return 0;
        }
    }

    state.player_pos = game->player_pos;
    state.snake_pos = game->snake_pos;
    state.lantern_picked = game->lantern_picked;
    state.visibility_range = game->visibility_range;
    state.is_game_over = game->is_game_over;
    state.game_won = game->game_won;

    return push(game->undo_stack, state);
}

int undo_move(Game *game)
{
    GameState previous_state;

    if (game == NULL || !pop(game->undo_stack, &previous_state)) {
        printf("There are no moves to undo.\n");
        return 0;
    }

    game->player_pos = previous_state.player_pos;
    game->snake_pos = previous_state.snake_pos;
    game->lantern_picked = previous_state.lantern_picked;
    game->visibility_range = previous_state.visibility_range;
    game->is_game_over = previous_state.is_game_over;
    game->game_won = previous_state.game_won;

    return 1;
}
