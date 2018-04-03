

/**********************************
 * Engineering 5800 
 * Junior Design Project
 * 
 * Intel Galileo Based Data Logger
 * Project Code
 * 
 * Created by: 
 * Jonathan Traverse - 201411451
 * David Walsh - 201435609
 * 
**********************************/

//Includes
#include <Wire.h>
#include "rgb_lcd.h"
#include <SPI.h>
#include <SD.h>
#include <stdlib.h>

//Temperature sensor pins
const int pinTemp = A0;
const int pin2Temp = A1;

//Push button pin
const int buttonPin = 2;

//Current sensor pin
const int sensorIn = A2;

//Start/Stop Recording LED pin
const int startLED = 3;

//SD Eject LED pin
const int ejectLED = 4;

//ACS712 sensitivity
const int sensitivity = 175;

//ACS712 Output Voltage
double Voltage = 0;

//ACS712 Output RMS Voltage
double VRMS = 0;

//DACS712 Output RMS Current
double AmpsRMS = 0;

//Load power consumption
double Power = 0;

//Wall output voltage for power calculation.
double Vwall = 117.0;

//Reference voltage of the ACS712
//float Vref = 2500;

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
    if (!SD.begin(4)) 
    {
      //Display error message
      lcd.print("SD CARD ERROR");
      lcd.setCursor(0, 1);
      lcd.print("CARD NOT FOUND");

      //Toggle LEDs
      digitalWrite(startLED,LOW);
      digitalWrite(ejectLED,HIGH);
      
      //Wait for Button Push to try again
      int i = 0;
      while (i == 0)
      {
        if (digitalRead(buttonPin))
        {
          if (SD.begin(4))
          {
            i = 1;
          }
        }
      }
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
  delay(1000); 
   
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
    //Serial.print(temperature);
    dataString += String(",");
    
    float temperature2 = 1/(log(resistance2/R0)/B+1/298.15)-273.15;
    dataString += String(val2); //Raw Data
    dataString += String(",");

    //CURRENT SENSOR//
    Voltage = getVPP();
    VRMS = (Voltage/2.0) *0.707; 
    AmpsRMS = (VRMS * 1000)/sensitivity;
    Power = AmpsRMS * Vwall;
    
    //Print Values DEBUGGING w/serial monitor only
    Serial.print(AmpsRMS);
    Serial.println(" Amps RMS");

    //Append to log string
    dataString += String(sensorIn);
    
    //Log to SD Card
    File dataFile = SD.open("datalog.txt", FILE_WRITE);

    // If the file is available, write to it
    if (dataFile) 
    {
      dataFile.println(dataString);
      dataFile.close();
      //Serial.println(dataString);//TEMP
    }
    // If the file isn't open
    else 
    {
      // Clear the screen
      lcd.clear();
      //Display error message
      lcd.print("LOGGING ERROR");
      lcd.setCursor(0, 1);
      lcd.print("CHECK SD CARD");
      
      //Toggle LEDs
      digitalWrite(startLED,LOW);
      digitalWrite(ejectLED,HIGH);
      
      //Wait for Button Push to try again
      int i = 0;
      while (0 == i)
      {
        if (digitalRead(buttonPin))
        {
          File dataFile = SD.open("datalog.txt", FILE_WRITE);
          if (dataFile)
          {
            i = 1;
          }
        }
      }
      continue;
    }

    // Clear the screen to display temperature stats
    lcd.clear();
    //Output the Temperatures to the LCD
    lcd.print("FRIDGE: ");
    lcd.print(temperature);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("FREEZER: ");
    lcd.print(temperature2);
    lcd.print("C");
    
    // Wait 2.5 second between screen swiches
    delay(1250); 

    if(digitalRead(buttonPin))
    {
      continue;
    }
        
    //Clear the screen again to display current and power stats.
    lcd.clear();
    //Output the Current and Power to the LCD
    lcd.print("CURRENT: ");
    lcd.print(AmpsRMS);
    lcd.print("A");
    lcd.setCursor(0, 1);
    lcd.print("Power: ");
    lcd.print(Power);
    lcd.print("W");

    //Wait 2.5 second between measurements.
    delay(1250);
    
  }
  idleState();
}
 float getVPP()
{
  float result;
  
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1023;          // store min value here
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {                                                                                                                                                                                                            
           /*record the maximum sensor value*/
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5.0)/1023.0;
     
   return result;
 }
