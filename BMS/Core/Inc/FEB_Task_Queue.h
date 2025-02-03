#ifndef INC_FEB_TASK_QUEUE_H_
#define INC_FEB_TASK_QUEUE_H_
#include <stdint.h>
#include <stdlib.h>
// ******************* Structs *******************

typedef struct {
	uint8_t command;
	uint8_t priority;
	void* pdsr;
	void* scsr;
} FEB_Task_t;

typedef struct {
	uint16_t len;
	FEB_Task_t* head;
	FEB_Task_t* tail;
} FEB_Task_Header_t;
// ******************* Helper functions ************
static void FEB_Queue(FEB_Task_Header_t, FEB_Task_t*);
static void FEB_Pop_Head(FEB_Task_Header_t);
static void FEB_Pop_Tail(FEB_Task_Header_t);

static void FEB_Queue(FEB_Task_Header_t header, FEB_Task_t* task){
	task->pdsr=0;
	task->scsr=0;
	if(header.head==0){
		header.head=task;
		header.tail=task;
	}
	else if(task->priority<=header.tail->priority){
		task->pdsr=(void*)header.tail;
		header.tail->scsr=(void*)task;
		header.tail=task;
	} else if (task->priority>header.head->priority) {
		task->scsr=(void*)header.head;
		header.head->pdsr=(void*)task;
		header.head=task;
	} else {
		FEB_Task_t* traversal=header.head;
		while(traversal && traversal->priority > task->priority){
			traversal=(FEB_Task_t*)traversal->scsr;
		}
		task->pdsr=traversal->pdsr;
		task->scsr=(struct FEB_Task_t*)traversal;
		((FEB_Task_t*)traversal->pdsr)->scsr=(void*)task;
		traversal->pdsr=(void*)task;
	}
	header.len++;
}
static void FEB_Pop_Head(FEB_Task_Header_t header){
	FEB_Task_t*oldHead=header.head;
	if(oldHead){
		header.head=(FEB_Task_t*)oldHead->scsr;
		free(oldHead);
	}
	if(header.head){
		header.head->pdsr=0;
	} else {
		header.tail=0;
	}
}
static void FEB_Pop_Tail(FEB_Task_Header_t header){
	FEB_Task_t*oldTail=header.tail;
	if(oldTail){
		header.tail=(FEB_Task_t*)oldTail->pdsr;
		free(oldTail);
	}
	if(header.tail){
		header.tail->scsr=0;
	} else {
		header.head=0;
	}
}
#endif /* INC_FEB_TASK_QUEUE_H_ */
