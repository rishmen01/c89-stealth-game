#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "game.h"

typedef struct {
    Position player_pos;
    Position snake_pos;
    int lantern_picked;
    int visibility_range;
    int is_game_over;
    int game_won;
} GameState;

typedef struct Node {
    GameState state;
    struct Node *next;
} Node;

typedef struct Stack {
    Node *top;
} Stack;

Stack *create_stack(void);
int is_empty(const Stack *stack);
int push(Stack *stack, GameState state);
int pop(Stack *stack, GameState *state);
void free_stack(Stack *stack);
int push_game_state(Game *game);
int undo_move(Game *game);

#endif
