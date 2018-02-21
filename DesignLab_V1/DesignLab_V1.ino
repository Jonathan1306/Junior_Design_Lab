
//Design Lab Code

/*Done so far:
  -  Reads the value of two Grove - Temperature Sensor, converts them to Celsius temperatures,
     and prints them to the LCD screen. The sensors are cnnected to pins A0 and A1.
*/

//Includes
#include <Wire.h>
#include "rgb_lcd.h"

// Define the pin to which the temperature sensor is connected.
const int pinTemp = A0;
const int pin2Temp = A1;

// Define the B-value of the thermistors.
// This value is a property of the thermistor used in the Grove - Temperature Sensor,
// and used to convert from the analog value it measures and a temperature value.
const int B = 4275;
const int R0 = 100000; //100k resistor

rgb_lcd lcd;

void setup()
{
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
}

void loop()
{
    // Get the (raw) value of the temperature sensors.
    int val = analogRead(pinTemp);
    int val2 = analogRead(pin2Temp);

    // Determine the current resistance of the thermistors based on the sensor values.
    float resistance = (float)(1023-val)*R0/val;
    float resistance2 = (float)(1023-val2)*R0/val2;

    // Calculate the temperatures based on the resistance values.
    float temperature = 1/(log(resistance/R0)/B+1/298.15)-273.15;
    float temperature2 = 1/(log(resistance2/R0)/B+1/298.15)-273.15;

    // clear the screen
    lcd.clear();
    
    //Output the Temperatures to the LCD
    lcd.print("FRIDGE: ");
    lcd.print(temperature);
    lcd.setCursor(0, 1);
    lcd.print("FREEZER: ");
    lcd.print(temperature2);
    
    // Wait one second between measurements.
    delay(1000);
}

