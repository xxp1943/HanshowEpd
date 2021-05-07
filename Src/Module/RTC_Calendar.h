//******************************************************************************
//   Code for application report - "Real Time Clock Library"
//
//   L.Westlund
//   Texas Instruments, Inc
//   August 2005
//******************************************************************************
// THIS PROGRAM IS PROVIDED "AS IS". TI MAKES NO WARRANTIES OR
// REPRESENTATIONS, EITHER EXPRESS, IMPLIED OR STATUTORY,
// INCLUDING ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
// COMPLETENESS OF RESPONSES, RESULTS AND LACK OF NEGLIGENCE.
// TI DISCLAIMS ANY WARRANTY OF TITLE, QUIET ENJOYMENT, QUIET
// POSSESSION, AND NON-INFRINGEMENT OF ANY THIRD PARTY
// INTELLECTUAL PROPERTY RIGHTS WITH REGARD TO THE PROGRAM OR
// YOUR USE OF THE PROGRAM.
//
// IN NO EVENT SHALL TI BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
// CONSEQUENTIAL OR INDIRECT DAMAGES, HOWEVER CAUSED, ON ANY
// THEORY OF LIABILITY AND WHETHER OR NOT TI HAS BEEN ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGES, ARISING IN ANY WAY OUT
// OF THIS AGREEMENT, THE PROGRAM, OR YOUR USE OF THE PROGRAM.
// EXCLUDED DAMAGES INCLUDE, BUT ARE NOT LIMITED TO, COST OF
// REMOVAL OR REINSTALLATION, COMPUTER TIME, LABOR COSTS, LOSS
// OF GOODWILL, LOSS OF PROFITS, LOSS OF SAVINGS, OR LOSS OF
// USE OR INTERRUPTION OF BUSINESS. IN NO EVENT WILL TI'S
// AGGREGATE LIABILITY UNDER THIS AGREEMENT OR ARISING OUT OF
// YOUR USE OF THE PROGRAM EXCEED FIVE HUNDRED DOLLARS
// (U.S.$500).
//
// Unless otherwise stated, the Program written and copyrighted
// by Texas Instruments is distributed as "freeware".  You may,
// only under TI's copyright in the Program, use and modify the
// Program without any charge or restriction.  You may
// distribute to third parties, provided that you transfer a
// copy of this license to the third party and the third party
// agrees to these terms by its first use of the Program. You
// must reproduce the copyright notice and any other legend of
// ownership on each copy or partial copy, of the Program.
//
// You acknowledge and agree that the Program contains
// copyrighted material, trade secrets and other TI proprietary
// information and is protected by copyright laws,
// international copyright treaties, and trade secret laws, as
// well as other intellectual property laws.  To protect TI's
// rights in the Program, you agree not to decompile, reverse
// engineer, disassemble or otherwise translate any object code
// versions of the Program to a human-readable form.  You agree
// that in no event will you alter, remove or destroy any
// copyright notice included in the Program.  TI reserves all
// rights not specifically granted under this license. Except
// as specifically provided herein, nothing in this agreement
// shall be construed as conferring by implication, estoppel,
// or otherwise, upon you, any license or other right under any
// TI patents, copyrights or trade secrets.
//
// You may not use the Program in non-TI devices.
//
//******************************************************************************
#ifndef RTC_CALENDAR_H
#define RTC_CALENDAR_H

#define JANUARY   0x00
#define FEBRUARY  0x01
#define MARCH     0x02
#define APRIL     0x03
#define MAY       0x04
#define JUNE      0x05
#define JULY      0x06
#define AUGUST    0x07
#define SEPTEMBER 0x08
#define OCTOBER   0x09
#define NOVEMBER  0x10
#define DECEMBER  0x11

#define SUNDAY    0
#define MONDAY    1
#define TUESDAY   2
#define WEDNESDAY 3
#define THURSDAY  4
#define FRIDAY    5
#define SATURDAY  6

#define NO_DAYLIGHT_SAVINGS 0x00
#define US_DAYLIGHT_SAVINGS 0x01
#define EU_DAYLIGHT_SAVINGS 0x02

#define setTime(H,M,S,P) {TI_second = S; TI_minute = M; TI_hour = H; TI_PM = P;} // encoded as BCD chars

extern char TI_second;      // 0x00 - 0x59
extern char TI_minute;      // 0x00 - 0x59
extern char TI_hour;        // 0x00 - 0x12
extern char TI_day;         // 0x01 - 0x31
extern char TI_dayOfWeek;   // 0 - 6 .. Sunday = 0
extern char TI_month;       // 0x00 - 0x11 .. Jan = 0
extern int  TI_year;        // 0x0000-0x2399 .. leapyears not computed for 0x2400
extern char TI_PM;          // AM = 0, PM = 1
extern char TI_FebDays;     // 0x28 or 0x29 depending on leap year
extern char TI_dayLightZone;// 0x00 = no daylight savings, 0x01 = US daylight, 0x02 = EU daylight
extern char TI_dayLightSavings; // defines daylight savings in effect. 1 = in effect 0 = not in effect

extern void incrementSeconds(void);
extern void incrementMinutes(void);
extern void incrementHours(void);
extern void incrementDays(void);
extern void incrementMonths(void);
extern void incrementYears(void);
extern void testLeap(void); // will set FebDays to the appropriate value based on the current year
extern void setDate( int year, char month, char day);  //encoded as decimal numbers with Jan = 1 and 1st day = 1, 31st day = 31
extern char get24Hour(void);

#define LEAP_YEAR (TI_FebDays == 0x29)

#endif
