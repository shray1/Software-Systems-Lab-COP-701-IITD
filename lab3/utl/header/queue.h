#ifndef QUEUE_H
#define QUEUE_H

#include<stdio.h>
#include<stdlib.h>

struct qNode {
    void * element;
    struct qNode * next;
};

struct Queue {
    int iSize;
    struct qNode * head;
    struct qNode * tail;
};

typedef struct Queue Queue;
typedef struct qNode qNode;

Queue * qInit();
void qErase(Queue * q);
void qPush(Queue * q, void * element);
void * qPop(Queue * q);
int qIsEmpty(Queue * q);
int qSize(Queue * q);

#endif