;	Created on: Mar 21, 2020
;	 Author: JuanFco
;************************************************************************************
;   Copyright                                                                       *
;   All Rights Reserved                                                             *
;   The Copyright symbol does not also indicate public availability or publication. *
;                                                                                   *
;                           "Francisco Gomez"                                       *
;                                                                                   *
; - Driver:             "RTOS's Scheduler".                                  		*
;                                                                                   *
; - Compiler:           Code Composer Studio (Licensed)                             *
; - Version:            9.3.0.00012                                                 *
; - Supported devices:  "F28xxxx"                                                   *
;                                                                                   *
; - AppNote:            This code is base on the following course:                  *
; https://courses.edx.org/courses/course-v1:UTAustinX+UT.RTBN.12.01x+2T2018/course/ *
;                                                                                   *
; - Created by:         "Francisco Gomez"                                           *
; - Date Created:       "21/03/2020"                                                *
; - Contact:            "juan.fco.gomez.ruiz@gmail.com"                             *
;                                                                                   *
; - Description:        _OS_Start function load first thread's context into			*
;						CPU and  dispatch it        								*
;						_OS_SystemTick_Handler_isr function saves thread's context	*
;						launch the scheduler and restore new thread context into	*
;						the CPU														*
;                       -   -                                                       *
; - Repository:                                                                     *
;   https://Francisco_GomezR@bitbucket.org/Francisco_GomezR/rtos_for_c28x.git       *
;                                                                               2012*
;************************************************************************************

;************************************************************************************
;   Revision History:
;
;   Date            CP#           Author
;   DD-MM-YYYY      XXXXX:1     Initials    Description of change
;   -----------   ------------  ---------   ------------------------------------
;   21-03-2020      1.0         JFGR        Schev
;
;************************************************************************************
;
; File/
;
;  "More detail description of the code"
;
;  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
;  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
;  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
;  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
;  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
;  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
;  DEALINGS IN THE SOFTWARE.

	.def _OS_Start
	.def _OS_SystemTick_Handler_isr

    .global _OS_Start
	.global _OS_SystemTick_Handler_isr
	.ref _OS_Scheduler
    .ref _osExecTaskPtr

;****************************************************************************
; 	Function: 		_OS_Start
;   Description:	load first thread's context into CPU registers and  dispatch it
;   Caveats:		never returns
;   Parameters:
;   Return:
;****************************************************************************
    .text
_OS_Start:
	.asmfunc
	.c28_amode
	C28ADDR ; Set AMODE to 0 C28x addressing
	EALLOW

	MOVL	XAR2,#_osExecTaskPtr;XAR0 points to osExecTaskPtr variable.
	MOVZ 	AR3,*AR2			;Accumulator register points to ThreadSP pointer
	MOVZ 	AR4,*AR3			;Get value (Address of Stack pointer) inside ThreadSP pointer
	MOV		SP,AR4				;Set SP register with the value contained in ThreadSP
	;POP/restore, NON-automaticly saved register from the CPU context belong  the First Thread to be dispatch
	POP 	XT
	POP 	XAR7
	POP 	XAR6
	POP 	XAR5
	POP 	XAR4
	POP 	XAR3
	POP 	XAR2
	POP 	AR1H:AR0H
	;Skip PC register
	SUB		SP,#2
	POP		RPC					;Restore the PC addres od the Thread to the RPC regiters
	;POP/restore, automaticly saved register from the CPU context belong to the First Thread to be dispatch
	;The reason is beacuse LRETR instruction doe snot POP this register automaticaly
	POP		DBGIER
	POP		IER
	POP		DP
	POP		ST1					;0x6A4A
	POP		AR1
	POP		AR0
	POP		PH
	POP		PL
	POP		AH
	POP		AL
	POP		T
	POP		ST0
	ASP
	ADDB	SP,#2				;Align Stack pointer to the TOP of he stack
	CLRC 	DBGM
	EDIS						;Disable Write Access to Protected Registers
	EINT
	.lp_amode 					;Change back the assembler mode to C2xlp.
	SPM		#0
	LRETR						;return from funtio which use the RPC address
	.endasmfunc
;end OS_Start


;****************************************************************************
; 	Function: 		_OS_SystemTick_Handler_isr
;   Description:	Saves thread's context never returns launch the scheduler
;					and restore new thread context into CPU registers
;   Caveats:		This function calls the scheduler that its in C code
;   Parameters:
;   Return:
;****************************************************************************
_OS_SystemTick_Handler_isr:
	.asmfunc
	.c28_amode
	;DINT							;Maskable Interrupts are automatically disabled when an interrupt service routine begins
	C28ADDR 						; Set AMODE to 0 C28x addressing
	EALLOW							;Enable Write Access to Protected Space
	ASP								;Align Stack Pointer
	;ESTOP0							;Breakpoint enforce by ASM code
	SUBB	SP,#2					;Alig to Stack strcuture
	PUSH	RPC
	PUSH 	AR1H:AR0H 				; 32-bit
	PUSH 	XAR2 					; 32-bit
	PUSH 	XAR3 					; 32-bit
	PUSH 	XAR4 					; 32-bit
	PUSH 	XAR5 					; 32-bit
	PUSH 	XAR6 					; 32-bit
	PUSH 	XAR7 					; 32-bit
	PUSH 	XT 						; 32-bit

	;Store SP into Thread control block before move next task's stackPointer
	MOVL	XAR2,#_osExecTaskPtr	;XAR2 points to osExecTaskPtr variable.
	MOVZ 	AR3,*AR2				;Accumulator register points to ThreadSP pointer
	MOV		ACC,@SP					;Points ACC register/pointer to the SP
	MOVL	*XAR3,ACC				;Store SP address into ThreadSP in side of _osExecTaskPtr poiner

	PUSH	XAR2					;push XAR2 ( osExecTaskPtr variable) to the stack
	LCR		#_OS_Scheduler			;call schdeuler function
	POP		XAR2					;restore  XAR2 ( osExecTaskPtr point to the new Thread, selected by scheduler) from the stack

	MOVZ 	AR3,*AR2				;AR3 register points to ThreadSP pointer
	MOVZ 	AR2,*AR3				;Get value (Address of Stack pointer) inside ThreadSP pointer
	MOV		SP,AR2					;Move new SP to the SP regster
	;Restore New Thread context to the CPU
	POP 	XT
	POP 	XAR7
	POP 	XAR6
	POP 	XAR5
	POP 	XAR4
	POP 	XAR3
	POP 	XAR2
	POP 	AR1H:AR0H
	POP		RPC
	ADDB	SP,#2					;Align stack, so the automatic operation of the CPU restore the other registers values

	CLRC 	DBGM					;Debug events are enabled
	EINT
	EDIS							;Disable Write Access to Protected Registers
	.lp_amode 						;Change back the assembler mode to C2xlp.
	NASP							;—Unalign Stack Pointer
	IRET							;Return form ISR function which indicates 8 register will pop automaticaly
	.endasmfunc
;//
;// End of file.
;//
