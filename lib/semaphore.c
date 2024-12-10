// User-level Semaphore

#include "inc/lib.h"

struct semaphore create_semaphore(char *semaphoreName, uint32 value)
{
	//TODO: [PROJECT'24.MS3 - #02] [2] USER-LEVEL SEMAPHORE - create_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("create_semaphore is not implemented yet");
	//Your Code is Here...
//	cprintf("Allocating New Semaphore\n");
	struct __semdata* newSemdata = smalloc(semaphoreName, sizeof(struct __semdata), 1);

	if (newSemdata == NULL){
		panic("Failed To Create New Semaphore!\n");
	}
//	cprintf("Initializing Semaphore Data\n");
	strncpy(newSemdata->name, semaphoreName, sizeof(newSemdata->name) - 1);
	newSemdata->name[sizeof(newSemdata->name) - 1] = '\0';
	newSemdata->count = value;
	sys_init_queue(&newSemdata->queue);

	struct semaphore wraper = {newSemdata};
//	cprintf("Done Creating Semaphore\n");
	return wraper;
}

struct semaphore get_semaphore(int32 ownerEnvID, char* semaphoreName)
{
	//TODO: [PROJECT'24.MS3 - #03] [2] USER-LEVEL SEMAPHORE - get_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("get_semaphore is not implemented yet");
	//Your Code is Here...
//	cprintf("Getting Semaphore\n");
	struct __semdata* retreivedSemdata = sget(ownerEnvID, semaphoreName);

	if (retreivedSemdata == NULL){
		panic("Failed To Get The Semaphore!\n");
	}

	struct semaphore wraper = {retreivedSemdata};
//	cprintf("Done Getting Semaphore\n");
	return wraper;
}

void wait_semaphore(struct semaphore sem)
{
	//TODO: [PROJECT'24.MS3 - #04] [2] USER-LEVEL SEMAPHORE - wait_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("wait_semaphore is not implemented yet");
	//Your Code is Here...
//	cprintf("[wait] Acquire Semaphore Lock\n");
	sem.semdata->lock = 0;
	while(xchg(&(sem.semdata->lock),1) != 0);

//	cprintf("[wait] Count: %d\n", semaphore_count(sem));
	sem.semdata->count--;
	if (semaphore_count(sem) < 0){
//		cprintf("[wait] Insert Process Into Queue\n");
		sys_enqueue(&sem.semdata->queue);
//		cprintf("[wait] Set Process as Blocked\n");
		sem.semdata->lock = 0;
		myEnv->env_status = ENV_BLOCKED;
	}
//	cprintf("[wait] Release Semaphore Lock\n");
	sem.semdata->lock = 0;
}

void signal_semaphore(struct semaphore sem)
{
	//TODO: [PROJECT'24.MS3 - #05] [2] USER-LEVEL SEMAPHORE - signal_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("signal_semaphore is not implemented yet");
	//Your Code is Here...
//	cprintf("[signal] Acquire Semaphore Lock\n");
	sem.semdata->lock = 0;
	while(xchg(&(sem.semdata->lock),1) != 0);
//	cprintf("[signal] Count: %d\n", semaphore_count(sem));
	sem.semdata->count++;
	if (semaphore_count(sem) <= 0){
		struct Env currentEnv;
//		cprintf("[signal] Pop Process From Queue\n");
		sys_dequeue(&sem.semdata->queue, &currentEnv);
//		cprintf("[signal] Popped Process: %x\n", &currentEnv);
//		cprintf("[signal] Insert Process Into Ready-List\n");
		sys_sched_insert_ready(&currentEnv);
	}
//	cprintf("[signal] Release Semaphore Lock\n");
	sem.semdata->lock = 0;
}

int semaphore_count(struct semaphore sem)
{
	return sem.semdata->count;
}
