#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// 컬러 코드
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define RESET   "\033[0m"

// FIFO 경로
#define FIFO1 "/tmp/fifo_p1"
#define FIFO2 "/tmp/fifo_p2"

// 메시지 타입
#define MSG_GAME 1
#define MSG_CHAT 2

#define MSG_TURN_START 3
#define MSG_TURN_END   4
#define MSG_TIMEOUT    5

// 최대 버퍼 크기
#define BUFFER_SIZE 1024

// 플레이어 정보
typedef struct {
    char name[32];
    char symbol;   // 'X' or 'O'
} Player;

// 메시지 구조
typedef struct {
    int type;         
    char sender[32];
    char text[BUFFER_SIZE];
} Message;



#endif

