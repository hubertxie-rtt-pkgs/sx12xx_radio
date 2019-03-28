/*!
 * \file      timer.c
 *
 * \brief     Timer objects and scheduling management implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include "board.h"
#include "rtthread.h"
#include "timer.h"






/*!
 * \brief Adds or replace the head timer of the list.
 *
 * \remark The list is automatically sorted. The list head always contains the
 *         next timer to expire.
 *
 * \param [IN]  obj Timer object to be become the new head
 * \param [IN]  remainingTime Remaining time of the previous head to be replaced
 */
static void TimerInsertNewHeadTimer( TimerEvent_t *obj );

/*!
 * \brief Adds a timer to the list.
 *
 * \remark The list is automatically sorted. The list head always contains the
 *         next timer to expire.
 *
 * \param [IN]  obj Timer object to be added to the list
 * \param [IN]  remainingTime Remaining time of the running head after which the object may be added
 */
static void TimerInsertTimer( TimerEvent_t *obj );

/*!
 * \brief Sets a timeout with the duration "timestamp"
 *
 * \param [IN] timestamp Delay duration
 */
static void TimerSetTimeout( TimerEvent_t *obj );

/*!
 * \brief Check if the Object to be added is not already in the list
 *
 * \param [IN] timestamp Delay duration
 * \retval true (the object is already in the list) or false
 */
static bool TimerExists( TimerEvent_t *obj );

void TimerInit( TimerEvent_t *obj, void ( *callback )( void *context ) )
{
	RT_ASSERT(obj);
	rt_timer_init(obj, "RadioTim", callback, NULL, 1000, RT_TIMER_FLAG_SOFT_TIMER);

}

void TimerSetContext( TimerEvent_t *obj, void* context )
{
    obj->Context = context;
}

void TimerStart( TimerEvent_t *obj )
{
	RT_ASSERT(obj);
    rt_timer_start(obj);
}


void TimerStop( TimerEvent_t *obj )
{
	RT_ASSERT(obj);
    rt_timer_stop(obj);
}

static bool TimerExists( TimerEvent_t *obj )
{
    return obj->parent.flag&RT_TIMER_FLAG_ACTIVATED;    
}

void TimerReset( TimerEvent_t *obj )
{
    TimerStop( obj );
    TimerStart( obj );
}

void TimerSetValue( TimerEvent_t *obj, uint32_t value )
{
    rt_tick_t tick;
    tick = TIME_TO_OSTICK(value);
    rt_timer_control(obj, RT_TIMER_CTRL_SET_TIME, &tick);
}



TimerTime_t TimerGetCurrentTime( void )
{
    return  rt_tick_get( );
}

TimerTime_t TimerGetElapsedTime( TimerTime_t past )
{
    if ( past == 0 )
    {
        return 0;
    }
    uint32_t nowInTicks = RtcGetTimerValue( );
    uint32_t pastInTicks = RtcMs2Tick( past );

    // Intentional wrap around. Works Ok if tick duration below 1ms
    return RtcTick2Ms( nowInTicks - pastInTicks );
}

static void TimerSetTimeout( TimerEvent_t *obj )
{
    int32_t minTicks= RtcGetMinimumTimeout( );
    obj->IsNext2Expire = true;

    // In case deadline too soon
    if( obj->Timestamp  < ( RtcGetTimerElapsedTime( ) + minTicks ) )
    {
        obj->Timestamp = RtcGetTimerElapsedTime( ) + minTicks;
    }
    RtcSetAlarm( obj->Timestamp );
}

TimerTime_t TimerTempCompensation( TimerTime_t period, float temperature )
{
//    return RtcTempCompensation( period, temperature );
}

void TimerProcess( void )
{
//    RtcProcess( );
}
