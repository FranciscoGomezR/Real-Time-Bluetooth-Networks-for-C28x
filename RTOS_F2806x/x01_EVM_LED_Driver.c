/*
 * x01_EVM_LED_Driver.c
 *
 *  Created on: Mar 22, 2020
 *      Author: JuanFco
 */

//*****************************************************************************
//
//			INCLUDE FILE SECTION FOR THIS MODULE
//
//*****************************************************************************
#include "x01_EVM_LED_Driver.h"
#include "x00_RTOS.h"
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

//*****************************************************************************
//
//			PRIVATE DEFINES SECTION - OWN BY THIS MODULE ONLY
//
//*****************************************************************************
int16 appSemaphore_Event1;
int16 appSemaphore_Event2;
int16 appSemaphore_Event3;
//*****************************************************************************
//
//			PRIVATE STRUCTs, UNIONs ADN ENUMs SECTION
//
//*****************************************************************************


//*****************************************************************************
//
//			PUBLIC VARIABLES
//
//****************************************************************************


//*****************************************************************************
//
//			PRIVATE VARIABLES
//
//*****************************************************************************


//*****************************************************************************
//
//			PRIVATE FUNCTIONS PROTOYPES
//
//*****************************************************************************
void fcn_DelayTime(volatile Uint32 DelayT);
void fcn_ToggleGPIO33(void);
void fcn_ToggleGPIO32(void);
void fcn_Func1(void);
void fcn_Func2(void);
void fcn_Func3(void);
void fcn_Func4(void);

void fcn_InitEvents(void);
void fcn_Event0ToggleGPIO50_J2_13(void);
void fcn_Event1ToggleGPIO51_J2_12(void);
void fcn_Event1ToggleGPIO55_J2_11(void);

//*****************************************************************************
//
//			PUBLIC FUNCTIONS SECTION
//
//*****************************************************************************
/*****************************************************************************
 * Function: 	fcn_InitThreadsGPIO
 * Description:
 * Caveats:
 * Parameters:
 * Return:
 *****************************************************************************/
 void fcn_InitThreadsGPIO(void)
 {
	 EALLOW;
	//
	// Make GPIO34 an output
	//
	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0; // GPIO34 = GPIO34
	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;  // GPIO34 = output
	GpioDataRegs.GPBSET.bit.GPIO34 = 1;	//Set GPIO34 to high

	//
	// Make GPIO39 an output
	//
	GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 0; // GPIO39 = GPIO39
	GpioCtrlRegs.GPBDIR.bit.GPIO39 = 1;  // GPIO39 = output
	GpioDataRegs.GPBSET.bit.GPIO39 = 1;	//Set GPIO34 to high
	//
	// Make GPIO12 an output
	//
	GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0; // GPIO34 = GPIO34
	GpioCtrlRegs.GPADIR.bit.GPIO12 = 1;  // GPIO34 = output
	GpioDataRegs.GPASET.bit.GPIO12 = 1;	//Set GPIO34 to high//
	// Make GPIO18 an output
	//
	GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 0; // GPIO34 = GPIO34
	GpioCtrlRegs.GPADIR.bit.GPIO18 = 1;  // GPIO34 = output
	GpioDataRegs.GPASET.bit.GPIO18 = 1;	//Set GPIO34 to high

	//
	// Make GPIO22 an output
	//
	GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO22 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO22 = 1;
	//
    // Make GPIO33 an input
    //
    GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO33 = 1;
    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;
    //
    // Make GPIO32 an input
    //
    GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO32 = 1;
    GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1;
	EDIS;
	fcn_InitEvents();
 }

 /*****************************************************************************
  * Function: 	fcn_Thread0ToogleGPIO34_D9_J1_5
  * Description:
  * Caveats:
  * Parameters:
  * Return:
  *****************************************************************************/
 void fcn_Thread0ToogleGPIO34_D9_J1_5(void)
 {
     static Uint32 CounterTask[32];
     static Uint32 x;
     while(1)
     {
         EALLOW;
         GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1; //toggle LED9
         GpioDataRegs.GPATOGGLE.bit.GPIO12 = 1; //toggle GPIO12
         EDIS;
         for(x=0;x<31;x++)
         {
        	 CounterTask[x]+=1;
         }
         //DelayT=100000;
         //while(DelayT--);
     }

 }

 /*****************************************************************************
  * Function: 	fcn_Thread0ToogleGPIO39_D10_J1_7
  * Description:
  * Caveats:
  * Parameters:
  * Return:
  *****************************************************************************/
 void fcn_Thread1ToogleGPIO39_D10_J1_7(void)
 {
     static volatile Uint32 CounterTask2a,CounterTask2b,CounterTask2c;
     static Uint32 DelayT;
     CounterTask2a=0;
     CounterTask2b=0;
     CounterTask2b=0;
     while(1)
     {
         EALLOW;
         GpioDataRegs.GPBTOGGLE.bit.GPIO39 = 1; //toggle
         GpioDataRegs.GPATOGGLE.bit.GPIO18 = 1; //toggle GPIO18
         EDIS;
         CounterTask2a++;
         CounterTask2b+=2;
         CounterTask2c+=3;
         //DelayT=200000;
         //while(DelayT--);

     }
 }

 /*****************************************************************************
  * Function: 	fcn_Thread2ToogleGPIO22_J1_8
  * Description:
  * Caveats:
  * Parameters:
  * Return:
  *****************************************************************************/
 void fcn_Thread2ToogleGPIO22_J1_8(void)
 {
     Uint32 CounterTask3;
     static Uint32 DelayT;
     CounterTask3=0;
     while(1)
     {
         EALLOW;
         GpioDataRegs.GPATOGGLE.bit.GPIO22 = 1; //toggle
         EDIS;
         CounterTask3++;
         //DelayT=500000;
         //while(DelayT--);
     }
 }

#define DEBUGG_OS_MODE	0
 /*****************************************************************************
  * Function:   fcn_Thread3ToogleGPIO33_J1_9
  * Description:
  * Caveats:
  * Parameters:
  * Return:
  *****************************************************************************/
 void fcn_Thread3ToogleGPIO33_J1_9(void)
 {
    while(1)
    {
		#if DEBUGG_OS_MODE == 1
    	EALLOW;
		GpioDataRegs.GPBTOGGLE.bit.GPIO33 = 1; //toggle
		EDIS;
		#endif
		#if DEBUGG_OS_MODE != 1
    	fcn_ToggleGPIO33();
		#endif
    }
 }

 /*****************************************************************************
  * Function:   fcn_Thread4ToogleGPIO32_J1_10
  * Description:
  * Caveats:
  * Parameters:
  * Return:
  *****************************************************************************/
 void fcn_Thread4ToogleGPIO32_J1_10(void)
 {
     while(1)
     {
		#if DEBUGG_OS_MODE == 1
		EALLOW;
		GpioDataRegs.GPBTOGGLE.bit.GPIO32 = 1; //toggle
		EDIS;
		#endif
		#if DEBUGG_OS_MODE != 1
    	 fcn_ToggleGPIO32();
    	 OS_Suspend();
		#endif
     }
 }
//*****************************************************************************
//
//			PRIVATE FUNCTIONS SECTION
//
//*****************************************************************************

/*****************************************************************************
 * Function: 	InitClocks
 * Description:
 * Caveats:
 * Parameters:
 * Return:
 *****************************************************************************/
 void fcn_ToggleGPIO33(void)
 {
	DISABLE_GLOBAL_INT();
	EALLOW;
	GpioDataRegs.GPBTOGGLE.bit.GPIO33 = 1; //toggle
	EDIS;
	ENABLE__GLOBAL_INT();
 }

 void fcn_ToggleGPIO32(void)
 {
	DISABLE_GLOBAL_INT();
	EALLOW;
	GpioDataRegs.GPBTOGGLE.bit.GPIO32 = 1; //toggle
	EDIS;
	ENABLE__GLOBAL_INT();
	fcn_Func1();

 }

 void fcn_Func1(void)
 {
	 static Uint32 vfcn1=0;
	 vfcn1++;
	 fcn_Func2();
 }
 void fcn_Func2(void)
 {
	 static Uint32 vfcn2=0;
	 vfcn2++;
	 fcn_Func3();
 }
 void fcn_Func3(void)
 {
	 static Uint32 vfcn3=0;
	 vfcn3++;
	 fcn_Func4();
 }
 void fcn_Func4(void)
 {
	 static Uint32 vfcn4=0;
	 vfcn4++;
 }


 void fcn_InitEvents(void)
 {
     EALLOW;
     //
     // Make GPIO50 an input
     //
     GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 0;
     GpioCtrlRegs.GPBDIR.bit.GPIO50 = 1;
     GpioDataRegs.GPBCLEAR.bit.GPIO50 = 1;
     //
     // Make GPIO51 an input
     //
     GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 0;
     GpioCtrlRegs.GPBDIR.bit.GPIO51 = 1;
     GpioDataRegs.GPBCLEAR.bit.GPIO51 = 1;
     //
     // Make GPIO52 an input
     //
     GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 0;
     GpioCtrlRegs.GPBDIR.bit.GPIO55 = 1;
     GpioDataRegs.GPBCLEAR.bit.GPIO55 = 1;
     EDIS;
 }
 void fcn_Event0ToggleGPIO50_J2_13(void)
 {
    static Uint32 event1=0;
    while(1)
    {
        OS_Wait(&appSemaphore_Event1);
        DISABLE_GLOBAL_INT();
        event1++;
        EALLOW;
        GpioDataRegs.GPBTOGGLE.bit.GPIO50 = 1; //toggle
        EDIS;
        ENABLE__GLOBAL_INT();
    }
 }
 void fcn_Event1ToggleGPIO51_J2_12(void)
 {
     static Uint32 event2=0;
     while(1)
     {
         OS_Wait(&appSemaphore_Event2);
         DISABLE_GLOBAL_INT();
         event2++;
         EALLOW;
         GpioDataRegs.GPBTOGGLE.bit.GPIO51 = 1; //toggle
         EDIS;
         ENABLE__GLOBAL_INT();
     }
 }
 void fcn_Event2ToggleGPIO55_J2_11(void)
 {
     static Uint32 event3=0;
     while(1)
     {
         OS_Wait(&appSemaphore_Event3);
         DISABLE_GLOBAL_INT();
         event3++;
         EALLOW;
         GpioDataRegs.GPBTOGGLE.bit.GPIO55 = 1; //toggle
         EDIS;
         ENABLE__GLOBAL_INT();
     }
 }


