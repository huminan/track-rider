#ifndef SYS_H
#define SYS_H

#include "STC12C5A60S2.h"
#include "gprs.h"
#include "uart.h"
#include "timer.h"
#include <string.h>
#include "gps.h"

#define DEVICEID	'1'

sbit P0_0 = P0^0;
sbit P0_1 = P0^1;
sbit P0_2 = P0^2;
sbit P0_3 = P0^3;

#define uchar unsigned char

#endif