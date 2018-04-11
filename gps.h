#ifndef GPS_H
#define GPS_H

#define LATITUDE 11
#define LONGITUDE 12

void Record_Gps();
char Locate_Pos(char *buf,char cx);
char * mid(char *dst,char *src, int n,int m);
void CleanGpsRcv(void);
void UART_1Interrupt(void);

char Send_ATComm(const char *comm, const char *ret, unsigned char delay, bit wait);
#endif