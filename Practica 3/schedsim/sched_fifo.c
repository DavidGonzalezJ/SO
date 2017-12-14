/*
 * sched_fifo.c
 *
 *  Created on: 14/12/2017
 *      Author: usuario_local
 */


#include "sched.h"


static task_t* pick_next_task_fifo(runqueue_t* rq)
{
	task_t* t=head_slist(&rq->tasks);

	/* Current is not on the rq -> let's remove it */
	if (t)
		remove_slist(&rq->tasks,t);

	return t;
}

static void enqueue_task_fifo(task_t* t,runqueue_t* rq, int preempted)
{
	if (t->on_rq || is_idle_task(t))
		return;

	insert_slist(&rq->tasks,t); //Push task
}

static task_t* steal_task_fifo(runqueue_t* rq)
{
	task_t* t=tail_slist(&rq->tasks);

	if (t)
		remove_slist(&rq->tasks,t);

	return t;
}

sched_class_t fifo_sched= {
	.pick_next_task=pick_next_task_fifo,
	.enqueue_task=enqueue_task_fifo,
	.steal_task=steal_task_fifo,
};
