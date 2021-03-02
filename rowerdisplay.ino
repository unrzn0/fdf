/* FluidRower display */
#include "pins_arduino.h"
#include <Wire.h>  
#include "SSD1306Wire.h"

#define HISTORY 50  // keep last 5s
#define PERIOD 200  // update every 100ms
#define PULSESPERMETER 5 // seems correct

// for 128x64 displays:
SSD1306Wire display(0x3c, SDA, SCL);  // ADDRESS, SDA, SCL

int reedPin = 23;
volatile int cnt = 0;
volatile int freq_cnt = 0;
int hist[HISTORY] = {0};
int hist_gradient[HISTORY] = {0};
int prevReed = LOW;
int last_time = 0;
int lastDisplayTime = 0;
int time_500m = 0;
long last_micros = 0;
int debouncing_time_ms = 2;
int period_100ms = 0;
float splittime_500m = 0;
int watts = 0;
int SPM = 0;
int pulses_last_stroke = 0;

void IRAM_ATTR IRQ_handler();

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(reedPin, INPUT_PULLDOWN);
  attachInterrupt(reedPin, IRQ_handler, RISING);
  Serial.begin(115200);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.drawString(2, 2, "Ready ...");
  display.display();
}

void IRAM_ATTR IRQ_handler()
{
   if ((long)(micros() - last_micros) >= debouncing_time_ms * 1000) {
      digitalWrite(LED_BUILTIN, HIGH);
      cnt++;
      freq_cnt++;
      last_micros = micros();
   }
}

// the loop function runs over and over again forever
void loop() {
  if(millis() - last_time >= PERIOD)
  {
    int local_freq_cnt = freq_cnt;
    noInterrupts();
    freq_cnt = 0;
    interrupts();
    
    last_time += PERIOD;
    for(int i=HISTORY-1; i>0; i--)
    {
       hist_gradient[i] = hist[i] - hist[i-1];
       hist[i] = hist[i-1];
    }
    hist_gradient[0] = hist[0] - local_freq_cnt;
    hist[0] = local_freq_cnt;

    if(hist[0] > 2 && hist_gradient[0] > 0)
    {
      //period_100ms = 99;
      for(int i=(1000/PERIOD); i<HISTORY; i++)
      {
        if(hist[i] > 2 && hist_gradient[i] > 0)
        {
          period_100ms = i;
          SPM = 60 * (1000/PERIOD) / period_100ms;
          pulses_last_stroke = 0;
          for(int k=0; k<i; k++)
             pulses_last_stroke += hist[k];
          float strokeperiod = (float)period_100ms / (float)(1000 / PERIOD);  // seconds - e.g. 14*5= 3seconds
          float strokedistance = (float)pulses_last_stroke / (float)PULSESPERMETER;  // e.g. 30/5=6m;
          splittime_500m = 500.f * strokeperiod / strokedistance;
          //splittime_500m = (int)((float)(500 * (1000/PERIOD)) / (float)(pulses_last_stroke * PULSESPERMETER)) ;
          break;
        }
      }
    }

    int j=0;
    display.clear();
    display.drawString(10, 10+2, "Dst: " + String(cnt/PULSESPERMETER) + "m");
    display.drawString(10, 2, "ST: " + String((int)splittime_500m) + "s   Pls: " + String(pulses_last_stroke) + "  D: " + String(period_100ms));
    display.drawString(60, 10+2, "SPM:  " + String(SPM) );
    display.drawString(60, 20+2, "/500:  " + String(((int)splittime_500m)/ 60) + ":" + String((int)splittime_500m%60) );
    
    for(j=0; j<HISTORY;j++)
    {
      int h=1+(hist[j]*3);
      int x=j*6;
      int y=63-h;
      int w=2;
      display.drawRect(x, y, w, h);
      h=1+(hist[j]*3+hist[j+1]*3)/2;
      x=j*6+3;
      y=63-h;
      w=2;      
      display.drawRect(x, y, w, h);
     
    }
    display.display();
  }
}
