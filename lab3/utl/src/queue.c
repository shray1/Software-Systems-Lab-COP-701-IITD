#include "queue.h"

Queue * qInit() {
    Queue * q;
    if ((q = (Queue * ) malloc(sizeof(Queue))) == NULL) {
        //printf("ERROR : Cannot allocate memory from malloc.\n");
        return NULL;
        //Should rather exit? 
    }
    q -> head = q -> tail = NULL;
    q -> iSize = 0;
    return q;
}

void qErase(Queue * q) {
    if (q == NULL)
        return;
    free(q);
    //How do we free the nodes of q.

}

void qPush(Queue * q, void * elem) {
    //printf("DEBUG:: qPush()\n");
    if (q) {
        qNode * nNode; //new node. 
        nNode = (qNode * ) malloc(sizeof(qNode));
        if (nNode == NULL) {
            //Memory error.
            //printf("ERROR : Cannot allocate memory from malloc.\n");
            return;
        }
        nNode -> element = elem;
        nNode -> next = NULL;
        if (q -> tail)
            q -> tail -> next = nNode;
        q -> tail = nNode;
        if (q -> head == NULL)
            q -> head = nNode;
        q -> iSize += 1;
    }
}

void * qPop(Queue * q) {
    //printf("DEBUG:: qPop()\n");
    qNode * firstNode;
    if (q == NULL || q -> iSize == 0) {
        //printf("qPop() called on empty queue.\n");
        return NULL;
        //Error?
    }
    if (q -> head) {
        firstNode = q -> head;
        q -> head = q -> head -> next;
        firstNode->next = NULL;
        q -> iSize--;
    } else {
        //printf("qPop() called on queue with NULL head.\n");
        return NULL;
    }
    if(q->iSize==0){
        q->tail=NULL;
    }
    void * returnVal = firstNode -> element;
    free(firstNode);
    return returnVal;
}

int qIsEmpty(Queue * q) {
    //printf("DEBUG:: qIsEmpty()\n");
    if (q -> iSize > 0)
        return 0;
    return 1;
}

int qSize(Queue* q){
    return q->iSize;
}