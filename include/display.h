#ifndef DISPLAY_H
#define DISPLAY_H

#include "common.h"

void init_terminal(void);
void restore_terminal(void);
void render_static_board(void);
void render_delta(Position prev_player, Position prev_enemies[MAX_ENEMIES]);
void render_ui(void);
void render_game_over(int won);
void render_game_over_overlay(int won, int is_new_record, const char *record_holder);
int check_input_hit(void);
Direction get_player_input(void);

#endif // DISPLAY_H
