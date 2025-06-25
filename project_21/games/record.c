#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "record.h"

// 게임 결과 저장
void save_result(const char *game_name, const char *player1, const char *player2, char winner, int turn_count, char board[3][3]) {
    FILE *file = fopen("game_result.txt", "a");
    if (!file) {
        perror("Failed to open result file");
        return;
    }

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(file, "Game: %s\n", game_name);
    fprintf(file, "Time: %s\n", time_str);
    if (winner == '-')
        fprintf(file, "Winner: Draw.\n");
    else
        fprintf(file, "Winner: %c\n", winner);

    fprintf(file, "Player 1: %s (%c)\n", player1, 'O');
    fprintf(file, "Player 2: %s (%c)\n", player2, 'X');
    fprintf(file, "Total turns: %d\n", turn_count);

    fprintf(file, "Board:\n");
    for (int i = 0; i < 3; i++) {
        fprintf(file, " %c | %c | %c \n", board[i][0], board[i][1], board[i][2]);
        if (i < 2) fprintf(file, "---|---|---\n");
    }
    fprintf(file, "--------------------------\n");

    fclose(file);
}




// Display the game records
void show_records() {
    FILE *file = fopen("game_result.txt", "r");
    if (!file) {
        printf("No record file found.\n");
        return;
    }

    printf("\n===== Game Records =====\n");

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    printf("=========================\n");

    fclose(file);
}

#ifdef BUILD_RECORD_MAIN
int main() {
    show_records();
    return 0;
}
#endif

