#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void show_menu() {
    printf("\n==== Mini Game Hub ====\n");
    printf("1) Tic-Tac-Toe\n");
    printf("2) Show Game Records\n");
    printf("3) Exit\n");
    printf("=======================\n");
    printf("Select an option: ");
}

int main() {
    int choice;

    while (1) {
        show_menu();
        scanf("%d", &choice);

        if (choice == 1) {
            // nickname
            char name[32];
            printf("Enter your name: ");
            scanf("%s", name);

            // O/X 선택
            char symbol;
            printf("Choose your symbol (O/X): ");
            scanf(" %c", &symbol);

            pid_t pid = fork();
            if (pid == 0) {
                if (symbol == 'O' || symbol == 'o') {
                    // O면 player1
                    execl("./player1", "player1", name, NULL);
                } else if (symbol == 'X' || symbol == 'x') {
                    // X면 player2
                    execl("./player2", "player2", name, NULL);
                } else {
                    printf("Invalid symbol.\n");
                    exit(1);
                }
                perror("execl error");
                exit(1);
            } else {
                wait(NULL); // 부모는 자식 기다림
            }

        } else if (choice == 2) {
            // 기록 보기
            pid_t pid = fork();
            if (pid == 0) {
                execl("./record", "record", NULL);
                perror("execl error");
                exit(1);
            } else {
                wait(NULL);
            }
        } else if (choice == 3) {
            printf("Exiting.\n");
            break;
        } else {
            printf("Invalid option.\n");
        }
    }

    return 0;
}





