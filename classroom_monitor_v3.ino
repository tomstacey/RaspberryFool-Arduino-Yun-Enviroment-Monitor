
#include "DHT.h"

//Uses Adafruit DHT Library https://github.com/adafruit/DHT-sensor-library 

/*
Classroom monitor 
- Verison 1 - Light, temperature, humity sensors working
- Verison 2 - Provided ability to export data to google spread sheet via temboo
- Verison 3 - Removed temboo and add direct link to google spread (python code running on liunx wifi)

ToDo - list (22/2/2014)
- Do we need time delays in the setup & confirm the duration
- Create libary for the sensor functions
- Fix the timestamp problem

Hardware setup
- 

Notes
- to use the console telenet into the box
- telnet localhost 6571

*/


#include <Console.h>
#include <Wire.h>
#include <Process.h>
#include <FileIO.h>
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

#define DHTTYPE DHT11
#define DHTPIN 7

//dht DHT11;

DHT dht(DHTPIN, DHTTYPE);

int BH1750_address = 0x23; // i2c Addresse
float temperature_post;
byte buff[2];

//const int ledPin = 13; // the pin that the LED is attached to
int incomingByte;      // a variable to read incoming serial data into
int timestamp, nodeid, humidity, temperature, audiolevel;
String startString, timeString;


char python_key[2];


int analogPin = A1; 

void setup() 
{
  // initialize the bridge
  Bridge.begin();
  delay(500);          // Delay to allow the bridge to come up
  
  Wire.begin();
  BH1750_Init(BH1750_address);
  delay(500);
  
  dht.begin();
  
  syncTime();

// Start the python code running on the wifi liunx processor  
    Process startPython;

//  startPython.runShellCommand("python /mnt/sda1/classroom_001.py&");
    nodeid = 001;                            
}

void loop() 
{
  float light_valf=0;
  String dataString;
  int system_status;
  double db;

  int chk = dht.readTemperature();

  // Read the light sensor
  if(BH1750_Read(BH1750_address)==2)
  {
    light_valf=((buff[0]<<8)|buff[1])/1.2;
  }

  // Send the data to google spreadsheet
   //timeString = getTimeStamp2();
  timeString = "";


  audiolevel = analogRead(analogPin);    
  db =  20.0  * log10 (audiolevel  +1.);

  Bridge.put("timestamp", String(timeString));           // row a = time stamp
  Bridge.put("nodeid", String(nodeid));                  // row b = node id (hard coded)
  Bridge.put("humidity", String(dht.readHumidity()));        // row c = humidity
  Bridge.put("temperature", String(dht.readTemperature()));  // row d = temperature
  Bridge.put("lightlevel", String(light_valf));          // row e = light level
  Bridge.put("audiolevel", String(db));          // row f = audio level
  
  // set the bridge to update all the values in the spread sheet  
  Bridge.put("update", String(1));
 
  delay(30000);
}

// ===========================
// Light sensor init function
void BH1750_Init(int address)
{
  
  Wire.beginTransmission(address);
  Wire.write(0x10); // 1 [lux] aufloesung
  Wire.endTransmission();
}

// Light sensor read
byte BH1750_Read(int address)
{
  byte i=0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available())
  {
    buff[i] = Wire.read(); 
    i++;
  }
  Wire.endTransmission();  
  return i;
}

void syncTime(){
  Process p;
  p.runShellCommand("ntpd -q -p 0.europe.pool.ntp.org -p 0.openwrt.pool.ntp.org");
  while(p.running());  
}


/*
// This function return a string with the time stamp
 String getTimeStamp() 
 {
   String result;
   Process time;
   // date is a command line utility to get the date and the time 
   // in different formats depending on the additional parameter 
   time.begin("date");
   time.addParameter("+%D-%T");  // parameters: D for the complete date mm/dd/yy
                                 //             T for the time hh:mm:ss    
   time.run();  // run the command

   // read the output of the command
   while(time.available()>0) {
     char c = time.read();
     if(c != '\n')
       result += c;
   }

   return result;
}

String getTimeStamp2() 
 {
   String result;
    // restart the date process:
    if (!date.running())  
    {
      date.begin("date");
      date.addParameter("+%D-%T");
      date.run();
    }
  
  //if there's a result from the date process, parse it:
  while (date.available()>0) 
  {
    // get the result of the date process (should be hh:mm:ss):
     result = date.readString();   
  }
  
  return result;
}


*/

