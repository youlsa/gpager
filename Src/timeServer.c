/*
 * timeServer.c
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

/* Includes ------------------------------------------------------------------*/
#include <time.h>
#include "hw.h"
#include "timeServer.h"

/*!
 * safely execute call back
 */
#define exec_cb( _callback_ )     \
  do {                          \
      if( _callback_ == NULL )    \
      {                           \
        while(1);                 \
      }                           \
      else                        \
      {                           \
        _callback_();               \
      }                           \
  } while(0);

/*!
 * safely event put
 */
#define event_cb( _callback_ )     						\
  do {                          						\
      if( _callback_ == NULL )    						\
      {                           						\
        while(1);                 						\
      }                           						\
      else                        						\
      {                           						\
        event_put(EVENT_TIMER, 0, _callback_);     \
      }                           						\
  } while(0);

/*!
 * Timers list head pointer
 */
static TimerEvent_t *TimerListHead = NULL;

/*!
 * \brief Adds or replace the head timer of the list.
 *
 * \remark The list is automatically sorted. The list head always contains the
 *     next timer to expire.
 *
 * \param [IN]  obj Timer object to be become the new head
 * \param [IN]  remainingTime Remaining time of the previous head to be replaced
 */
static void TimerInsertNewHeadTimer(TimerEvent_t *obj);

/*!
 * \brief Adds a timer to the list.
 *
 * \remark The list is automatically sorted. The list head always contains the
 *     next timer to expire.
 *
 * \param [IN]  obj Timer object to be added to the list
 * \param [IN]  remainingTime Remaining time of the running head after which the object may be added
 */
static void TimerInsertTimer(TimerEvent_t *obj);

/*!
 * \brief Sets a timeout with the duration "timestamp"
 *
 * \param [IN] timestamp Delay duration
 */
static void TimerSetTimeout(TimerEvent_t *obj);

/*!
 * \brief Check if the Object to be added is not already in the list
 *
 * \param [IN] timestamp Delay duration
 * \retval true (the object is already in the list) or false
 */
static bool TimerExists(TimerEvent_t *obj);

void TimerInit(TimerEvent_t *obj, void (*callback)(void))
{
	obj->Timestamp = 0;
	obj->ReloadValue = 0;
	obj->IsRunning = false;
	obj->Callback = callback;
	obj->Next = NULL;
}

void TimerStart(TimerEvent_t *obj)
{
	uint32_t elapsedTime = 0;

	BACKUP_PRIMASK();

	DISABLE_IRQ();

	if((obj == NULL) || (TimerExists(obj) == true))
	{
		RESTORE_PRIMASK();
		return;
	}
	obj->Timestamp = obj->ReloadValue;
	obj->IsRunning = false;

	if(TimerListHead == NULL)
	{
		HW_RTC_SetTimerContext();
		TimerInsertNewHeadTimer(obj); // insert a timeout at now+obj->Timestamp
	}
	else
	{
		elapsedTime = HW_RTC_GetTimerElapsedTime();
		obj->Timestamp += elapsedTime;

		if(obj->Timestamp < TimerListHead->Timestamp)
		{
			TimerInsertNewHeadTimer(obj);
		}
		else
		{
			TimerInsertTimer(obj);
		}
	}

	RESTORE_PRIMASK();
}

static void TimerInsertTimer(TimerEvent_t *obj)
{
	TimerEvent_t* cur = TimerListHead;
	TimerEvent_t* next = TimerListHead->Next;

	while(cur->Next != NULL)
	{
		if(obj->Timestamp  > next->Timestamp)
		{
			cur = next;
			next = next->Next;
		}
		else
		{
			cur->Next = obj;
			obj->Next = next;
			return;
		}
	}

	cur->Next = obj;
	obj->Next = NULL;
}

static void TimerInsertNewHeadTimer(TimerEvent_t *obj)
{
	TimerEvent_t* cur = TimerListHead;

	if(cur != NULL)
	{
		cur->IsRunning = false;
	}

	obj->Next = cur;
	TimerListHead = obj;
	TimerSetTimeout(TimerListHead);
}

void TimerIrqHandler(void)
{
	TimerEvent_t* cur;
	TimerEvent_t* next;

	uint32_t old =  HW_RTC_GetTimerContext();
	uint32_t now =  HW_RTC_SetTimerContext();
	uint32_t DeltaContext = now - old; //intentionnal wrap around

	/* update timeStamp based upon new Time Reference*/
	/* beacuse delta context should never exceed 2^32*/
	if(TimerListHead != NULL)
	{
		for(cur=TimerListHead; cur->Next != NULL; cur= cur->Next)
		{
			next =cur->Next;
			if(next->Timestamp > DeltaContext)
			{
				next->Timestamp -= DeltaContext;
			}
			else
			{
				next->Timestamp = 0 ;
			}
		}
	}

	/* execute imediately the alarm callback */
	if(TimerListHead != NULL)
	{
		cur = TimerListHead;
		TimerListHead = TimerListHead->Next;
//		exec_cb( cur->Callback );
		event_cb( cur->Callback );
	}


	// remove all the expired object from the list
	while((TimerListHead != NULL) && (TimerListHead->Timestamp < HW_RTC_GetTimerElapsedTime()))
	{
		cur = TimerListHead;
		TimerListHead = TimerListHead->Next;
//		exec_cb( cur->Callback );
		event_cb( cur->Callback );
	}

	/* start the next TimerListHead if it exists AND NOT running */
	if((TimerListHead != NULL) && (TimerListHead->IsRunning == false))
	{
		TimerSetTimeout(TimerListHead);
	}
}

void TimerStop(TimerEvent_t *obj)
{
	BACKUP_PRIMASK();

	DISABLE_IRQ();

	TimerEvent_t* prev = TimerListHead;
	TimerEvent_t* cur = TimerListHead;

	// List is empty or the Obj to stop does not exist
	if((TimerListHead == NULL) || (obj == NULL))
	{
		RESTORE_PRIMASK();
		return;
	}

	if(TimerListHead == obj) // Stop the Head
	{
		if(TimerListHead->IsRunning == true) // The head is already running
		{
			if(TimerListHead->Next != NULL)
			{
				TimerListHead->IsRunning = false;
				TimerListHead = TimerListHead->Next;
				TimerSetTimeout(TimerListHead);
			}
			else
			{
				HW_RTC_StopAlarm();
				TimerListHead = NULL;
			}
		}
		else // Stop the head before it is started
		{
			if(TimerListHead->Next != NULL)
			{
				TimerListHead = TimerListHead->Next;
			}
			else
			{
				TimerListHead = NULL;
			}
		}
	}
	else // Stop an object within the list
	{
		while(cur != NULL)
		{
			if(cur == obj)
			{
				if(cur->Next != NULL)
				{
					cur = cur->Next;
					prev->Next = cur;
				}
				else
				{
					cur = NULL;
					prev->Next = cur;
				}
				break;
			}
			else
			{
				prev = cur;
				cur = cur->Next;
			}
		}
	}

	RESTORE_PRIMASK();
}

static bool TimerExists(TimerEvent_t *obj)
{
	TimerEvent_t* cur = TimerListHead;

	while(cur != NULL)
	{
		if(cur == obj)
		{
			return true;
		}
		cur = cur->Next;
	}

	return false;
}

void TimerReset(TimerEvent_t *obj)
{
	TimerStop(obj);
	TimerStart(obj);
}

void TimerSetValue(TimerEvent_t *obj, uint32_t value)
{
	uint32_t minValue = 0;
	uint32_t ticks = HW_RTC_ms2Tick(value);

	TimerStop(obj);

	minValue = HW_RTC_GetMinimumTimeout();

	if(ticks < minValue)
	{
		ticks = minValue;
	}

	obj->Timestamp = ticks;
	obj->ReloadValue = ticks;
}

TimerTime_t TimerGetCurrentTime(void)
{
	uint32_t now = HW_RTC_GetTimerValue();
	return  HW_RTC_Tick2ms(now);
}

TimerTime_t TimerGetElapsedTime(TimerTime_t past)
{
	uint32_t nowInTicks = HW_RTC_GetTimerValue();
	uint32_t pastInTicks = HW_RTC_ms2Tick(past);
	/* intentional wrap around. Works Ok if tick duation below 1ms */
	return HW_RTC_Tick2ms(nowInTicks- pastInTicks);
}

static void TimerSetTimeout(TimerEvent_t *obj)
{
	int32_t minTicks= HW_RTC_GetMinimumTimeout();
	obj->IsRunning = true;

	//in case deadline too soon
	if(obj->Timestamp  < (HW_RTC_GetTimerElapsedTime() + minTicks))
	{
		obj->Timestamp = HW_RTC_GetTimerElapsedTime() + minTicks;
	}
	HW_RTC_SetAlarm(obj->Timestamp);
}
