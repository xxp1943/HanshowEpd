# HanshowEpd
Modify and program the Hanshow 2.13 inch price tag as a EPD clock. 

## Overview
The Hanshow 2.13 inch price tag is very interesting device. It is a small MCU system with EPD to display the price. 
We can re-program it to realize some new functions. The goal of this repository is making an EPD clock.

It contains:
* 2.13 inch EPD
* MSP430G2553 MCU
* 25VQ21 256KB SPI Flash
* A7106 RF chip

![PriceTag](https://github.com/xxp1943/HanshowEpd/blob/EpdClock/Images/HSPriceTag.png)

## Hardware
![Connection](https://github.com/xxp1943/HanshowEpd/blob/EpdClock/Images/HardwareConnection.png)

## EpdClock Branch
This branch is used for EPD Clock.

### Modify Hardware
Modify the hardware to add 32768Hz crystal for RTC function.
1. Cut off the connection of P2.6 and P2.7.
2. Connect crystal to P2.6 and P2.7.
3. Connect the EPD_POW and RF_POW to P3.2 and P2.1
![Connection](https://github.com/xxp1943/HanshowEpd/blob/EpdClock/Images/Modify.png)
