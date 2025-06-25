#include <ncurses.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "common.h"
#include "record.h"

WINDOW *chat_win, *game_win, *input_win, *timer_win;
int read_fd, write_fd;
int turn_count = 0;
char name[32];
char opponent_name[32];
char board[3][3];
char my_symbol = 'X';
char opp_symbol = 'O';
char current_player = 'O';
int timeout_flag = 0;

// Timer thread
void *timer_thread(void *arg) {
    timeout_flag = 0;
    for (int i = 10; i >= 0; i--) {
        werase(timer_win);
        box(timer_win, 0, 0);
        mvwprintw(timer_win, 1, 1, "Timer: %ds", i);
        wrefresh(timer_win);
        sleep(1);
    }
    timeout_flag = 1;
    werase(timer_win);
    box(timer_win, 0, 0);
    mvwprintw(timer_win, 1, 1, "Time out!");
    wrefresh(timer_win);
    return NULL;
}

void draw_board() {
    werase(game_win);
    box(game_win, 0, 0);
    mvwprintw(game_win, 0, 2, " Game Board ");
    for (int i = 0; i < 3; i++) {
        mvwprintw(game_win, 2 + i * 2, 4, " %c | %c | %c ", board[i][0], board[i][1], board[i][2]);
        if (i < 2)
            mvwprintw(game_win, 3 + i * 2, 4, "---|---|---");
    }
    wrefresh(game_win);
}

int check_winner(char s) {
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == s && board[i][1] == s && board[i][2] == s) return 1;
        if (board[0][i] == s && board[1][i] == s && board[2][i] == s) return 1;
    }
    if (board[0][0] == s && board[1][1] == s && board[2][2] == s) return 1;
    if (board[0][2] == s && board[1][1] == s && board[2][0] == s) return 1;
    return 0;
}

int is_draw() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] == ' ')
                return 0;
    return 1;
}

void *output_thread(void *arg) {
    Message msg;
    while (1) {
        int n = read(read_fd, &msg, sizeof(msg));
        if (n > 0) {
            if (msg.type == MSG_CHAT) {
                wattron(chat_win, COLOR_PAIR(2));
                wprintw(chat_win, "[%s]: %s\n", msg.sender, msg.text);
                wattroff(chat_win, COLOR_PAIR(2));
                wrefresh(chat_win);
            } else if (msg.type == MSG_GAME) {
		turn_count++;
                int r, c;
                sscanf(msg.text, "%d %d", &r, &c);
                board[r - 1][c - 1] = opp_symbol;
                draw_board();
                current_player = my_symbol;

		strncpy(opponent_name, msg.sender, sizeof(opponent_name));
            }
        }
    }
    return NULL;
}


int main() {
    printf("Enter your nickname: ");
    scanf("%s", name);

    // FIFO setup
    mkfifo(FIFO1, 0666);
    mkfifo(FIFO2, 0666);
    write_fd = open(FIFO2, O_RDWR);
    read_fd = open(FIFO1, O_RDWR);

    // ncurses setup
    initscr();
    cbreak(); noecho();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    chat_win = newwin(10, max_x, 0, 0);
    box(chat_win, 0, 0);
    mvwprintw(chat_win, 0, 2, " Chat ");
    wrefresh(chat_win);

    game_win = newwin(max_y - 17, max_x, 10, 0);
    input_win = newwin(3, max_x - 20, max_y - 4, 20);
    timer_win = newwin(4, 20, max_y - 7, 0);
    box(game_win, 0, 0); box(input_win, 0, 0); box(timer_win, 0, 0);
    wrefresh(game_win); wrefresh(input_win); wrefresh(timer_win);

    pthread_t out_tid;
    pthread_create(&out_tid, NULL, output_thread, NULL);

    // Initialize board
    memset(board, ' ', sizeof(board));
    draw_board();

    char input[100];
    while (1) {
        if (current_player == my_symbol) {
            pthread_t timer_tid;
            pthread_create(&timer_tid, NULL, timer_thread, NULL);

            werase(input_win);
            box(input_win, 0, 0);
            mvwprintw(input_win, 1, 1, "Your turn (row col) or /chat: ");
            wrefresh(input_win);
            wgetnstr(input_win, input, sizeof(input) - 1);

            pthread_cancel(timer_tid);

            if (timeout_flag) {
                wprintw(chat_win, "Time out! Opponent wins!\n");
                wrefresh(chat_win);
		save_result("TicTacToe", name, opponent_name, opp_symbol, turn_count, board);
                break;
            }

            if (input[0] == '/') {
                Message chat;
                chat.type = MSG_CHAT;
                strncpy(chat.sender, name, sizeof(chat.sender));
                strncpy(chat.text, input + 1, sizeof(chat.text));
                write(write_fd, &chat, sizeof(chat));
                continue;
            }

            int r, c;
            if (sscanf(input, "%d %d", &r, &c) != 2 || r < 1 || r > 3 || c < 1 || c > 3 || board[r - 1][c - 1] != ' ') {
                wprintw(chat_win, "Invalid input!\n");
                wrefresh(chat_win);
                continue;
            }

            board[r - 1][c - 1] = my_symbol;
            draw_board();
	    turn_count++;

            Message move;
            move.type = MSG_GAME;
            snprintf(move.text, sizeof(move.text), "%d %d", r, c);
            strncpy(move.sender, name, sizeof(move.sender));
            write(write_fd, &move, sizeof(move));

            if (check_winner(my_symbol)) {
                wprintw(chat_win, "You win!\n");
                wrefresh(chat_win);
		save_result("TicTacToe", name, opponent_name, my_symbol, turn_count, board);
                break;
            } else if (is_draw()) {
                wprintw(chat_win, "It's a draw.\n");
                wrefresh(chat_win);
                break;
            }

            current_player = opp_symbol;
        } else {
            usleep(100000);
        }
    }

    pthread_join(out_tid, NULL);
    close(read_fd); close(write_fd);
    delwin(chat_win); delwin(game_win); delwin(input_win); delwin(timer_win);
    endwin();
    return 0;
}




