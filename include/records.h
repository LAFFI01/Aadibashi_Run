#ifndef RECORDS_H
#define RECORDS_H

typedef struct {
    char         name[32];
    unsigned int score;
} ScoreRecord;

void load_high_score(void);
void save_high_score(const char *player_name, unsigned int score);

#endif /* RECORDS_H */
