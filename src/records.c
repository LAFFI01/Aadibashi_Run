#include <stdio.h>
#include <string.h>
#include "records.h"
#include "common.h"

#define HIGH_SCORE_FILE "data/high_score.dat"

void load_high_score(void) {
    FILE *fp = fopen(HIGH_SCORE_FILE, "rb");
    if (fp != NULL) {
        ScoreRecord record;
        // Read the single binary structure block from disk
        if (fread(&record, sizeof(ScoreRecord), 1, fp) == 1) {
            high_score = record.score;
            strncpy(high_score_name, record.name, sizeof(high_score_name) - 1);
            high_score_name[sizeof(high_score_name) - 1] = '\0';
        } else {
            high_score = 0;
            strcpy(high_score_name, "NCIT Student");
        }
        fclose(fp);
    } else {
        high_score = 0; // First run default
        strcpy(high_score_name, "NCIT Student");
    }
}

void save_high_score(const char *player_name, unsigned int score) {
    FILE *fp = fopen(HIGH_SCORE_FILE, "wb");
    if (fp != NULL) {
        ScoreRecord record;
        // Construct the record structure block in memory
        strncpy(record.name, player_name, sizeof(record.name) - 1);
        record.name[sizeof(record.name) - 1] = '\0';
        record.score = score;
        
        // Write the structured block directly to disk
        fwrite(&record, sizeof(ScoreRecord), 1, fp);
        fclose(fp);
        
        // Update global cache
        strncpy(high_score_name, player_name, sizeof(high_score_name) - 1);
        high_score_name[sizeof(high_score_name) - 1] = '\0';
    }
}
