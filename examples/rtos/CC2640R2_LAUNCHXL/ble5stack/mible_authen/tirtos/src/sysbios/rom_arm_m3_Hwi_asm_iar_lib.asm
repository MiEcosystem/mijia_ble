;
;  Copyright (c) 2013-2017, Texas Instruments Incorporated
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
; ======== Hwi_asm_iar.s ========
;
;

        PRESERVE8

        PUBLIC ti_sysbios_family_arm_m3_Hwi_dispatch__I__mangled__
        PUBLIC ti_sysbios_family_arm_m3_Hwi_pendSV__I__mangled__
        PUBLIC ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I__mangled__
        PUBLIC ti_sysbios_family_arm_m3_Hwi_return__mangled__
        PUBLIC ti_sysbios_family_arm_m3_Hwi_interruptsAreDisabledButShouldNotBe

        EXTERN ti_sysbios_family_arm_m3_Hwi_doSwiRestore__I
        EXTERN ti_sysbios_family_arm_m3_Hwi_doTaskRestore__I
        EXTERN ti_sysbios_family_arm_m3_Hwi_dispatchC__I
        EXTERN ti_sysbios_family_arm_m3_Hwi_excHandler__I

        EXTERN ti_sysbios_family_arm_m3_Hwi_Module__state__V

;
; Here on all dispatched interrupts
;
; Warning! Do not overwrite r3 prior to jumping to dispatcher.
; When in sparse dispatch table mode, r3 contains the address of
; the Hwi object.
;

        SECTION CODE:CODE:NOROOT
        THUMB

ti_sysbios_family_arm_m3_Hwi_dispatch__I__mangled__:

;
; get IRP
; If this hwi switched to MSP then IRP is on PSP stack
; else if this is a nested interrupt then IRP is on current MSP stack
;
        tst     lr, #4          ; context on PSP?
        ite     NE
        mrsne   r1, psp         ; if yes, then use PSP
        moveq   r1, sp          ; else use MSP
        ldr     r0, [r1, #24]   ; get IRP (2nd of 8 items to be pushed)

#if defined (__ARMVFP__)
        sub     r2, r1, #72     ; make room for 9 FP regs
        ite     NE              ; CCs are still valid from tst lr above
        msrne   psp, r2         ; update appropriate SP
        moveq   sp, r2
        vstmdb  r1!, {d0-d7}    ; push vfp scratch regs on appropriate stack
        fmrx    r2, fpscr       ; push fpscr too
        str     r2, [r1, #-8]!  ; (keep even align)
#endif

        mrs     r1, basepri
        push    {r1}            ; save old basepri
        push    {lr}            ; save EXC_RETURN

        ldr     lr, dispatchAddr
        blx     lr              ; pass IRP to Hwi_dispatchC()
                                ; dispatcher returns swi and task keys in r0

; Here on return from dispatcher, r0 = tskKey, interrupts disabled
        ldr     r2, swiTaskKeyAddr
        ldr     r1, [r2]
        and     r0, r1, r0
        str     r0, [r2]

        pop     {lr}            ; restore EXC_RETURN
        tst     lr, #8          ; returning to thread mode?
        bne     lab$2           ; branch if lowest order interrupt

;
; return now, no further processing on nested interrupts
;
; This is normal when:
;  1) this is a nested interrupt,
;  2) the Task scheduler has been disabled by the interrupted task thread
;  3) this is a Swi-only system
;
        pop     {r0}            ; restore old basepri
        msr     basepri, r0     ;
#if defined (__ARMVFP__)
        ldr     r0, [r13], #8   ; pop fpscr (keep even align)
        fmxr    fpscr, r0       ;
        vldmia  r13!, {d0-d7}   ; pop vfp scratch regs
#endif
        bx      lr              ; return from interrupt

;
; Here when no other interrupt is pending
; perform a dummy EXC_RETURN to return to thread mode before
; running the Swi and Task schedulers
;
lab$2:
        pop     {r2}            ; restore old basepri
        tst     lr, #4          ; context on PSP?
        ite     NE
        mrsne   r1, psp         ; if yes, then use PSP
        moveq   r1, sp          ; else use MSP
        sub     r1, r1, #(8*4)  ; make room for dummy
        ite     NE              ; Z bit still valid from above tst
        msrne   psp, r1         ; update appropriate SP
        moveq   sp, r1

        str     r2, [r1, #4]    ; old basepri pops into r1

        mov     r0, #0x01000000 ; dummy xPSR with T bit set
        str     r0, [r1, #28]
        ldr     r0, lab$1Addr
        str     r0, [r1, #24]   ; last str with imm offset
                                ; 5 more inst before bx lr
                                ; (See ARM errata #838869)

; copy saved swi and task keys into interrupt context

        ldr     r3, swiTaskKeyAddr
        ldr     r0, [r3]
        str     r0, [r1]        ; tskkey pops into r0
        movw.w  r0, #0x00000101
        str     r0, [r3]        ; re-init swiTaskKey

        bx      lr              ; EXC_RETURN to $1 below

;
; Now we're back in thread mode and could be running on the task stack
; or the ISR stack.
;
; If an ISR hit while a Swi posted from an ISR was running, we'll be on the ISR stack.
; Otherwise we'll be on the pre-empted Task stack.
;
;       r0 = swiTaskKey
;       r1 = old basepri
;
; Call Swi and Task schedulers
;
lab$1:
        push    {r1}            ; push old basepri
        push    {r0}            ; push copy of swiTaskKey

        mrs     r1, control     ; on ISR stack already?
        cmp     r1, #0
        itt     NE
        movne   r2, #0          ; no,
        msrne   control, r2     ; switch to ISR stack

        push    {r1}            ; push old control reg onto ISR stack (MSP)
        push    {r1}            ; dummy to force 8 byte stack align

        ldr     lr, doSwiRestoreAddr
        blx     lr              ; pass swiTaskKey (r0) to Hwi_doSwiRestore()

        pop     {r0}            ; pop dummy
        pop     {r0}            ; pop old control register

        cmp     r0, #0          ; lowest order ISR?
        itt     NE
        movne   r1, #2          ; if yes
        msrne   control, r1     ; switch back ti Task stack (PSP)


        pop     {r0}            ; pop copy of swiTaskKey
        ldr     lr, doTaskRestoreAddr
        blx     lr              ; pass swiTaskKey to Hwi_doTaskRestore()
                                ; current task may be pre-empted here
;
; Here on return from doTaskRestoreHwi() with ints disabled.
;
; Extra care must be taken now to ensure that we completely unwind
; to the originally interrupted background thread without allowing
; any more dispatched interrupts. Otherwise the BIOS promise of no
; more than 2 (dispatched) interrupt contexts on a Task stack can be
; broken.
;
; In order to achieve this feat, we first post the pendSV interrupt
; which is configured with the same priority level as Hwi_disable().
; Then we re-enable interrupts. This will cause an immediate pendSV
; exception to be taken which is still at Hwi_disable() priority.
; We then blow off the pendSV stack contents and return to the
; background thread at the normal priority level.
;
        ldr     r0, nvic_icsr   ; point to Hwi_nvic.ICSR
        mov     r1, #0x10000000
        str     r1, [r0]        ; post a pendSV interrupt

        pop     {r0}            ; retrieve old basepri

; CAUTION!!! Do NOT single step thru the next instruction
; else, the processor won't arrive at pendSV thru the
; exception mechanism and catastrophic things will happen
; afterwards.

        msr     basepri, r0     ; causes pendSV to happen
        nop                     ; 2 nops required for prefetch
        nop                     ;

; Arriving here can only happen if interrupts were globally disabled
; when the pendSV interrupt was posted above. This can happen if
; user code has set the PRIMASK and not cleared it, or when single
; stepping with interrupts disabled.

ti_sysbios_family_arm_m3_Hwi_interruptsAreDisabledButShouldNotBe:
        b       ti_sysbios_family_arm_m3_Hwi_interruptsAreDisabledButShouldNotBe

        SECTION CODE:CODE:NOROOT
        THUMB
ti_sysbios_family_arm_m3_Hwi_pendSV__I__mangled__:
        tst     lr, #4          ; context on PSP?
        ite     NE
        mrsne   r1, psp         ; if yes, then use PSP
        moveq   r1, sp          ; else use MSP
        add     r1, r1, #(8*4)  ; blow off pendSV stack
#if defined (__ARMVFP__)
        ldr     r0, [r1], #8    ; pop fpscr (keep even align)
        fmxr    fpscr, r0       ;
        vldmia  r1!, {d0-d7}    ; pop vfp scratch regs
#endif
        ite     NE              ; Z bit still valid from above tst
        msrne   psp, r1         ; update appropriate SP
        moveq   sp, r1

ti_sysbios_family_arm_m3_Hwi_return__mangled__:
        bx      lr              ; return into task, re-enables ints

        DATA
        ALIGNROM 2
nvic_icsr:
        DC32        0xe000ed04
dispatchAddr:
        DC32        ti_sysbios_family_arm_m3_Hwi_dispatchC__I
doSwiRestoreAddr:
        DC32        ti_sysbios_family_arm_m3_Hwi_doSwiRestore__I
doTaskRestoreAddr:
        DC32        ti_sysbios_family_arm_m3_Hwi_doTaskRestore__I
swiTaskKeyAddr:
        DC32        ti_sysbios_family_arm_m3_Hwi_Module__state__V + 0x20
lab$1Addr:
        DC32        lab$1

        SECTION CODE:CODE:NOROOT
        THUMB

ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I__mangled__:
        tst     lr, #4          ; context on PSP?
        ite     NE
        mrsne   r0, psp         ; if yes, then use PSP
        moveq   r0, sp          ; else use MSP

        mov     sp, r0          ; use this stack
        stmfd   sp!, {r4-r11}   ; save r4-r11 while we're at it
        mov     r0, sp          ; pass sp to exception handler
        mov     r1, lr          ; pass lr too

        ldr     pc, excHandlerAddr
        nop
        DATA
        ALIGNROM 2
excHandlerAddr:
        DC32         ti_sysbios_family_arm_m3_Hwi_excHandler__I

        END
