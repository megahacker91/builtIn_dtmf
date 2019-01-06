
#ifndef builtIn_dtmf_h
#define builtIn_dtmf_h

#include "Arduino.h"

class dtmf_class
{
public:
void begin(byte sensor_pin, byte available_pin, byte led_pin);
char read();
char read_con();
bool available();

private:
void sample(int);
void detect(float dtmf_mag[],int adc_centre);
char button(float mags[],float magnitude);
char button_con(float mags[],float magnitude);
void ProcessSample(int,int);
void ResetDTMF(void);
};

#endif
