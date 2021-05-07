;******************************************************************************
;   Code for application report - "Real Time Clock Library"
;******************************************************************************
; THIS PROGRAM IS PROVIDED "AS IS". TI MAKES NO WARRANTIES OR
; REPRESENTATIONS, EITHER EXPRESS, IMPLIED OR STATUTORY,
; INCLUDING ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
; FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
; COMPLETENESS OF RESPONSES, RESULTS AND LACK OF NEGLIGENCE.
; TI DISCLAIMS ANY WARRANTY OF TITLE, QUIET ENJOYMENT, QUIET
; POSSESSION, AND NON-INFRINGEMENT OF ANY THIRD PARTY
; INTELLECTUAL PROPERTY RIGHTS WITH REGARD TO THE PROGRAM OR
; YOUR USE OF THE PROGRAM.
;
; IN NO EVENT SHALL TI BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
; CONSEQUENTIAL OR INDIRECT DAMAGES, HOWEVER CAUSED, ON ANY
; THEORY OF LIABILITY AND WHETHER OR NOT TI HAS BEEN ADVISED
; OF THE POSSIBILITY OF SUCH DAMAGES, ARISING IN ANY WAY OUT
; OF THIS AGREEMENT, THE PROGRAM, OR YOUR USE OF THE PROGRAM.
; EXCLUDED DAMAGES INCLUDE, BUT ARE NOT LIMITED TO, COST OF
; REMOVAL OR REINSTALLATION, COMPUTER TIME, LABOR COSTS, LOSS
; OF GOODWILL, LOSS OF PROFITS, LOSS OF SAVINGS, OR LOSS OF
; USE OR INTERRUPTION OF BUSINESS. IN NO EVENT WILL TI'S
; AGGREGATE LIABILITY UNDER THIS AGREEMENT OR ARISING OUT OF
; YOUR USE OF THE PROGRAM EXCEED FIVE HUNDRED DOLLARS
; (U.S.$500).
;
; Unless otherwise stated, the Program written and copyrighted
; by Texas Instruments is distributed as "freeware".  You may,
; only under TI's copyright in the Program, use and modify the
; Program without any charge or restriction.  You may
; distribute to third parties, provided that you transfer a
; copy of this license to the third party and the third party
; agrees to these terms by its first use of the Program. You
; must reproduce the copyright notice and any other legend of
; ownership on each copy or partial copy, of the Program.
;
; You acknowledge and agree that the Program contains
; copyrighted material, trade secrets and other TI proprietary
; information and is protected by copyright laws,
; international copyright treaties, and trade secret laws, as
; well as other intellectual property laws.  To protect TI's
; rights in the Program, you agree not to decompile, reverse
; engineer, disassemble or otherwise translate any object code
; versions of the Program to a human-readable form.  You agree
; that in no event will you alter, remove or destroy any
; copyright notice included in the Program.  TI reserves all
; rights not specifically granted under this license. Except
; as specifically provided herein, nothing in this agreement
; shall be construed as conferring by implication, estoppel,
; or otherwise, upon you, any license or other right under any
; TI patents, copyrights or trade secrets.
;
; You may not use the Program in non-TI devices.
;
;******************************************************************************
;   RTC Library
;
;   Description; Keeps track of seconds, minutes, day, month, year, AM/PM
;                Leap years are accounted for until year 2400
;                Both EU standard and US daylight savings time is implemented
;
;   Typical Cycle Count per function:
;   incrementSeconds            14
;   incrementMinutes            14
;   incrementHours              35
;   incrementDays               37
;   incrementMonths             14
;   incrementYears              36
;   setDate                     681
;   get24Hour                   23
;
;   Memory Usage:               714 bytes
;                               (12 bytes RAM)
;
;   L. Westlund / D. Szmulwicz
;   Version    1.1
;   Texas Instruments, Inc
;   January 2011
;   Built with Code Composer Studio Version: 4.2.1
;******************************************************************************
;Change log
;
;1.0 - Inital version - L.Westlund
;1.1 - Added data alignment for TI_year - D. Szmulwicz
;    - Changed US Daylight savings time algorithm
;******************************************************************************


            ;Variables
            .def  TI_day
            .def  TI_FebDays
            .def  TI_hour
            .def  TI_dayOfWeek
            .def  TI_PM
            .def  TI_second
            .def  TI_year
            .def  TI_minute
            .def  TI_month
            .def  TI_dayLightZone
            .def  TI_dayLightSavings
            ;Functions
            .def  incrementSeconds
            .def  incrementMinutes
            .def  incrementHours
            .def  incrementDays
            .def  incrementMonths
            .def  incrementYears
            .def  testLeap
            .def  setDate
            .def  get24Hour

            .bss TI_year, 2,2
            .bss TI_second, 1
            .bss TI_minute, 1
            .bss TI_hour, 1
            .bss TI_day, 1
            .bss TI_month, 1
            .bss TI_PM, 1
            .bss TI_FebDays, 1
            .bss TI_dayOfWeek, 1
            .bss TI_dayLightZone, 1
            .bss TI_dayLightSavings, 1

            .text                           ; text section
TI_daysInMonth .byte 0x31, 0x28, 0x31, 0x30, 0x31, 0x30, 0x31, 0x31, 0x30, 0x31, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x30, 0x31
TI_mNumbers    .byte 0x00, 0x03, 0x02, 0x05, 0x00, 0x03, 0x05, 0x01, 0x04, 0x06, 0x02, 0x04

;============================================================================
; incrementSeconds
;============================================================================
incrementSeconds
            clrc
            dadd.b  #0x01,   &TI_second     ; tick one second
            cmp.b   #0x60,   &TI_second     ; see if we've hit 60 seconds
            jne     return                  ; if not, return
            clr.b   &TI_second              ; if so, go back to 00
                                            ; fall down to increment minutes
;============================================================================
; incrementMinutes
;============================================================================
incrementMinutes
            clrc
            dadd.b  #0x01,   &TI_minute     ; tick one minute
            cmp.b   #0x60,   &TI_minute     ; see if we've hit 60 minutes
            jne     return                  ; if not, return
            clr.b   &TI_minute              ; if so, go back to 00
                                            ; fall down to increment hours
;============================================================================
; incrementHours
;============================================================================
incrementHours
            clrc
            mov.b   &TI_hour,   r14
            dadd.b  #0x01,   r14
            cmp.b   #0x01,   &TI_dayLightZone ; see what we are doing for DS
            jeq     US_DS_TEST
            jge     EU_DS_TEST

noDS        cmp.b   #0x12,   r14            ; test for 12:00 o'clock
            jne     notSwitchPM             ; if not, don't switch the PM variable
            tst.b   &TI_PM                  ; see if it is PM and we should roll
            jnz     rollAM                  ; PM and roll bit set, change to AM and roll day
            xor.b   #0x01,   &TI_PM         ; change PM value
returnHours
            mov.b   r14,     &TI_hour
return      ret
notSwitchPM
            cmp.b   #0x13,   r14            ; see if we've hit 13
            jne     returnHours             ; if not, return
            mov.b   #0x01,   &TI_hour       ; if so, 13 o'clock == 1 o'clock
            ret                             ; return to save CPU cycles
rollAM
            clr.b   &TI_PM                  ; clear PM, now it is AM
            mov.b   r14,    &TI_hour        ; save hour value
                                            ; fall down to increment days

;============================================================================
; incrementDays
;============================================================================
incrementDays
            clrc
            dadd.b  #0x01,   &TI_day        ; add a day
            cmp.b   #0x06,   &TI_dayOfWeek  ; see if it is Sat, end of week
            jl      notSat
            mov.b   #0xFF,   &TI_dayOfWeek
notSat      add.b   #0x01,   &TI_dayOfWeek
            mov.w   #TI_daysInMonth, r13    ; move the destination addr (word) to r13
            mov.b   &TI_month,  r15
            add.w   r15,     r13            ; add the month offset to the daysInMonth pointer
            mov.b   @r13,    r14            ; r14 = number of days in the current month
            cmp.b   #0x01,   &TI_month      ; month '01' == FEB
            jne     compareDays             ; special case for Feb due to leap years
getFebDays
            mov.b   &TI_FebDays, r14        ; r14 holds the number of days in the current Feb
compareDays
            sub.b   &TI_day,    r14         ; r14 = number days in Month - current day value
            jge     return                  ; if r14 > 0 ... we still have days left
            mov.b   #0x01,   &TI_day        ; if not, roll to day 1
                                            ; fall down to increment months

;============================================================================
; incrementMonths
;============================================================================
incrementMonths
            clrc
            dadd.b  #0x01,   &TI_month      ; increment month counter
            cmp.b   #0x12,   &TI_month      ; see if we've gone past the last month (11)
            jl      return                  ; if no return
resetMonth
            clr.b   &TI_month               ; if so, back to January == 0
                                            ; and fall through to years

;============================================================================
; incrementYear - This algorithm will fail on 2400 A.D and not compute leap
;============================================================================
incrementYears
            clrc
            dadd.w  #0x01,   &TI_year       ; add 1 to the year
testLeap    mov.w   &TI_year,   r14         ; r14 = year
            bit.w   #0xFF,   r14            ; called by C functions to test new year
            jz      notLeapYear             ; all 2x00's up to 2400 are not leaps
            bit.w   #0x10,   r14            ; test for odd numbered decade
            jz      evenTens
oddtens                                     ; in odd tens digit decades (10s,30s,50s,...)
            and.w   #0x0F,   r14
            cmp.w   #0x02,   r14            ; year 2 is a leap year
            jeq     setLeapYear
            cmp.w   #0x06,   r14            ; year 6 is a leap year
            jeq     setLeapYear
            jmp     notLeapYear
evenTens                                    ; in even tens digit decades (20s,40s,60s,...)
            and.w   #0x0F,   r14
            cmp.w   #0x00,   r14            ; year 0 is a leap year
            jeq     setLeapYear
            cmp.w   #0x04,   r14            ; year 4 is a leap year
            jeq     setLeapYear
            cmp.w   #0x08,   r14            ; year 8 is a leap year
            jeq     setLeapYear
            jmp     notLeapYear
setLeapYear
            mov.b   #0x29,   &TI_FebDays
            ret
notLeapYear
            mov.b   #0x28,   &TI_FebDays
            ret

;============================================================================
; Returns hour in 24 hour format
;============================================================================
get24Hour
            tst.b  &TI_PM
            jnz    afternoon
            cmp.b  #0x12,    &TI_hour
            jeq    zero_hour
            mov.b  &TI_hour, r12
            ret
zero_hour   clr    r12
            ret
afternoon   mov.b  &TI_hour, r12
            cmp    #0x12,    r12
            jeq    return
            clrc
            dadd.b #0x12,    r12
            ret

;============================================================================
; Tests whether this is a dalights savings day in the US
;============================================================================
US_DS_TEST
           cmp.b    #0x02,   r14            ; see if it is 2
           jne      noDS
           tst.b    &TI_PM                  ; see if it is in AM
           jnz      noDS
           cmp.b    #0x02,   &TI_month      ; see if it is March
           jeq      secondSunday_March_US
           cmp.b    #0x10,   &TI_month      ; see if it is Oct
           jeq      firstSunday_November_US
           jmp      noDS

secondSunday_March_US
           call     #secondSunday
           tst.b    r15                     ; return register for function
           jnz      start_US_DS
           jmp      noDS
start_US_DS
           mov.b    #0x03,   &TI_hour
           mov.b    #0x01,   &TI_dayLightSavings
           ret

firstSunday_November_US
           tst.b    &TI_dayLightSavings
           jz       noDS
           call     #firstSunday
           tst.b    r15                     ; return register for function
           jnz      end_US_DS
           jmp      noDS
end_US_DS
           mov.b    #0x01,   &TI_hour
           clr.b    &TI_dayLightSavings
           ret


;============================================================================
; Tests whether this is a dalights savings day in the EU
;============================================================================
EU_DS_TEST
           cmp.b    #0x13,   r14            ; see if it is 1
           jne      noDS
           tst.b    &TI_PM                  ; see if it is in AM
           jnz      noDS
           cmp.b    #0x02,   &TI_month      ; see if it is March
           jeq      lastSunday_March_EU
           cmp.b    #0x09,   &TI_month      ; see if it is Oct
           jeq      lastSunday_October_EU
           jmp      noDS

lastSunday_March_EU
           call     #lastSunday
           tst.b    r15                     ; return register for function
           jnz      start_EU_DS
           jmp      noDS
start_EU_DS
           mov.b    #0x02,   &TI_hour
           mov.b    #0x01,   &TI_dayLightSavings
           ret

lastSunday_October_EU
           tst.b    &TI_dayLightSavings
           jz       noDS
           call     #lastSunday
           tst.b    r15                     ; return register for function
           jnz      end_EU_DS
           jmp      noDS
end_EU_DS
           mov.b    #0x12, &TI_hour
           clr.b    &TI_dayLightSavings
           ret
;============================================================================
; firstSunday - returns a 1 in r15 if this is the first sunday in a month with 31 days, 0 otherwise
;============================================================================
firstSunday
           tst.b    &TI_dayOfWeek
           jnz      returnZero
           cmp.b    #0x08,   &TI_day
           jge       returnZero
           mov.b    #0x01,   r15
           ret
;============================================================================
; secondSunday - returns a 1 in r15 if this is the second sunday in a month, 0 otherwise
;============================================================================
secondSunday
          tst.b    &TI_dayOfWeek
          jnz      returnZero
          cmp.b    #0x08,    &TI_day
          jl       returnZero 
          cmp.b    #0x15,    &TI_day
          jge      returnZero
          mov.b    #0x01,    r15
          ret
;============================================================================
; lastSunday - returns a 1 in r15 if this is the last sunday in a month with 31 days, 0 otherwise
;============================================================================
lastSunday
           tst.b    &TI_dayOfWeek
           jnz      returnZero
           cmp.b    #0x25,   &TI_day
           jl       returnZero
           mov.b    #0x01,   r15
           ret
returnZero clr      r15
           ret

;============================================================================
; setDate (y, m, d) - Call this function to set the date to a specified date
;============================================================================
setDate
            push.w  r6
            push.w  r7
            push.w  r8
            push.w  r10                     ; save preserved registers 
            mov.b   #0x01,   &TI_dayLightSavings
            mov.b   r14,     r6             ; day into r6
            mov.b   r13,     r7             ; month into r7
            mov.w   r12,     r8             ; year into r8
            cmp.b   #0x03,   r7             ; see if month == Jan or Feb
            jge     skipMarch
            dec.w   r8                      ; if so, act as though it is the previous year (for leaps)
skipMarch
            mov.w   r8,      r11            ; r11 is working y value
            mov.w   r8,      r12            ; year
            rra.w   r12                     ; y / 4
            rra.w   r12

            add.w   r12,     r11            ; r11 = y + (y/4)
            mov.w   r8,      r12            ; y / 100
            mov.w   #0x64,   r14            ; 100 decimal
            call    #divide                 ; r12 = y/100
            sub.w   r12,     r11            ; r11 = y + (y/4)-(y/100)
            rra.w   r12                     ; y/400 = (y/100)>>2
            rra.w   r12
            add.w   r12,     r11
            dec.w   r7
            mov.b   TI_mNumbers(r7), r10
            inc.w   r7
            add.w   r10,     r11
            add.w   r6,      r11
            mov.w   r11,     r12
            mov.b   #0x07,   r14
            call    #modulus
            mov.b   r14,     &TI_dayOfWeek

            mov.b   r6,      r12
            call    #BIN2BCD4
            mov.b   r13,     &TI_day

            mov.b   r7,      r12
            dec.b   r12
            call    #BIN2BCD4
            mov.b   r13,     &TI_month

            cmp.b   #0x03,   r7             ; see if month == Jan or Feb
            jge     skipMarchReset
            inc.w   r8                      ; If so, year needs to be added to to reset
skipMarchReset
            mov.w   r8,      r12
            call    #BIN2BCD4
            mov.w   r13,     &TI_year
            call    #testLeap
            pop.w   r10
            pop.w   r8
            pop.w   r7
            pop.w   r6
            ret
;============================================================================
BIN2BCD4  ; Subroutine converts binary number R12 -> Packed 4- digit BCD R13
;           Input:  R12  0000 - 0FFFh, R15 working register
;           Output: R13  0000 - 4095
;============================================================================
            mov.w   #16,     r15            ; Loop Counter
            clr.w   r13                     ; 0 -> RESULT LSD
BIN1        rla.w   r12                     ; Binary MSB to carry
            dadd.w  r13,     r13            ; RESULT x2 LSD
            dec.w   r15                     ; Through?
            jnz     BIN1                    ; Not through
            ret                             ;
;============================================================================
; modulus - returns 14 = r12%r14
;============================================================================
modulus
            mov.w   r14,     r15
            mov.w   r12,     r13
            clr.w   r14
            mov.w   #0x1,    r12
labelM2     rla.w   r13
            rlc.w   r14
            cmp.w   r15,     r14
            jnc     labelM1
            sub.w   r15,     r14
labelM1     rlc.w   r12
            jnc     labelM2
            ret
;============================================================================
; divide - returns r12 = r12 / r14
;============================================================================
divide
            mov.w   r14,     r15
            mov.w   r12,     r13
            clr.w   r14
            mov.w   #0x1,    r12
label       rla.w   r13
            rlc.w   r14
            cmp.w   r15,     r14
            jnc     label2
            sub.w   r15,     r14
label2      rlc.w   r12
            jnc     label
            ret

            .end