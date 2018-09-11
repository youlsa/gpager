/*
 * event.c
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"

typedef struct
{
	uint8_t   first;
	uint8_t   last;
	uint8_t   valid;
	uint8_t   items;
	event_t  *event;
} queue_t;

#define EVENTS  10
static event_t events[EVENTS];
static queue_t queue;

void queue_clear(queue_t *q)
{
	q->valid =  0;
	q->first =  0;
	q->last  =  0;
}

void queue_init(queue_t *q, uint8_t items, event_t *event)
{
	q->valid =  0;
	q->first =  0;
	q->last  =  0;
	q->items =  items;
	q->event =  event;
}

bool queue_empty(queue_t *q)
{
	return (q->valid==0);
}

bool queue_full(queue_t *q)
{
	return (q->valid >= q->items);
}

bool queue_put(queue_t *q, uint32_t e, uint32_t v, void (*callback)(void))
{
	if(queue_full(q))
	{
		//DPRINTF("  QUEUE: full\n");
		return false;
	}
	else
	{
		q->valid++;
		q->event[q->last].e = e;
		q->event[q->last].v = v;
		q->event[q->last].Callback = callback;
		q->last = (q->last+1) % q->items;
		return true;
	}
}

bool queue_get(queue_t *q, event_t *event, bool remain)
{
	if(queue_empty(q))
	{
		//DPRINTF("The queue is empty\n");
		return false;
	}
	else
	{
		memcpy(event, &q->event[q->first], sizeof(event_t));
		if(!remain)
		{
			q->first=(q->first+1) % q->items;
			q->valid--;
		}
		return true;
	}
}

void event_init(void)
{
	queue_init(&queue, EVENTS, events);
}

bool event_put(uint32_t e, uint32_t v, void (*callback)(void))
{
	return queue_put(&queue, e, v, callback);
}

bool event_get(event_t *event, bool remain)
{
	return queue_get(&queue, event, remain);
}

bool event_empty(void)
{
	return queue_empty(&queue);
}
