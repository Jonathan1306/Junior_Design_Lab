
//Design Lab Code

/*Done so far:
  -  Reads the value of two Grove - Temperature Sensor, converts them to Celsius temperatures,
     and prints them to the LCD screen. The sensors are cnnected to pins A0 and A1.
*/

//Includes
#include <Wire.h>
#include "rgb_lcd.h"
#include <SPI.h>
#include <SD.h>
#include <stdlib.h>

// Define the pin to which the temperature sensor is connected.
const int pinTemp = A0;
const int pin2Temp = A1;

//Define the pin of the push button
const int buttonPin = 2;

//Define Start/Stop Recording LED
const int startLED = 3;

//Define SD Eject LED
const int ejectLED = 4;

//Define sensitivity of the ACS712
float sensitivity = 1850;

//Define reference voltage of the ACS712
float Vref = 2500;

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
    Serial.begin(9600);//TEMP

    //set up button
    pinMode(buttonPin, INPUT);
    
    //set up LEDs
    pinMode(startLED, OUTPUT);
    pinMode(ejectLED, OUTPUT);

    // see if the card is present and can be initialized:
    if (!SD.begin(4)) {
      //Display error message
      lcd.print("SD CARD ERROR");
      
      //Wait Indefinitely
      while (1);
    }
    Serial.println("card initialized.");
}

void loop()
{
  //Enter Idle State on Startup
  idleState();   
}

void idleState()
{   
  //Change LCD Output
  lcd.clear();
  lcd.print("IDLE STATE");
  lcd.setCursor(0, 1);
  lcd.print("PUSH TO START");
  
  //Toggle LEDs
  digitalWrite(startLED,LOW);
  digitalWrite(ejectLED,HIGH);  
  
  //Wait for button push
  delay(1000);
  while (!digitalRead(buttonPin))
  {
    //Do nothing 
  }
  
  //Enter Recording State
  delay(1000);
  recordingState();
  
  //**Maybe add jump to step 1 in case gets past this 
}

void recordingState()
{  
   // Clear the screen
   lcd.clear();
  
  //Toggle LEDs
  digitalWrite(startLED,HIGH);
  digitalWrite(ejectLED,LOW); 
   
  while(!digitalRead(buttonPin))
  {    
    // String for assembling the data to log:
    String dataString = "";
    
    //Buffer for temporary strings
    String buff = "";
    
    // Get the (raw) value of the temperature sensors.
    int val = analogRead(pinTemp);
    int val2 = analogRead(pin2Temp);

    // Determine the current resistance of the thermistors based on the sensor values.
    float resistance = (float)(1023.0-val)*R0/val;
    float resistance2 = (float)(1023.0-val2)*R0/val2;

    // Calculate the temperatures based on the resistance values and append to log string.
    float temperature = 1/(log(resistance/R0)/B+1/298.15)-273.15;
    dataString += String(val); //Raw Data
    dataString += String(",");
    
    float temperature2 = 1/(log(resistance2/R0)/B+1/298.15)-273.15;
    dataString += String(val2); //Raw Data
    dataString += String(",");

    //CURRENT SENSOR//
    unsigned int x=0;
    float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;
    
    //Get average of 150 current sensor readings
    for (int x = 0; x < 150; x++)
    { 
      //Get 150 samples
      AcsValue = analogRead(A0);     //Read current sensor values   
      Samples = Samples + AcsValue;  //Add samples together
      delay (3); // let ADC settle before next sample 3ms
    }
    //Calculate Voltage, Current and Power
    AvgAcs=Samples/150.0;//Taking Average of Samples
    float Voltage = (AvgAcs / 1024.0) * 5000; // Gets you mV
    float Current = ((Voltage - (Vref-700))/sensitivity);
    int Power = Voltage*Current;

    //Print Values
    Serial.print("Raw Value = " ); // shows pre-scaled value 
    Serial.print(AvgAcs); 
    Serial.print("\t Voltage = "); // shows the voltage measured 
    Serial.print(Voltage,3); // the '3' after voltage allows you to display 3 digits after decimal point
    Serial.print("\t Amps = "); // shows the voltage measured 
    Serial.println(Current,3); // the '3' after voltage allows you to display 3 digits after decimal point
    Serial.print("\n");
    
    //Append to log string
    dataString += String(Power);
    
    //Log to SD Card
    File dataFile = SD.open("datalog.txt", FILE_WRITE);

    // If the file is available, write to it
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
      Serial.println(dataString);//TEMP
    }
    // If the file isn't open
    else 
    {
      //Display error message
      lcd.print("LOGGING ERROR");
      
      //Wait indefinitely
      while(true){};
    }
  
    //Output the Temperatures to the LCD
    lcd.print("FRIDGE: ");
    lcd.print(temperature);
    lcd.setCursor(0, 1);
    lcd.print("FREEZER: ");
    lcd.print(temperature2);
    
    // Wait one second between measurements.
    delay(1000); 
  }
  idleState();
} 
