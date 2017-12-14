/*
 * sched_lottery.c
 *
 *  Created on: 14/12/2017
 *      Author: usuario_local
 */
#include "sched.h"

/* Structure to store RR thread-specific fields */
struct lottery_data {
	int number_tickets;
};

static int task_new_lottery(task_t* t)
{
	struct lottery_data* cs_data=malloc(sizeof(struct lottery_data));

	if (!cs_data)
		return 1;  /* Cannot reserve memory */

	/* initialize the quantum */

	switch(t->prio){
	case 1:
		cs_data->number_tickets=50;
		break;
	case 2:
		cs_data->number_tickets=25;
		break;
	case 3:
		cs_data->number_tickets=10;
		break;
	case 4:
		cs_data->number_tickets=5;
		break;
	default:
		cs_data->number_tickets = 0;
		break;
	}
	t->tcs_data=cs_data;
	return 0;
}

static void task_free_lottery(task_t* t)
{
	if (t->tcs_data) {
		free(t->tcs_data);
		t->tcs_data=NULL;
	}
}

static task_t* pick_next_task_lottery(runqueue_t* rq)
{
	//RECORRO LA COLA PARA SABER LOS TICKETS
	int n = rand()%;

	task_t* t=head_slist(&rq->tasks);

	/* Current is not on the rq -> let's remove it */
	if (t)
		remove_slist(&rq->tasks,t);

	return t;
}

static void enqueue_task_lottery(task_t* t,runqueue_t* rq, int preempted)
{
	struct rr_data* cs_data=(struct rr_data*) t->tcs_data;

	if (t->on_rq || is_idle_task(t))
		return;

	insert_slist(&rq->tasks,t); //Push task
	cs_data->remaining_ticks_slice=rr_quantum; // Reset slice
}

static void task_tick_lottery(runqueue_t* rq)
{
	task_t* current=rq->cur_task;
	struct rr_data* cs_data=(struct rr_data*) current->tcs_data;

	if (is_idle_task(current))
		return;

	cs_data->remaining_ticks_slice--; /* Charge tick */

	if (cs_data->remaining_ticks_slice<=0)
		rq->need_resched=TRUE; //Force a resched !!
}

static task_t* steal_task_lottery(runqueue_t* rq)
{
	task_t* t=tail_slist(&rq->tasks);

	if (t)
		remove_slist(&rq->tasks,t);

	return t;
}

sched_class_t lottery_sched= {
	.task_new=task_new_lottery,
	.task_free=task_free_lottery,
	.pick_next_task=pick_next_task_lottery,
	.enqueue_task=enqueue_task_lottery,
	.task_tick=task_tick_lottery,
	.steal_task=steal_task_lottery
};
