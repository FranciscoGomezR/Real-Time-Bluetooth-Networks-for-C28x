/*
 * x01_EVM_LED_Driver.h
 *
 *  Created on: Mar 22, 2020
 *      Author: JuanFco
 */

#ifndef X01_EVM_LED_DRIVER_H_
#define X01_EVM_LED_DRIVER_H_
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
* - Date Created:       "22/03/2020"                                                *
* - Contact:            "juan.fco.gomez.ruiz@gmail.com"                             *
*                                                                                   *
* - Description:        Simple function hat init. GPOS and toggle them              *
*                       work as Threads to be handle by the Scheduler               *
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
*   22-03-2020      1.0         JFGR        Simple function that init. GPIOS and toggle them
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
#include "F2806x_Cla_typedefs.h"// F2806x CLA Type definitions
#include "F2806x_Device.h"      // F2806x Headerfile Include File

//*****************************************************************************
//
//			PUBLIC DEFINES SECTION
//
//*****************************************************************************

//*****************************************************************************
//
//			PUBLIC STRUCTs, UNIONs ADN ENUMs SECTION
//
//*****************************************************************************

//*****************************************************************************
//
//			PUBLIC VARIABLES PROTOTYPE
//
//*****************************************************************************
extern Uint32 CounterTask1;
extern Uint32 CounterTask2;
extern Uint32 CounterTask3;

extern int16 appSemaphore_Event1;
extern int16 appSemaphore_Event2;
extern int16 appSemaphore_Event3;

//*****************************************************************************
//
//			PUBLIC FUNCTIONS PROTOYPES
//
//*****************************************************************************
void fcn_InitThreadsGPIO(void);

void fcn_Thread0ToogleGPIO34_D9_J1_5(void);
void fcn_Thread1ToogleGPIO39_D10_J1_7(void);
void fcn_Thread2ToogleGPIO22_J1_8(void);
void fcn_Thread3ToogleGPIO33_J1_9(void);
void fcn_Thread4ToogleGPIO32_J1_10(void);

void fcn_Event0ToggleGPIO50_J2_13(void);
void fcn_Event1ToggleGPIO51_J2_12(void);
void fcn_Event2ToggleGPIO55_J2_11(void);


#endif /* X01_EVM_LED_DRIVER_H_ */
