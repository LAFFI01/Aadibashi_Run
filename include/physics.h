#ifndef PHYSICS_H
#define PHYSICS_H

#include "common.h"

void init_game(void);
void update_physics(void);
int  check_collision(Position pos);
void spawn_food(void);
void spawn_axe(void);
void reset_positions(void);
void handle_player_caught(int enemy_idx);

#endif /* PHYSICS_H */
