#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include<stdio.h>
#include<memory.h>
#include<stdlib.h>
#include<queue>
using namespace std;

typedef double PriorityType;

typedef struct tagePQNode{
    PriorityType Priority;
    pair<int,int >* Data;
}PQNode;

typedef struct tagPriorityQueue{
    PQNode* Nodes;
    int Capacity;
    int UsedSize;
}PriorityQueue;

PriorityQueue* PQ_Create(int InitialSize);
void PQ_Destroy(PriorityQueue* PQ);
void PQ_Enqueue(PriorityQueue* PQ, PQNode NewData);
void PQ_Dequeue(PriorityQueue* PQ, PQNode* Root);
int PQ_GetParent(int Index);
int PQ_GetLeftChild(int Index);
void PQ_SwapNodes(PriorityQueue* PQ, int Index1, int Index2);
int PQ_IsEmpty(PriorityQueue* PQ);
int PQ_Refresh(PriorityQueue* PQ, PQNode* tNode);

#endif PRIORITYQUEUE_H
