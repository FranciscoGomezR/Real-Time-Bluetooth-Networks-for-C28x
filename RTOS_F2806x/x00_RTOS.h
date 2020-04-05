/*
 * x00_RTOS.h
 *
 *  Created on: Mar 21, 2020
 *      Author: JuanFco
 */
#ifndef X00_RTOS_H_
#define X00_RTOS_H_
/************************************************************************************
*	Copyright  			    									                    *
*   All Rights Reserved																*
*   The Copyright symbol does not also indicate public availability or publication.	*
* 																				   	*
* 							"Francisco Gomez"									    *
* 																					*
* - Driver:   			"RTOS with Event Threads".									*
* 																					*
* - Compiler:           Code Composer Studio (Licensed)								*
* - Version:			9.3.0.00012												    *
* - Supported devices:  "F28xxxx" 										            *
* 																					*
* - AppNote:			This code is base on the following course:	    			*
* https://courses.edx.org/courses/course-v1:UTAustinX+UT.RTBN.12.01x+2T2018/course/ *
*																					*
* - Created by: 		"Francisco Gomez"											*
* - Date Created: 		"02/04/2020"											    *
* - Contact:			"juan.fco.gomez.ruiz@gmail.com"								*
* 																					*
* - Description: 		Scheduler is a Round-Robin with Priority level, blocking	*
* 	                    and sleeping features. Include Periodic Event-threads       *
* 	                    Handler.                                                    *
* 	                    I know TI already provides you a RTOS, more reliable and    *
* 	                    and robust.                                                 *
* 	                    This RTOS is base on he coursera-course:                    *
* 	                    Real-Time Bluetooth Networks - Shape the World              *
* 	                    UTAustinX: UT.RTBN.12.01x                                   *
* 	                    (that RTOS is implemented for ARM types)                    *
* 	                    the intention of this code is to implement that knowledge   *
* 	                    on C28x processor in order to share it, make it robust,     *
* 	                    reliable and little-bit more complex through                *
* 	                    your collaboration                                          *
*   					-  	- 														*
* - Repository:                                                                     *
*   https://Francisco_GomezR@bitbucket.org/Francisco_GomezR/rtos_for_c28x.git       *
* 																				2012*
*************************************************************************************

*************************************************************************************
* 	Revision History:
*
*   Date          	CP#           Author
*   DD-MM-YYYY      XXXXX:1		Initials	Description of change
*   -----------   ------------  ---------   ------------------------------------
*  	02-04-2020		1.0			JFGR		Initial Code (No bug detected, fairly tested)
*
*************************************************************************************
*
* File/

*  "More detail description of the code"
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
*  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
*  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
*  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
*  DEALINGS IN THE SOFTWARE.
*
*/
//*****************************************************************************
//
//			INCLUDE FILE SECTION FOR THIS MODULE
//
//*****************************************************************************
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "F2806x_Cla_typedefs.h"// F2806x CLA Type definitions
#include "F2806x_Device.h"      // F2806x Headerfile Include File
#include "04_Utilities/x41_Utilities.h"

//*****************************************************************************
//
//			PUBLIC DEFINES SECTION
//
//*****************************************************************************
#define OS_SCHEDULER_FREQ                   90        //MHz
#define OS_AUX_TIMER_FREQ                   90        //MHz

#define OS_TASKS_TIMESLICE_US               10000                       //3ms
#define OS_EVENT_TIMETICK__US               1000                        //1ms

#define NUM_EVENTS                          3                           // maximum number of Events
#define NUM_THREADS                         5                           // maximum number of threads
#define NUM_GLOBAL_THREADS                  NUM_EVENTS + NUM_THREADS

#define STACKSIZE                           500                            // number of 32-bit words in stack

//Semaphores
#define OS_SEMAPHORE_FREE                   1
#define OS_SEMAPHORE_WAIT                   0

//Thread blocking status
#define OS_THREAD_BLOCKSTATUS_FREE          1
#define OS_THREAD_BLOCKSTATUS_BUSY		    0
#define OS_THREAD_BLOCKSTATUS_AWAKE         0
#define OS_THREAD_BLOCKSTATUS_1_WAITING	    -1
#define OS_THREAD_BLOCKSTATUS_2_WAITING     -2

#define OS_SLEEPCOUNTER_RESET			    0

//Priorities
#define OS_PRIOROTY_MAXIMUM_LEVEL           0
#define OS_PRIOROTY_MINIMUM_LEVEL           9

#define DISABLE_GLOBAL_INT()			    DINT				//Disable Maskable Interrupts (Set INTM Bit)
#define ENABLE__GLOBAL_INT()			    EINT				//Enable Maskable Interrupts (Clear INTM Bit) .

#define AUX_HW_TIMER_CTRL                   ENABLE
#define GPIO13_FOR_DEBUGGING                ENABLE
#define RST_SLICE_T_AT_SUSPEND              DISABLE

//*****************************************************************************
//
//			PUBLIC STRUCTs, UNIONs ADN ENUMs SECTION
//
//*****************************************************************************
struct struct_TaskCtrlBlock{
    Uint32 						*ThreadSP;          // pointer to stack
    Uint16                      StaticPriority;     // Initial Priority value - Field
    Uint16                      WorkingPriority;    // Current Priority value for Thread
    struct struct_TaskCtrlBlock *nextThreadSP;      // linked-list pointer
    Uint16                      Interrupted;        // Flag to indicate thread was interrupted
    int16                       *ptrBlockStatus;    // Pointer to a Semaphore that indicates if this Thread is in Blocked State or not.
    Uint32                       ptrSleepTicks;     // Ticks counter that this thread will reaming in Sleep State.
    Uint16                      Type;               // Identify if is a Thread or and Event
};


struct struct_PeriodicEvents{
    int16 *ptrSemaphore;
    Uint32 Ticks;
};
typedef struct struct_PeriodicEvents s_PeriodicEvent;

enum{
    OS_RealTime_Thread_ID0=0,
    OS_RealTime_Thread_ID1,
    OS_RealTime_Thread_ID2,
    OS_RealTime_MaxID
};

enum{
    OS_Standard_Thread_ID0 = OS_RealTime_MaxID,
    OS_Standard_Thread_ID1,
    OS_Standard_Thread_ID2,
    OS_Standard_Thread_ID3,
    OS_Standard_Thread_ID4,
    OS_Global_Thread_IDmax
};

enum{
    state0_NormalFlow=0,
    state1_EventDispatch,
    state2_ResumeThreadFlow
};


//struct struct_PeriodicEvents{
//    void (* ptrfcn_Event) (void);
//    Uint32 Ticks;
//};

/*enum{
    OS_Thread_ID0=0,
    OS_Thread_ID1,
    OS_Thread_ID2,
    OS_Thread_ID3,
    OS_Thread_ID4,
    OS_Thread_IDmax
};

enum{
    OS_TimeEvent_ID0=0,
    OS_TimeEvent_ID1,
    OS_TimeEvent_ID2,
    OS_TimeEvent_IDmax
};*/

//*****************************************************************************
//
//			PUBLIC VARIABLES PROTOTYPE
//
//*****************************************************************************
extern s_PeriodicEvent sa_PeriodicEvents[];

//*****************************************************************************
//
//			PUBLIC FUNCTIONS PROTOYPES
//
//*****************************************************************************

//Uint32 OS_u32_AddThreads( void(* pTask))

// ******** OS_Init ************
// initialize operating system, disable interrupts until OS_Launch
// initialize OS controlled I/O: systick, 80 MHz PLL
// input:  none
// output: none
void OS_Initialization(void);

//******** OS_AddThread ***************
// add three foregound threads to the scheduler
// Inputs: three pointers to a void/void foreground tasks
// Outputs: 1 if successful, 0 if this thread can not be added
Uint32 OS_u32_AddThreads( void(*ptrfcn_Event)(void), Uint16 ThreadID, Uint16 NextThreadID, Uint16 Priority);

//******** OS_FirstThreadToRun ***************
// Set first Thread to Run by scheduler
// Inputs: the Id of the THREAD
void OS_FirstThreadToRun(Uint16 ThreadID);

//******** OS_AddPeriodicEventThread ***************
// Add one background periodic event thread
// Typically this function receives the highest priority
// Inputs: pointer to a void/void event thread function
//         period given in units of OS_Launch
// Outputs: 1 if successful, 0 if this thread cannot be added
// It is assumed that the event threads will run to completion and return
// It is assumed the time to run these event threads is short compared to 1 msec
// These threads cannot spin, block, loop, sleep, or kill
// These threads can call OS_Signal
//Uint32 OS_AddPeriodicEventThread(void(*ptrfcn_Thread)(void), Uint32 Period_ms);
void OS_AddPeriodicEventThread(struct struct_PeriodicEvents *ptrfcn_Event, int16 *ptrSemaphore, Uint32 Period_ms);

//******** OS_Launch ***************
// start the scheduler, enable interrupts
// Inputs: number of clock cycles for each time slice
//         (maximum of 24 bits)
// Outputs: none (does not return)
void OS_Launch(Uint32 TimeSlice_us);

//******** OS_Suspend ***************
// Called by main thread to cooperatively suspend operation
// Inputs: none
// Outputs: none
// Will be run again depending on sleep/block status
void OS_Suspend(void);

// ******** OS_Sleep ************
// place this thread into a dormant state
// input:  number of msec to sleep
// output: none
// OS_Sleep(0) implements cooperative multitasking
void OS_Sleep(Uint32 osTasksleepTime);

// ******** OS_InitSemaphore ************
// Initialize counting semaphore
// Inputs:  pointer to a semaphore
//          initial value of semaphore
// Outputs: none
void OS_InitSemaphore(int16 *ptrSemaphore, int16 value);

// ******** OS_Signal ************
// Increment semaphore
// Lab2 spinlock
// Lab3 wakeup blocked thread if appropriate
// Inputs:  pointer to a counting semaphore
// Outputs: none
void OS_Signal(int16 *ptrSemaphore);

// ******** OS_Wait ************
// Decrement semaphore and block if less than zero
// Lab2 spinlock (does not suspend while spinning)
// Lab3 block if less than zero
// Inputs:  pointer to a counting semaphore
// Outputs: none
void OS_Wait(int16 *ptrSemaphore);


#endif /* X00_RTOS_H_ */
