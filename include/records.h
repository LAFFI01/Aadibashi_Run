#ifndef RECORDS_H
#define RECORDS_H

#include "common.h"

void load_high_score(void);
void save_high_score(const char *player_name, unsigned int score);

#endif // RECORDS_H
