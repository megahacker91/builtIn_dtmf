
// include core Wiring API
#include "Arduino.h"

// include this library's description file
#include "builtIn_dtmf.h"
byte dtmf_sensor_pin;
byte dtmf_available_pin;
byte dtmf_led_pin;

float SAMPLING_RATE;
float TARGET;
int N = 125;
float coeff[8];
float Q1[8];
float Q2[8];
float cosine;
int nochar_count = 0;
float d_mags[8];
int testData[160];

const int dtmf_tones[8] = {697,770,852,941,1209,1336,1477,1633};
const unsigned char dtmf_map[16] = {0x11,0x21,0x41,0x12,0x22,0x42,0x14,0x24,0x44,0x28,0x81,0x82,0x84,0x88,0x18,0x48};
const char dtmf_char[16] = {'1','2','3','4','5','6','7','8','9','0','A','B','C','D','*','#',};




void dtmf_class::begin(byte sensor_pin, byte available_pin, byte led_pin)
{
  dtmf_sensor_pin = sensor_pin;
  pinMode(dtmf_sensor_pin,INPUT);
  dtmf_available_pin = available_pin;
    pinMode(dtmf_available_pin,INPUT_PULLUP);
  dtmf_led_pin = led_pin;
    pinMode(dtmf_led_pin,OUTPUT);

  #if F_CPU == 16000000L
  SAMPLING_RATE = 8928.0;
  #else
  SAMPLING_RATE = 4400.0;
  #endif

  float omega;
  // Calculate the coefficient for each DTMF tone
  for(int i = 0;i < 8;i++) {
    omega = (2.0 * PI * dtmf_tones[i]) / SAMPLING_RATE;
// DTMF detection doesn't need the phase.
// Computation of the magnitudes (which DTMF does need) does not
// require the value of the sin.
// not needed    sine = sin(omega);
    coeff[i] = 2.0 * cos(omega);
  }
  ResetDTMF();
}


/* Call this routine before every "block" (size=N) of samples. */
void dtmf_class::ResetDTMF(void)
{
  for(int i=0; i<8 ; i++) {
    Q2[i] = 0;
    Q1[i] = 0;
  }
}


/* Call this routine for every sample. */
//El_Supremo - change to int (WHY was it byte??)
void dtmf_class::ProcessSample(int sample,int adc_centre)
{
  float Q0;
//EL_Supremo subtract adc_centre to offset the sample correctly
  for(int i=0;i < 8;i++) {
    Q0 = coeff[i] * Q1[i] - Q2[i] + (float) (sample - adc_centre);
    Q2[i] = Q1[i];
    Q1[i] = Q0;
  }
}

/* Sample some test data. */
void dtmf_class::sample(int sensorPin)
{
// El_Supremo
// To toggle the output on digital pin 4
const unsigned char f_counter = 0x10;

  for (int index = 0; index < N; index++)
  {
    testData[index] = analogRead(sensorPin);
// El_Supremo
    // toggle bit 4 for a frequency counter
    PORTD ^= f_counter;
  }
}

// return the magnitudes of the 8 DTMF frequencies
void dtmf_class::detect(float dtmf_mag[],int adc_centre)
{
  int index;
  float d_tmp;

  /* Process the samples. */
  for (index = 0; index < N; index++)
  {
    ProcessSample(testData[index],adc_centre);
  }
  // Calculate the magnitude of each tone.
  for(int i=0;i < 8;i++) {
// El_Supremo 150318 the compilers in Arduino verisons 1.6.0 and 1.6.1
// generated "unable to find a register to spill" error in the original statement
// here. Breaking it into pieces worked around the problem.
//     dtmf_mag[i] = sqrt(Q1[i]*Q1[i] + Q2[i]*Q2[i] - coeff[i]*Q1[i]*Q2[i]);

    // This is the equivalent of sqrt(real*real + imag*imag)
    d_tmp = Q1[i]*Q1[i];
    d_tmp += Q2[i]*Q2[i];
    d_tmp -= coeff[i]*Q1[i]*Q2[i];

    dtmf_mag[i] = sqrt(d_tmp);
  }
  ResetDTMF();
}


char last_dtmf = 0;
// Detect which button was pressed using magnitude as the
// cutoff. Returns the character or a zero
char dtmf_class::button(float mags[],float magnitude)
{
  int bit = 1;
  int j;
  int dtmf = 0;



  for(int i=0;i<8;i++) {
    if(mags[i] > magnitude) {
      dtmf |= bit;
    }
    bit <<= 1;
  }
  for(j=0;j<16;j++) {
    if(dtmf_map[j] == dtmf)break;
  }
  if(j < 16) {
    // wait for the button to be released
    if(dtmf_char[j] == last_dtmf)return(0);
    last_dtmf = dtmf_char[j];
    return(dtmf_char[j]);
  }else{
  last_dtmf = 0;
  return(0);
}}
char dtmf_class::button_con(float mags[],float magnitude)
{
  int bit = 1;
  int j;
  int dtmf = 0;



  for(int i=0;i<8;i++) {
    if(mags[i] > magnitude) {
      dtmf |= bit;
    }
    bit <<= 1;
  }
  for(j=0;j<16;j++) {
    if(dtmf_map[j] == dtmf)break;
  }
  if(j < 16) {
    return(dtmf_char[j]);
  }else{
  return(0);
}}

char dtmf_class::read(){
  char char1;
  sample(dtmf_sensor_pin);
  detect(d_mags,506);
  char1 = button(d_mags,1800);
  if(char1) {
  nochar_count = 0;
  return char1;
  } else {
    if(nochar_count > 30000)nochar_count = 51;
    return 0 ;
  }

}

char dtmf_class::read_con(){
  char char1;
  sample(dtmf_sensor_pin);
  detect(d_mags,506);
  char1 = button_con(d_mags,1800);
  if(char1) {
  nochar_count = 0;
  return char1;
  } else {
    if(nochar_count > 30000)nochar_count = 51;
    return 0 ;
  }

}

bool dtmf_class::available(){
  if (!digitalRead(dtmf_available_pin)) {
    return 1;

  }else {
    return 0;

   }
 }
