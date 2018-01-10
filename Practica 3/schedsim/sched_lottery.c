/*
 * sched_lottery.c
 *
 *  Created on: 14/12/2017
 *      Author: usuario_local
 */
#include "sched.h"
int lot_quantum=3;

// Structure to store RR thread-specific fields
struct lottery_data {
	int number_tickets;
	int remaining_ticks_slice;
};

static int task_new_lottery(task_t* t)
{
	struct lottery_data* cs_data=malloc(sizeof(struct lottery_data));

	if (!cs_data)
		return 1;  //Cannot reserve memory

	// initialize the quantum

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
	cs_data->remaining_ticks_slice=lot_quantum;
	t->tcs_data=cs_data;

	return 0;
}

static task_t* pick_next_task_lottery(runqueue_t* rq)
{
	srand(time(NULL));
	struct lottery_data* cs_data=malloc(sizeof(struct lottery_data));
	int temp = 0;
	task_t* aux = head_slist(&rq->tasks);
	while(aux){
		cs_data = (struct lottery_data*) aux->tcs_data;
		temp += cs_data->number_tickets;
		aux = next_slist(&rq->tasks, aux);
	}
	if(temp){
		//RECORRO LA COLA PARA SABER LOS TICKETS
		int n = rand() % temp;
		int ini = 0, fin=0;
		aux = head_slist(&rq->tasks);
		cs_data = (struct lottery_data*) aux->tcs_data;
		fin = cs_data->number_tickets;

		while(!(ini<n && fin > n)){
			aux = next_slist(&rq->tasks, aux);
			ini = fin + 1;
			cs_data = (struct lottery_data*) aux->tcs_data;
			fin += cs_data->number_tickets;
		}
		cs_data->number_tickets--;
		cs_data->remaining_ticks_slice = lot_quantum;
	}
	// Current is not on the rq -> let's remove it
	if (aux){
		aux->tcs_data = cs_data;
		remove_slist(&rq->tasks,aux);
	}

	return aux;
}

static void enqueue_task_lottery(task_t* t,runqueue_t* rq, int preempted)
{
	if (t->on_rq || is_idle_task(t))
		return;

	insert_slist(&rq->tasks,t); //Push task
	struct lottery_data* cs_data=(struct lottery_data*) t->tcs_data;
	cs_data->remaining_ticks_slice=lot_quantum; // Reset slice
}

static void task_tick_lottery(runqueue_t* rq)
{
	task_t* current=rq->cur_task;
	struct lottery_data* cs_data=(struct lottery_data*) current->tcs_data;

	if (is_idle_task(current))
		return;

	cs_data->remaining_ticks_slice--; // Charge tick

	if (cs_data->remaining_ticks_slice<=0)
		rq->need_resched=TRUE; //Force a resched !!
}

static task_t* steal_task_lottery(runqueue_t* rq)
{
	task_t* t = tail_slist(&rq->tasks);

	if (t)
		remove_slist(&rq->tasks,t);

	return t;
}

static void task_free_lottery(task_t* t)
{
	if (t->tcs_data) {
		free(t->tcs_data);
		t->tcs_data=NULL;
	}
}
sched_class_t lottery_sched= {
	.task_new=task_new_lottery,
	.task_free=task_free_lottery,
	.pick_next_task=pick_next_task_lottery,
	.enqueue_task=enqueue_task_lottery,
	.task_tick=task_tick_lottery,
	.steal_task=steal_task_lottery
};

