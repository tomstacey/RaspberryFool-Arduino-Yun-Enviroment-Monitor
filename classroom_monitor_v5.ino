/*
Classroom monitor (Learnometer)
- Verison 1 - Light, temperature, humity sensors working
- Verison 2 - Provided ability to export data to google spread sheet via temboo
- Verison 3 - Removed temboo and add direct link to google spread (python code running on liunx wifi)
- Version 4 - Tom mucking about
- Version 5 - Brendon's ticker for time and other changes, Updated to use Adafruit DHT library - https://github.com/adafruit/DHT-sensor-library


*/


#include <Console.h>
#include <Wire.h>
#include <Process.h>
#include <FileIO.h>
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#include <DHT.h>
#include "DHT.h"

#define DHTTYPE DHT11
#define DHTPIN 7

DHT dht(DHTPIN, DHTTYPE);


int BH1750_address = 0x23; // i2c Addresse
float temperature_post;
byte buff[2];

//const int ledPin = 13; // the pin that the LED is attached to
int incomingByte, timestamp_counter=0;      // a variable to read incoming serial data into
int timestamp, nodeid, humidity, temperature, audiolevel;
long master_tick;
String startString, timeString;
//time_t timeValue;
//time_t Foo;

char python_key[2];

int analogPin = A1; 

void setup() 
{
  // initialize the bridge
  Bridge.begin();
  delay(500);          // Delay to allow the bridge to come up

/*
  Console.begin(); 

   while (!Console){
     ; // wait for Console port to connect.
   }
   Console.println("You're connected to the Console!!!!");
 */
  Wire.begin();
  BH1750_Init(BH1750_address);
  delay(500);
  
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
  Process wifiCheck;  // initialize a new process
  int chk = dht.readTemperature();

  if(master_tick > 1000)
  {
  
    if(timestamp_counter < 5)
    {
      syncTime();
      timestamp_counter = timestamp_counter +1;
  //    Console.print(F(" time stamp-"));
  //    Console.println(timestamp_counter);
    }
/*
   wifiCheck.runShellCommand("/usr/bin/pretty-wifi-info.lua | grep Signal");  // command you want to run
  char  c = wifiCheck.read();
  while(wifiCheck.running());
  int  result = wifiCheck.parseInt();
   // while there's any characters coming back from the 
   // process, print them to the serial monitor:
   if (result > 30) 
   {
     Console.println(F("Internet connection okay"));
   }
   else
   {
     Console.println(F("Internet connection not okay"));
   }
*/

//      Console.println(F("Sensor read & spreadsheet write"));
      master_tick = 0;
      
      // Read the light sensor
      if(BH1750_Read(BH1750_address)==2)
      {
        light_valf=((buff[0]<<8)|buff[1])/1.2;
      }

      // Send the data to google spreadsheet
      timeString = getTimeStamp();
  
      audiolevel = analogRead(A1);    
      //db =  20.0  * log10 (audiolevel  +1.);
  //    audiolevel = audiolevel;

      Bridge.put("timestamp", String(timeString));           // row a = time stamp
      Bridge.put("nodeid", String(nodeid));                  // row b = node id (hard coded)
  	Bridge.put("humidity", String(dht.readHumidity()));        // row c = humidity
  	Bridge.put("temperature", String(dht.readTemperature()));  // row d = temperature
      Bridge.put("lightlevel", String(light_valf));          // row e = light level
      Bridge.put("audiolevel", String(audiolevel));          // row f = audio level
  
      // set the bridge to update all the values in the spread sheet  
      Bridge.put("update", String(1));
 
  }
  else
  {
     master_tick = master_tick +1;
  }
 
  delay(50);
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


/*
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



