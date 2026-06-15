#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "records.h"
#include "common.h"

#define HIGH_SCORE_FILE "data/high_score.dat"
#define RECORD_MAGIC    0xCAFEBEEFu

typedef struct {
    unsigned int magic;
    ScoreRecord  record;
} StoredRecord;

static void ensure_data_dir(void)
{
    int r = system("mkdir -p data");
    (void)r; /* best-effort; ignore return */
}

void load_high_score(void)
{
    ensure_data_dir();
    FILE *fp = fopen(HIGH_SCORE_FILE, "rb");
    if (fp) {
        StoredRecord sr;
        if (fread(&sr, sizeof(sr), 1, fp) == 1 && sr.magic == RECORD_MAGIC) {
            high_score = sr.record.score;
            strncpy(high_score_name, sr.record.name, sizeof(high_score_name) - 1);
            high_score_name[sizeof(high_score_name) - 1] = '\0';
        } else {
            high_score = 0;
            strcpy(high_score_name, "NCIT Student");
        }
        fclose(fp);
    } else {
        high_score = 0;
        strcpy(high_score_name, "NCIT Student");
    }
}

void save_high_score(const char *player_name, unsigned int score)
{
    ensure_data_dir();
    FILE *fp = fopen(HIGH_SCORE_FILE, "wb");
    if (fp) {
        StoredRecord sr;
        sr.magic = RECORD_MAGIC;
        strncpy(sr.record.name, player_name, sizeof(sr.record.name) - 1);
        sr.record.name[sizeof(sr.record.name) - 1] = '\0';
        sr.record.score = score;
        fwrite(&sr, sizeof(sr), 1, fp);
        fclose(fp);
        strncpy(high_score_name, player_name, sizeof(high_score_name) - 1);
        high_score_name[sizeof(high_score_name) - 1] = '\0';
    }
}
