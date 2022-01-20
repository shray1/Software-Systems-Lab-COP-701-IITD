#include "stdio.h"
#include "stdlib.h"
#include "string.h"

struct qNode
{
  void* element;
  struct qNode *next;
};

struct Queue
{
 int iSize;
 struct qNode* head;
 struct qNode* tail;
};

typedef struct Queue Queue;
typedef struct qNode qNode;


Queue* qInit();
void qErase(Queue* q);
void qPush(Queue* q, void* element);
void* qPop(Queue* q);
int qIsEmpty(Queue* q);


int main()
{
	char *a = "abcd";
	char *e = "efgh";
	char *i = "ijkl";
	char *m = "mnop";
	Queue* q= qInit();
	printf("Size of Queue q is %d\n", q->iSize);
	qPush(q,a);
	qPush(q,e);
	qPush(q,m);
	qPush(q,i);
	printf("Size of Queue q is %d\n", q->iSize);
	while(q->iSize > 0){
		char* x = (char*)qPop(q);
		printf("Value popped = %s\n", x);
		printf("Size of Queue q is %d\n", q->iSize);
	}
  char* z ="zebra";
  qPush(q,z);
  printf("Value popper = %s\n",(char*)qPop(q));
	return 0;
}



Queue* qInit()
{
  Queue* q;
  if((q=(Queue*)malloc(sizeof(Queue)))==NULL)
  {
    printf("ERROR : Cannot allocate memory from malloc.\n");
    return NULL;
    //Should rather exit? 
  }
  q->head = q->tail = NULL;
  q->iSize = 0;
  return q;
}

void qErase(Queue* q)
{
  if(q == NULL)
      return;
  free(q);
  //How do we free the nodes of q.

}


void qPush(Queue* q, void* elem)
{
  if(q){
    qNode* nNode; //new node. 
    nNode = (qNode*)malloc(sizeof(qNode));
    if(nNode == NULL){
      //Memory error.
      printf("ERROR : Cannot allocate memory from malloc.\n");
      return;
    }
    nNode->element = elem;
    nNode->next = NULL;
    if(q->tail)
       q->tail->next = nNode;
    q->tail = nNode;
    if(q->head==NULL)
       q->head = nNode;
    q->iSize+=1;
  }
}

void* qPop(Queue* q)
{
  qNode* firstNode;
  if(q==NULL || q->iSize==0){
    return NULL;
    //Error?
  }
  if(q->head){
    firstNode = q->head;
    q->head = q->head->next;
    q->iSize--;
  }else{
    return NULL;
  }
  if(q->iSize == 0){
    q->tail=NULL;
  }
  void* returnVal = firstNode->element;
  free(firstNode);
  return returnVal;
}

int qIsEmpty(Queue* q)
{
  if(q->iSize > 0)
     return 0;
  return 1;
}