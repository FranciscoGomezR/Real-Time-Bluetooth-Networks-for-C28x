/*
 * x00_RTOS.c
 *
 *  Created on: Mar 21, 2020
 *      Author: JuanFco
 */

//*****************************************************************************
//
//			INCLUDE FILE SECTION FOR THIS MODULE
//
//*****************************************************************************
#include "x00_RTOS.h"
#include "x01_EVM_LED_Driver.h"
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

//*****************************************************************************
//
//			PRIVATE DEFINES SECTION - OWN BY THIS MODULE ONLY
//
//*****************************************************************************
#define ISR___C_FCN						0xF0
#define ISR_ASM_FCN						0x0F
#define RTOS_DEBUGG_CPUTIMER			ISR_ASM_FCN

#define OS_THREAD_INT__CLEAR            0x0000
#define OS_THREAD_INTERRUPED            0x00FF
#define OS_NORMAL_THREAD                0x00AA
#define OS_EVENT__THREAD                0xAA00
//*****************************************************************************
//
//			PRIVATE STRUCTs, UNIONs ADN ENUMs SECTION
//
//*****************************************************************************
typedef struct struct_TaskCtrlBlock  s_TaskCtrlBlock;
//typedef struct struct_PeriodicEvents s_PeriodicEvent;

typedef struct{
    Uint16  ToRun;
    Uint16  NoEventsTriggered;
}struct_SM_scheduler;

enum{
    S0_scheduler_Normal=0,
    S1_scheduler_Events,
    S2_scheduler_Resume
};

//*****************************************************************************
//
//			PUBLIC VARIABLES
//
//****************************************************************************
//Variables related to Periodic Threads
s_PeriodicEvent sa_PeriodicEvents[OS_RealTime_MaxID];

//*****************************************************************************
//
//			PRIVATE VARIABLES
//
//*****************************************************************************
//Variables related to the RTOs  Threads
s_TaskCtrlBlock *osExecTaskPtr;
s_TaskCtrlBlock sOS_TaskCtrlBlocks[OS_Global_Thread_IDmax];
Uint32 Stacks[OS_Global_Thread_IDmax][STACKSIZE];

//static Uint32 PeriodicEventsCounter=0;
static Uint32 PeriodicTicksCounter=0;
struct_SM_scheduler sm_Scheduler;

//*****************************************************************************
//
//			PRIVATE FUNCTIONS PROTOYPES
//
//*****************************************************************************
//Function define in .asm file
extern  void OS_Start(void);
        void OS_fcn_SetInitialStack(Uint32 i);
        void OS_Scheduler(void);

#define OS_RISE_EVENT_FLAG() {  osExecTaskPtr->Interrupted = OS_THREAD_INTERRUPED;  \
                                sm_Scheduler.NoEventsTriggered += 1;                \
                                sm_Scheduler.ToRun =S1_scheduler_Events;            }
extern  __interrupt void OS_SystemTick_Handler_isr(void);
        __interrupt void OS_PeriodicEventsTick_Handler_isr(void);
        void OC_SetStackContenToZero(Uint32 *ptrStack, Uint32 Size);

        #if RTOS_DEBUGG_CPUTIMER == ISR___C_FCN
        __interrupt void OS_Timing_isr(void);
        #endif

//*****************************************************************************
//
//			PUBLIC FUNCTIONS SECTION
//
//*****************************************************************************
/*****************************************************************************
 * Function: 	OS_InitSemaphore
 * Description:	Initialize counting semaphore
 * Caveats:
 * Parameters:	pointer to a semaphore
 * 				initial value of semaphore
 * Return:
 *****************************************************************************/
void OS_InitSemaphore(int16 *ptrSemaphore, int16 value)
{
	*ptrSemaphore = value;
}

/*****************************************************************************
 * Function: 	OS_Signal
 * Description:	Increment semaphore
 * Caveats:
 * Parameters:	pointer to a counting semaphore
 * Return:
 *****************************************************************************/
void OS_Signal(int16 *ptrSemaphore)
{
    static s_TaskCtrlBlock *ptrThreadToSearchFor;
	DISABLE_GLOBAL_INT();
	*ptrSemaphore = *ptrSemaphore + 1;
	if( (*ptrSemaphore) <= 0 )
	{
		//point to the next thread from the currently running.
		ptrThreadToSearchFor = osExecTaskPtr->nextThreadSP;
		//check the following thread in the loop/TCB list to see which one (and first semaphore)
		//semaphore has blocked the object
		while(ptrThreadToSearchFor->ptrBlockStatus != ptrSemaphore )
		{
			ptrThreadToSearchFor = ptrThreadToSearchFor->nextThreadSP;
		}
		//Wake up this object/resource
		ptrThreadToSearchFor->ptrBlockStatus = OS_THREAD_BLOCKSTATUS_AWAKE;
	}else{}
	ENABLE__GLOBAL_INT();
}

/*****************************************************************************
 * Function: 	OS_Wait
 * Description:	Decrement semaphore and block if less than zero
 * Caveats:
 * Parameters:	pointer to a counting semaphore
 * Return:
 *****************************************************************************/
void OS_Wait(int16 *ptrSemaphore)
{
	DISABLE_GLOBAL_INT();
	*ptrSemaphore = *ptrSemaphore - 1;	//Decrement the semaphore by ONE count
	if((*ptrSemaphore) < 0 )
	{
		//If this semanphore is less than 0, the Thrad that made this call must get blocked.
		osExecTaskPtr->ptrBlockStatus = ptrSemaphore;
		ENABLE__GLOBAL_INT();
		//Suspend thread
		OS_Suspend();
	}else{}
	ENABLE__GLOBAL_INT();
}

/*****************************************************************************
 * Function: 	OS_Initialization
 * Description:
 * Caveats:
 * Parameters:
 * Return:
 *****************************************************************************/
void OS_Initialization(void)
{
	//
	// Step 1. Initialize System Control:
	// PLL, WatchDog, enable Peripheral Clocks
	// This example function is found in the F2806x_SysCtrl.c file.
	// CPU to 90MHz
	//
	InitSysCtrl();
	//
	// Step 3. Clear all interrupts and initialize PIE vector table:
	// Disable CPU interrupts
	//
	DINT;
	//
	// Initialize the PIE control registers to their default state.
	// The default state is all PIE interrupts disabled and flags are cleared.
	// This function is found in the F2806x_PieCtrl.c file.
	//
	InitPieCtrl();
	//
	// Disable CPU interrupts and clear all CPU interrupt flags:
	//
	IER = 0x0000;
	IFR = 0x0000;
	//
	// Initialize the PIE vector table with pointers to the shell Interrupt
	// Service Routines (ISR).
	// This will populate the entire table, even if the interrupt
	// is not used in this example.  This is useful for debug purposes.
	// The shell ISR routines are found in F2806x_DefaultIsr.c.
	// This function is found in F2806x_PieVect.c.
	//
	InitPieVectTable();
    #if GPIO13_FOR_DEBUGGING == ENABLE
	EALLOW;
	//
    // Make GPIO13 an output
    //
    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;    // GPIO13
    GpioCtrlRegs.GPADIR.bit.GPIO13 = 1;     // GPIO13 = output
    GpioDataRegs.GPASET.bit.GPIO13 = 1;     //Set GPIO13 to high//
	EDIS;
    #endif
}

/*****************************************************************************
 * Function: 	OS_u32_AddThreads
 * Description:	add foregound thread to the scheduler
 * Caveats:
 * Parameters:	pointers to a void/void foreground task
 *              ID of task/thread to be added to the scheduler
 *              ID of the next thread to be run after it
 * outputs:		1 if successful, 0 if this thread can not be added
 * Return:
 *****************************************************************************/
Uint32 OS_u32_AddThreads( void(*ptrfcn_Thread)(void), Uint16 ThreadID, Uint16 NextThreadID, Uint16 Priority)
{
    DISABLE_GLOBAL_INT();
    sOS_TaskCtrlBlocks[ThreadID].nextThreadSP       = &sOS_TaskCtrlBlocks[NextThreadID]; // 0 points to 1
    sOS_TaskCtrlBlocks[ThreadID].StaticPriority     = Priority;
    sOS_TaskCtrlBlocks[ThreadID].WorkingPriority    = Priority;
    sOS_TaskCtrlBlocks[ThreadID].ptrBlockStatus     = OS_THREAD_BLOCKSTATUS_AWAKE;
    sOS_TaskCtrlBlocks[ThreadID].ptrSleepTicks      = OS_SLEEPCOUNTER_RESET;
    sOS_TaskCtrlBlocks[ThreadID].Interrupted        = OS_THREAD_INT__CLEAR;
    if(ThreadID<OS_RealTime_MaxID)
    {
        sOS_TaskCtrlBlocks[ThreadID].Type           = OS_EVENT__THREAD;
    }else{
        sOS_TaskCtrlBlocks[ThreadID].Type           = OS_NORMAL_THREAD;
    }
    OS_fcn_SetInitialStack(ThreadID);
    Stacks[ThreadID][7] = (Uint32)(ptrfcn_Thread);  // PC
    Stacks[ThreadID][0] = (Uint32)(0x00000000);     // Top of the stack
    return 1;
}

/*****************************************************************************
 * Function:    OS_FirstThreadToRun
 * Description: Set first Thread to Run by scheduler
 * Caveats:
 * Parameters:  ID of the THREAD
 * outputs:
 * Return:
 *****************************************************************************/
void OS_FirstThreadToRun(Uint16 ThreadID)
{
    osExecTaskPtr = &sOS_TaskCtrlBlocks[ThreadID];       // thread 0 will run first
}

/*****************************************************************************
 * Function:    OS_AddPeriodicEventThread
 * Description: Add one background periodic event thread
 * Caveats:     Typically this function receives the highest priority
 *              It is assumed that the event threads will run to completion and return
 *              It is assumed the time to run these event threads is short compared to 1 msec
 *              These threads cannot spin, block, loop, sleep, or kill
 *              These threads can call OS_Signal
 * Parameters:  pointer to a void/void event thread function
 *              period given in units of OS_Launch
 * Return:      1 if successful, 0 if this thread cannot be added
 *****************************************************************************/
void OS_AddPeriodicEventThread(struct struct_PeriodicEvents *ptrfcn_EventBlock, void(*ptrfcn_Event)(void), Uint32 Period_ms)
{
    ptrfcn_EventBlock->OS_ptrfcn_Event = ptrfcn_Event;
    ptrfcn_EventBlock->Ticks = (Uint32)Period_ms;//(OS_SLEEP_TIMETICK__US*Period_ms);
}
//Uint32 OS_AddPeriodicEventThread(void(*ptrfcn_Event)(void), Uint32 Period_ms)
//{
//    sa_PeriodicEvents[PeriodicEventsCounter].ptrfcn_Event = ptrfcn_Event;
//    sa_PeriodicEvents[PeriodicEventsCounter].Ticks = (Uint32)(OS_SLEEP_TIMETICK__US*Period_ms);
//    PeriodicEventsCounter++;
//    return 1;
//}

/*****************************************************************************
 * Function: 	OS_Initialization
 * Description:
 * Caveats:
 * Parameters:
 * Return:
 *****************************************************************************/
void OS_Launch(Uint32 TimeSlice_us)
{
	//
	// Interrupts that are used in this example are re-mapped to
	// ISR functions found within this file.
	//
	EALLOW;  // This is needed to write to EALLOW protected registers
	#if RTOS_DEBUGG_CPUTIMER == ISR_ASM_FCN
		PieVectTable.TINT2 = &OS_SystemTick_Handler_isr;
	#endif
	#if RTOS_DEBUGG_CPUTIMER == ISR___C_FCN
		PieVectTable.TINT2 = &OS_Timing_isr;
	#endif
		PieVectTable.TINT1 = &OS_PeriodicEventsTick_Handler_isr;
	EDIS;    // This is needed to disable write to EALLOW protected registers
	//
	// Step 4. Initialize the Device Peripheral. This function can be
	//         found in F2806x_CpuTimers.c
	//
	InitCpuTimers();   // For this example, only initialize the Cpu Timers
	//
	// Configure CPU-Timer 0, 1, and 2 to interrupt every second:
	// 80MHz CPU Freq, 1 second Period (in uSeconds)
	//
	ConfigCpuTimer(&CpuTimer2, OS_SCHEDULER_FREQ, TimeSlice_us);
    #if AUX_HW_TIMER_CTRL == ENABLE
	ConfigCpuTimer(&CpuTimer1, OS_AUX_TIMER_FREQ, OS_EVENT_TIMETICK__US);
    #endif
	//
	// To ensure precise timing, use write-only instructions to write to the
	// entire register. Therefore, if any of the configuration bits are changed
	// in ConfigCpuTimer and InitCpuTimers (in F2806x_CpuTimers.h), the below
	// settings must also be updated.
	//
	//
	// Use write-only instruction to set TSS bit = 0
	//
	CpuTimer2Regs.TCR.all = 0x4000;
    #if AUX_HW_TIMER_CTRL == ENABLE
	CpuTimer1Regs.TCR.all = 0x4000;
    #endif
	//
	// Step 5. User specific code, enable interrupts
	//
	//
	// Enable CPU int 14, which is connected to CPU-Timer 2
	//
	IER |= M_INT14;         //Int14 ->  CPU Timer 2
    #if AUX_HW_TIMER_CTRL == ENABLE
	IER |= M_INT13;         //Int13 ->  CPU Timer 1
    #endif
    sm_Scheduler.ToRun = S0_scheduler_Normal;
    sm_Scheduler.NoEventsTriggered = 0;
	OS_Start();
}

//*****************************************************************************
//
//			PRIVATE FUNCTIONS SECTION
//
//*****************************************************************************
/*****************************************************************************
 * Function: 	OS_Scheduler
 * Description:
 * Caveats:
 * Parameters:
 * Return:
 *****************************************************************************/
void OS_Scheduler(void)
{
    // look at all threads in TCB list choose
    // highest priority thread not blocked and not sleeping
    // If there are multiple highest priority (not blocked, not sleeping) run these round robin
    Uint16 PriorityToRun;
    Uint16 Counter;
    static s_TaskCtrlBlock *ptrSearchThread;
    static s_TaskCtrlBlock *ptrThreadtToRun;

    PriorityToRun   = OS_PRIOROTY_MINIMUM_LEVEL;
    ptrSearchThread = osExecTaskPtr;

        Counter         = (OS_Global_Thread_IDmax);
        while( Counter-- )
        {
            //point to the "theorical" next thread to run
            ptrSearchThread = ptrSearchThread->nextThreadSP;
            //Ask if the "theorical next" has the following 3 status:
            //  - Has a lower priority than: PriorityToRun
            //  - Has NOT blocked status
            //  - Has NOT a Sleep status
            if(  (ptrSearchThread->WorkingPriority < PriorityToRun) &&
                 (ptrSearchThread->ptrBlockStatus == 0)             &&
                 (ptrSearchThread->ptrSleepTicks  == 0) )
            {
                //Described the three conditions To Move to next thread IF:
                //  - Block status has a pointer value "point to semaphore"
                //  - Sleep counter has a value than 0 "means is in sleep mode"
                //  - (condition) if thread priority is lower than: PriorityToRun var.
                PriorityToRun = ptrSearchThread->WorkingPriority;
                ptrThreadtToRun = ptrSearchThread;
            }else{}
        }// Loop to look in all threads

    // if loop has reach the thread that was running prior to run schedules, search is done.
    osExecTaskPtr = (s_TaskCtrlBlock *)ptrThreadtToRun;
    //osExecTaskPtr = osExecTaskPtr->nextThreadSP;
}

/*****************************************************************************
 * Function:    OS_EventsPeriodicTick_Handler_isr
 * Description: Set Semaphore belong to each Periodic Event thread when its tie has came-up
 * Caveats:
 * Parameters:
 * Return:
 *****************************************************************************/
__interrupt void OS_PeriodicEventsTick_Handler_isr(void)
{
    Uint16 IDcounter=OS_RealTime_MaxID;
    s_TaskCtrlBlock *ptrSearchThread;
    DISABLE_GLOBAL_INT();
    #if GPIO13_FOR_DEBUGGING == ENABLE
    EALLOW;
    GpioDataRegs.GPATOGGLE.bit.GPIO13 = 1;
    EDIS;
    #endif
    PeriodicTicksCounter = ( PeriodicTicksCounter + 1 );
    if(PeriodicTicksCounter)
    {
        IDcounter=OS_RealTime_MaxID;
        while(IDcounter--)
        {
            if((PeriodicTicksCounter % sa_PeriodicEvents[IDcounter].Ticks) == 0)
            {
                sa_PeriodicEvents[IDcounter].OS_ptrfcn_Event();
            }else{}
        }

        ptrSearchThread = osExecTaskPtr;
        IDcounter = OS_Global_Thread_IDmax;
        while(IDcounter--)
        {
            if(ptrSearchThread->ptrSleepTicks != 0)
            {
                ptrSearchThread->ptrSleepTicks--;
            }else{}
            ptrSearchThread = ptrSearchThread->nextThreadSP;
        }
    }else{      }
    ENABLE__GLOBAL_INT();
}

/*****************************************************************************
 * Function: 	InitClocks
 * Description:
 * Caveats:
 * Parameters:
 * Return:
 *****************************************************************************/
void OS_fcn_SetInitialStack(Uint32 i)
{
	//***YOU IMPLEMENT THIS FUNCTION*****
	sOS_TaskCtrlBlocks[i].ThreadSP = &Stacks[i][17];
	Stacks[i][ 1 ] = 0x00010088;		//MSB (T)       : LSB(ST0)
	Stacks[i][ 2 ] = 0x00030004;		//MSB (AH)      : LSB(AL)
	Stacks[i][ 3 ] = 0x00050006;		//MSB (PH)      : LSB(PL)
	Stacks[i][ 4 ] = 0x00070008;		//MSB (AR1)     : LSB(AR0)
	Stacks[i][ 5 ] = 0x00096A4A;		//MSB (SP)      : LSB(ST1)
	Stacks[i][ 6 ] = 0x00003000;		//MSB (DBGSTAT) : LSB(IER)  -> 3000 (Measn INT14 and INT13 enable)
	Stacks[i][ 7 ] = 0xFFFFFFFFF;       //PC location (22bit)
	Stacks[i][ 8 ] = 0xFFFFFFFFF;       //RPC location (22bit)
	Stacks[i][ 9 ]  = 0x000A000B;		//MSB (AR1H)    : LSB(AR0H)
	Stacks[i][ 10 ] = 0x0000000C;		//XAR2
	Stacks[i][ 11 ] = 0x0000000E;		//XAR3
	Stacks[i][ 12 ] = 0x0000000F;		//XAR4
	Stacks[i][ 13 ] = 0x00000010;		//XAR5
	Stacks[i][ 14 ] = 0x00000011;		//XAR6
	Stacks[i][ 15 ] = 0x00000012;		//XAR7
	Stacks[i][ 16 ] = 0x00130000;		//XT

	OC_SetStackContenToZero(sOS_TaskCtrlBlocks[i].ThreadSP,STACKSIZE-17);
	//R0 to R3 are automaticly filled by the ARM CPU
}

/*****************************************************************************
 * Function:    OS_Suspend
 * Description: Called by main thread to cooperatively suspend operation
 * Caveats:     Will be run again depending on sleep/block status
 * Parameters:
 * Return:
 *****************************************************************************/
void OS_Suspend(void)
{
    #if RST_SLICE_T_AT_SUSPEND == ENABLE
	CpuTimer2Regs.TCR.bit.TRB=1;		//REload CPU-Timer 2 counter
	EALLOW;
    #endif
	__asm(" INTR	INT14");            //__asm(" OR    IFR,#0x2000"); //Alternative fcn
}


/*****************************************************************************
 * Function:    OS_Sleep
 * Description: place this thread into a dormant state
 * Caveats:     implements cooperative multitasking
 * Parameters:  number of msec to sleep
 * Return:
 *****************************************************************************/
void OS_Sleep(Uint32 osTasksleepTime)
{
    // Load the Number of tick to spleep into the Thread block information
    osExecTaskPtr->ptrSleepTicks = osTasksleepTime;
    OS_Suspend();
}

/*****************************************************************************
 * Function:    OC_SetStackContenToZero
 * Description: preset stack value to Zero, except the initial CPU context
 * Caveats:
 * Parameters:  Pointer to stack address after CPU context and size of stack (Stacksize - CPU context)
 * Return:
 *****************************************************************************/
void OC_SetStackContenToZero(Uint32 *ptrStack, Uint32 Size)
{
	while(Size--)
	{
		*ptrStack = 0x00000000;
		ptrStack++;
	}
}


#if RTOS_DEBUGG_CPUTIMER == ISR___C_FCN
__interrupt void
OS_Timing_isr(void)
{
    EALLOW;
    CpuTimer2.InterruptCount++;
    fcn_Toogle_LED_D9();
    //
    // The CPU acknowledges the interrupt.
    //
    EDIS;
}
#endif




