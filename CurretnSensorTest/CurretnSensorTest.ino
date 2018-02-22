/*
Measuring Current Using ACS712
*/
/*
const int analogIn = A0;
int mVperAmp = 160;
int RawValue= 0;
int ACSoffset = 2440; 
double Voltage = 0;
double Amps = 0;

void setup(){ 
 Serial.begin(9600);
}

void loop(){
 
 RawValue = analogRead(analogIn);
 Voltage = (RawValue / 1024.0) * 5000; // Gets you mV
 Amps = ((Voltage - (ACSoffset + 700)) / mVperAmp);
 
 
 Serial.print("Raw Value = " ); // shows pre-scaled value 
 Serial.print(RawValue); 
 Serial.print("\t mV = "); // shows the voltage measured 
 Serial.print(Voltage,3); // the '3' after voltage allows you to display 3 digits after decimal point
 Serial.print("\t Amps = "); // shows the voltage measured 
 Serial.println(Amps,3); // the '3' after voltage allows you to display 3 digits after decimal point
 delay(500); 
}
//*/
//*
float sensitivity = 1.6;
float Vref = 2.44;

void setup() {
  Serial.begin(9600); //Start Serial Monitor to display current read value on Serial monitor
}

void loop() {
unsigned int x=0;
float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;

  for (int x = 0; x < 150; x++){ //Get 150 samples
  AcsValue = analogRead(A0);     //Read current sensor values   
  Samples = Samples + AcsValue;  //Add samples together
  delay (3); // let ADC settle before next sample 3ms
}
AvgAcs=Samples/150.0;//Taking Average of Samples
float Voltage = (AvgAcs / 1024.0) * 5; // Gets you V
float Amps = ((Voltage - (Vref - 0.7))/1.6);

//((AvgAcs * (5.0 / 1024.0)) is converitng the read voltage in 0-5 volts
//2.5 is offset(I assumed that arduino is working on 5v so the viout at no current comes
//out to be 2.5 which is out offset. If your arduino is working on different voltage than 
//you must change the offset according to the input voltage)
//0.185v(185mV) is rise in output voltage when 1A current flows at input

Serial.print("Raw Value = " ); // shows pre-scaled value 
Serial.print(AvgAcs); 
Serial.print("\t Voltage = "); // shows the voltage measured 
Serial.print(Voltage,3); // the '3' after voltage allows you to display 3 digits after decimal point
Serial.print("\t Amps = "); // shows the voltage measured 
Serial.println(Amps,3); // the '3' after voltage allows you to display 3 digits after decimal point

//Serial.print(AcsValueF);//Print the read current on Serial monitor
Serial.print("\n");
delay(50);
}
//*/
