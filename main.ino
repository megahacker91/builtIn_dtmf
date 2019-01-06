#include <builtIn_dtmf.h>
#define sensor_pin A0
#define available_pin 2
#define led_pin 13

dtmf_class port;


void setup(){
Serial.begin(9600);
port.begin(sensor_pin , available_pin , led_pin );

}

void loop(){
if(port.available()){ //if we  have 0 volt on "available_pin" . it is bullup pin normaly.
  char input_char = port.read(); //if we have tone it will return the char one time until relais it .return 0 if no tone
  char input_char_con = port.read_con();   //if we have tone it will return the char while the button pressed .return 0 if no tone
  if(input_char_con){                     // it will return true wile we have char .
    //but your code here.
  }
  if(input_char){                         // it will return true one time if  we have char then return false.
    
  }
}
}
