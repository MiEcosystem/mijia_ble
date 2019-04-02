;
;  Copyright (c) 2013, Texas Instruments Incorporated
;  All rights reserved.
;
;  Redistribution and use in source and binary forms, with or without
;  modification, are permitted provided that the following conditions
;  are met:
;
;  *  Redistributions of source code must retain the above copyright
;     notice, this list of conditions and the following disclaimer.
;
;  *  Redistributions in binary form must reproduce the above copyright
;     notice, this list of conditions and the following disclaimer in the
;     documentation and/or other materials provided with the distribution.
;
;  *  Neither the name of Texas Instruments Incorporated nor the names of
;     its contributors may be used to endorse or promote products derived
;     from this software without specific prior written permission.
;
;  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
;  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
;  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
;  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
;  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
;  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
;  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
;  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
;  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
;  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
;
; ======== TaskSupport_asm_iar.s ========
;
;
        PRESERVE8

        PUBLIC ti_sysbios_family_arm_m3_TaskSupport_swap__E__mangled__
        PUBLIC ti_sysbios_family_arm_m3_TaskSupport_buildTaskStack__mangled__
        PUBLIC ti_sysbios_family_arm_m3_TaskSupport_glue__mangled__
        PUBLIC ti_sysbios_knl_Task_swapReturn__mangled__

;
;  ======== Task_buildTaskStack ========
;
;  Task_buildTaskStack(stack, fxn, exit, enter, arg0, arg1)
;                         r0,  r1,   r2,    r3,  [0],  [4]
;
;  initial stack image:
;
;               d8-d15 = 0      if vfp supported
;               r4 = -1
;               r5 = -1
;               r6 = -1
;               r7 = -1
;               r8 = -1
;               r9 = -1
;               r10 = -1
;               r11 = -1
;               glue
;               arg0
;               arg1
;               fxn
;               exit
;               enter
;

; 1st argument, return value
#define STACK     r0
; 2nd argument
#define FXN       r1
; 3rd argument
#define EXIT      r2
; 4th argument
#define ENTER     r3
ARG0    DEFINE    0       ; 5th argument (stack offset)
ARG1    DEFINE    4       ; 6th argument (stack offset)

        SECTION CODE:CODE:NOROOT(2)
        THUMB

ti_sysbios_family_arm_m3_TaskSupport_buildTaskStack__mangled__:

        str     ENTER, [STACK,#-4]!
        str     EXIT, [STACK,#-4]!
        str     FXN, [STACK,#-4]!

        ldr     r1, [sp, #ARG1]
        str     r1, [STACK,#-4]!
        ldr     r1, [sp, #ARG0]
        str     r1, [STACK,#-4]!

        ldr     r1, glueAddr
        str     r1, [STACK,#-4]!

        eor     r1, r1, r1
        sub     r1, r1, #1

        str     r1, [STACK,#-4]!        ; init r4-r11, lr with -1
        str     r1, [STACK,#-4]!
        str     r1, [STACK,#-4]!
        str     r1, [STACK,#-4]!
        str     r1, [STACK,#-4]!
        str     r1, [STACK,#-4]!
        str     r1, [STACK,#-4]!
        str     r1, [STACK,#-4]!

#if defined(__ARMVFP__)
        str     r1, [STACK,#-4]!
        str     r1, [STACK,#-4]!        ; make room for 8 vfp
        str     r1, [STACK,#-4]!        ; saved by parent regs
        str     r1, [STACK,#-4]!        ; d8-d15
        str     r1, [STACK,#-4]!
        str     r1, [STACK,#-4]!
        str     r1, [STACK,#-4]!
        str     r1, [STACK,#-4]!

        str     r1, [STACK,#-4]!
        str     r1, [STACK,#-4]!
        str     r1, [STACK,#-4]!
        str     r1, [STACK,#-4]!
        str     r1, [STACK,#-4]!
        str     r1, [STACK,#-4]!
        str     r1, [STACK,#-4]!
        str     r1, [STACK,#-4]!
#endif
        bx      lr

        DATA
glueAddr:
        DC32  ti_sysbios_family_arm_m3_TaskSupport_glue__mangled__

;
;  ======== TaskSupport_swap ========
;
;  TaskSupport_swap(oldSP, newSP)
;

; 1st argument
#define OLD      r0
; 2nd argument
#define NEW      r1

        SECTION CODE:CODE:NOROOT
        THUMB
ti_sysbios_family_arm_m3_TaskSupport_swap__E__mangled__:

        push    {r4-r11, lr}
#if defined(__ARMVFP__)
        vpush   {d8-d15}
#endif
        str     sp, [OLD]

ti_sysbios_knl_Task_swapReturn__mangled__:
        ldr     sp, [NEW]
#if defined(__ARMVFP__)
        vpop    {d8-d15}                ; pop vfp caller regs
#endif
        pop     {r4-r11, pc}

;
;  ======== TaskSupport_glue ========
;
;  This glue function is executed immediately before executing the "fxn"
;  passed to Task_start.  This function loads the parameters to "fxn" into
;  registers - necessary for the m3 register passing model.
;
        SECTION CODE:CODE:NOROOT
        THUMB

ti_sysbios_family_arm_m3_TaskSupport_glue__mangled__:

        ldr     r0, [sp, #16]   ; Task enter function
        blx     r0              ; call Task enter function

; return from Task enter function here
        ldr     r0, [sp, #0]    ; task arg0
        ldr     r1, [sp, #4]    ; task arg1
        ldr     r2, [sp, #8]    ; task fxn

        ldr     lr, [sp, #12]   ; Task exit function
        bx      r2              ; call task fxn,  ("returns" to exit func)

        END
