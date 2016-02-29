/*
 * 8x8 Dot matrix MAX7219 
 * 
 * pin 12 : DIN 
 * pin 11 : CLK 
 * pin 10 : CS 
 * 
 * 
 * DS13021 Real Time Clock
 * 
 * pin 6 : SCLK // Arduino pin for the Serial Clock
 * pin 7 : IO   // Arduino pin for the Data I/O
 * pin 8 : CE   // Arduino pin for the Chip Enable
 * 
 */
#include "LedControl.h"
#include "ds1302.h"

LedControl lc=LedControl(12,11,10,2);

void setup() {
  Serial.begin(9600);

  // Start by clearing the Write Protect bit
  // Otherwise the clock data cannot be written
  // The whole register is written, 
  // but the WP-bit is the only bit in that register.
  DS1302_write (DS1302_ENABLE, 0);

  // Disable Trickle Charger.
  DS1302_write (DS1302_TRICKLE, 0x00);
  
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  for(int index=0;index<lc.getDeviceCount();index++) {
    lc.shutdown(index,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(index,0);
    /* and clear the display */
    lc.clearDisplay(index);
  }
}

byte number[][8] = {
  {
    B00000110,
    B00001001,
    B00001001,
    B00001001,
    B00001001,
    B00001001,
    B00001001,
    B00000110
  },
  {
    B00000010,
    B00000010,
    B00000010,
    B00000010,
    B00000010,
    B00000010,
    B00000010,
    B00000010
  },
  {
    B00000110,
    B00001001,
    B00000001,
    B00000010,
    B00000100,
    B00001000,
    B00001000,
    B00001111
  },
  {
    B00000110,
    B00001001,
    B00000001,
    B00000110,
    B00000001,
    B00000001,
    B00001001,
    B00000110
  },
  {
    B00000001,
    B00000011,
    B00000101,
    B00001001,
    B00001111,
    B00000001,
    B00000001,
    B00000001
  },
  {
    B00001111,
    B00001000,
    B00001000,
    B00001110,
    B00000001,
    B00000001,
    B00001001,
    B00000110
  },
  {
    B00000110,
    B00001000,
    B00001000,
    B00001110,
    B00001001,
    B00001001,
    B00001001,
    B00000110
  },
  {
    B00001111,
    B00001001,
    B00001001,
    B00000001,
    B00000001,
    B00000001,
    B00000001,
    B00000001
  },
  {
    B00000110,
    B00001001,
    B00001001,
    B00000110,
    B00001001,
    B00001001,
    B00001001,
    B00000110
  },
  {
    B00000110,
    B00001001,
    B00001001,
    B00000111,
    B00000001,
    B00000001,
    B00000001,
    B00000110
  },
  { // colon
    B00000000,
    B00000000,
    B00000001,
    B00000000,
    B00000000,
    B00000001,
    B00000000,
    B00000000
  },
};

void printNumber10(int d, int num)
{
  int first = num / 10;
  int second = num % 10;
  
  for (int i = 0; i < 8; ++i) {
    lc.setRow(d,i,(first > 0 ? (number[first][i] << 6) : 0) + (number[second][i] << 2) + number[10][i]);
  }
}

void printNumber(int d, int num)
{
  int first = num / 10;
  int second = num % 10;
  
  for (int i = 0; i < 8; ++i) {
    lc.setRow(d,i,(number[first][i] << 4) + number[second][i]);
  }
}

void loop() { 
  ds1302_struct rtc;
  char buffer[80];     // the code uses 70 characters.

  // Read all clock data at once (burst mode).
  DS1302_clock_burst_read( (uint8_t *) &rtc);

  sprintf(buffer, "Time = %02d:%02d:%02d, ", \
    bcd2bin(rtc.h24.Hour10, rtc.h24.Hour), \
    bcd2bin(rtc.Minutes10, rtc.Minutes), \
    bcd2bin(rtc.Seconds10, rtc.Seconds));
  Serial.print(buffer);

  sprintf(buffer, "Date(day of month) = %d, Month = %d, " \
    "Day(day of week) = %d, Year = %d", \
    bcd2bin(rtc.Date10, rtc.Date), \
    bcd2bin(rtc.Month10, rtc.Month), \
    rtc.Day, \
    2000 + bcd2bin(rtc.Year10, rtc.Year));
  Serial.println( buffer);

  printNumber10(1, bcd2bin(rtc.h12.Hour10, rtc.h12.Hour));
  printNumber(0, bcd2bin(rtc.Minutes10, rtc.Minutes));
  //printNumber(0, bcd2bin(rtc.Seconds10, rtc.Seconds));

  delay(1000);
}
