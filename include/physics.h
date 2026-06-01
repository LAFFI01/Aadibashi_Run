#ifndef PHYSICS_H
#define PHYSICS_H

#include "common.h"

void init_game(void);
void update_physics(void);
int check_collision(Position pos);
void spawn_food(void);

#endif // PHYSICS_H
