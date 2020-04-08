/*
 * x00_RTOS.h
 *
 *  Created on: Mar 21, 2020
 *      Author: JuanFco
 */
/************************************************************************************
*   Copyright                                                                       *
*   All Rights Reserved                                                             *
*   The Copyright symbol does not also indicate public availability or publication. *
*                                                                                   *
*                           "Francisco Gomez"                                       *
*                                                                                   *
* - Driver:             "RTOS with Event Threads".                                  *
*                                                                                   *
* - Compiler:           Code Composer Studio (Licensed)                             *
* - Version:            9.3.0.00012                                                 *
* - Supported devices:  "F28xxxx"                                                   *
*                                                                                   *
* - AppNote:            This code is base on the following course:                  *
* https://courses.edx.org/courses/course-v1:UTAustinX+UT.RTBN.12.01x+2T2018/course/ *
*                                                                                   *
* - Created by:         "Francisco Gomez"                                           *
* - Date Created:       "02/04/2020"                                                *
* - Contact:            "juan.fco.gomez.ruiz@gmail.com"                             *
*                                                                                   *
* - Description:        Scheduler is a Round-Robin with Priority level, blocking    *
*                       and sleeping features. Include Periodic Event-threads       *
*                       Handler.                                                    *
*                       I know TI already provides you a RTOS, more reliable and    *
*                       and robust.                                                 *
*                       This RTOS is base on he coursera-course:                    *
*                       Real-Time Bluetooth Networks - Shape the World              *
*                       UTAustinX: UT.RTBN.12.01x                                   *
*                       (that RTOS is implemented for ARM types)                    *
*                       the intention of this code is to implement that knowledge   *
*                       on C28x processor in order to share it, make it robust,     *
*                       reliable and little-bit more complex through                *
*                       your collaboration                                          *
*                       -   -                                                       *
* - Repository:                                                                     *
*   https://Francisco_GomezR@bitbucket.org/Francisco_GomezR/rtos_for_c28x.git       *
*                                                                               2012*
*************************************************************************************

*************************************************************************************
*   Revision History:
*
*   Date            CP#           Author
*   DD-MM-YYYY      XXXXX:1     Initials    Description of change
*   -----------   ------------  ---------   ------------------------------------
*   02-04-2020      1.0         JFGR        Initial Code (No bug detected, fairly tested)
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
//
// Included Files
//
#include "x00_RTOS.h"
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "x01_EVM_LED_Driver.h"
//
// Main
//
void main(void)
{
    //Initiate C28x clock at 90MHz
	OS_Initialization();
	//Set GPIO to be use by Threads as outputs
	fcn_InitThreadsGPIO();
    //Set Normal-Threads into the scheduler in Round-Robin with priority
	OS_u32_AddThreads(&fcn_Thread0ToogleGPIO34_D9_J1_5,     OS_Standard_Thread_ID0,OS_Standard_Thread_ID1,  3);
	OS_u32_AddThreads(&fcn_Thread1ToogleGPIO39_D10_J1_7,    OS_Standard_Thread_ID1,OS_Standard_Thread_ID2,  3);
	OS_u32_AddThreads(&fcn_Thread2ToogleGPIO22_J1_8,        OS_Standard_Thread_ID2,OS_Standard_Thread_ID3,  3);
	OS_u32_AddThreads(&fcn_Thread3ToogleGPIO33_J1_9,        OS_Standard_Thread_ID3,OS_Standard_Thread_ID4,  3);
	OS_u32_AddThreads(&fcn_Thread4ToogleGPIO32_J1_10,       OS_Standard_Thread_ID4,OS_Standard_Thread_ID0,  3);
	//Set which Thread will run First
	OS_FirstThreadToRun(OS_RealTime_Thread_ID0);

	//Init. ans set Ticks for each periodic event-thread
	OS_AddPeriodicEventThread((s_PeriodicEvent *)&sa_PeriodicEvents[OS_RealTime_Thread_ID0],
	                          fcn_Event0ToggleGPIO50_J2_13,
	                          13);                  //in mili-seconds
	OS_AddPeriodicEventThread((s_PeriodicEvent *)&sa_PeriodicEvents[OS_RealTime_Thread_ID1],
	                          fcn_Event1ToggleGPIO51_J2_12,
	                          50);                  //in mili-seconds
	OS_AddPeriodicEventThread((s_PeriodicEvent *)&sa_PeriodicEvents[OS_RealTime_Thread_ID2],
	                          fcn_Event2ToggleGPIO55_J2_11,
	                          123);                 //in mili-seconds
	//Launch OS and dispatch first Thread
	OS_Launch(OS_TASKS_TIMESLICE_US);
    return 0;
}
//
// End of File
//

