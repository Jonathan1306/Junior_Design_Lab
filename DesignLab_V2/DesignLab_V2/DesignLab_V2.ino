
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

//Define the pin for the current sensor
const int sensorIn = A2;

//Define Start/Stop Recording LED
const int startLED = 3;

//Define SD Eject LED
const int ejectLED = 4;

//Define sensitivity of the ACS712
const int sensitivity = 185;

//Define Voltage for ACS712 Output
double Voltage = 0;

//Define RMS Voltage for ACS712 Output
double VRMS = 0;

//Define AmpsRMS for ACS712 Output
double AmpsRMS = 0;

//Define Power consumption for connected load.
double Power = 0;

//Define wall output voltage for power calculation.
double Vwall = 117.0;

//Define reference voltage of the ACS712
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
    if (!SD.begin(4)) {
      //Display error message
      lcd.print("SD CARD ERROR");
      lcd.setCursor(0, 1);
      lcd.print("CARD NOT FOUND");
      
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
    Serial.print(temperature);
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
    //Serial.print(AmpsRMS);
    //Serial.println(" Amps RMS");

    //Append to log string
    dataString += String(sensorIn);
    
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
      // Clear the screen
      lcd.clear();
      //Display error message
      lcd.print("LOGGING ERROR");
      lcd.setCursor(0, 1);
      lcd.print("CHECK SD CARD");
      //Turn off the Recording LED
      digitalWrite(startLED,LOW);
      
      //Wait indefinitely
      while(true){};
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
    
    // Wait 2.5 second between measurements.
    delay(2500); 

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
    delay(2500);
    
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
