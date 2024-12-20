// User-level Semaphore

#include "inc/lib.h"

struct semaphore create_semaphore(char *semaphoreName, uint32 value)
{
	//TODO: [PROJECT'24.MS3 - #02] [2] USER-LEVEL SEMAPHORE - create_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("create_semaphore is not implemented yet");
	//Your Code is Here...
	struct __semdata* newSemdata = smalloc(semaphoreName, sizeof(struct __semdata), 1);

	if (newSemdata == NULL){
		panic("Failed To Create New Semaphore!\n");
	}

	strncpy(newSemdata->name, semaphoreName, sizeof(newSemdata->name) - 1);
	newSemdata->name[sizeof(newSemdata->name) - 1] = '\0';
	newSemdata->count = value;
	sys_init_queue(&newSemdata->queue);
	newSemdata->lock = 0;

	struct semaphore wraper = {newSemdata};

	return wraper;
}

struct semaphore get_semaphore(int32 ownerEnvID, char* semaphoreName)
{
	//TODO: [PROJECT'24.MS3 - #03] [2] USER-LEVEL SEMAPHORE - get_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("get_semaphore is not implemented yet");
	//Your Code is Here...
	struct __semdata* retreivedSemdata = sget(ownerEnvID, semaphoreName);

	if (retreivedSemdata == NULL){
		panic("Failed To Get The Semaphore!\n");
	}

	struct semaphore wraper = {retreivedSemdata};

	return wraper;
}

void wait_semaphore(struct semaphore sem)
{
	//TODO: [PROJECT'24.MS3 - #04] [2] USER-LEVEL SEMAPHORE - wait_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("wait_semaphore is not implemented yet");
	//Your Code is Here...
	while(xchg(&(sem.semdata->lock), 1) != 0);

	sem.semdata->count--;
	if (sem.semdata->count < 0){
		sys_wf_semaphore(sem.semdata);
	}

	sem.semdata->lock = 0;
}

void signal_semaphore(struct semaphore sem)
{
	//TODO: [PROJECT'24.MS3 - #05] [2] USER-LEVEL SEMAPHORE - signal_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("signal_semaphore is not implemented yet");
	//Your Code is Here...
	while(xchg(&(sem.semdata->lock), 1) != 0);

	sem.semdata->count++;
	if (sem.semdata->count <= 0){
		sys_sf_semaphore(sem.semdata);
	}
	sem.semdata->lock = 0;
}

int semaphore_count(struct semaphore sem)
{
	return sem.semdata->count;
}
