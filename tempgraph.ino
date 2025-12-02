#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#define DHTPIN 5
#define DHTTYPE DHT11
#define RED 0xF800
/*
* Connections
* SSD1306 OLED | Arduino Uno
* ---------------------------
*     VCC      |    +5V       (Vcc/power/+ve)
*     GND      |    GND       (Ground/-ve/0v)
*     SCL      |    A5        (Serial Clock Line)
*     SDA      |    A4        (Serial Data Line)
*/

const int SCREEN_WIDTH = 128; // OLED display width, in pixels
const int SCREEN_HEIGHT = 64; // OLED display height, in pixels
int intro = 0;
int option = 0; //will be connected to momentary button
int temperature;
long time = 0;
float tempValues [10];
float humidityValues[10];
int tempValuesIndex = 0;
int humidityValuesIndex = 0;
bool btnPressed = false;
DHT dht(DHTPIN,DHTTYPE); //data pin and the type of sensor being used
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  pinMode(10,OUTPUT);
  Serial.begin(9600);
  dht.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    while(true);
  }
}

void loop() {
  //----TODO-----
  //ADD MOMENTARY PUSH BUTTON TO CHANGE MODE
  //time = millis();
  //update the time every minute and store current temperature in array
  //then increment the idex
  
  if(millis() - time > 600000){
    tempValues[tempValuesIndex] = dht.readTemperature();
    humidityValues[humidityValuesIndex] = dht.readHumidity();
    if(tempValuesIndex < 9){ //only increment up to 9 because that will be array end
      tempValuesIndex ++;
      humidityValuesIndex ++;
    }else{
      for(int i=0; i < 9; i++){  //move temperature readings back one index in the array
        if(i < 9){
         tempValues[i] = tempValues[i + 1]; //move old reading one index back.. first index will be replaced completely
         humidityValues[i] = humidityValues[i + 1]; 
        }else{
          tempValues[9] = dht.readTemperature(); //new reading added to end of array
          humidityValues[9] = dht.readHumidity();
        }
        }
      }
 
    for(int i=0; i < 9; i++){
      Serial.println(tempValues[i]);
      }
   
    time = millis();
  }

  introMessage(); //display intro message
  
  temperature = analogRead(A0);
  setDisplayProperties(35);

 
  //using print instead of println keeps it all on the same line
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  //update option selected when user clicks button
  Serial.println(digitalRead(7));
  if(digitalRead(7)){
    Serial.println(digitalRead(7));
    if(option < 4){
      option++;
      delay(500);
    }else{
      option = 0;
      delay(500);
    }
   
  }

 


  switch (option) {
  case 0:
  showTemp();
    //showBarGraph(tempValues,80,-40,"Temp");
    break;
  case 1:
  //showHumidity();
  showLineGraph(100,0,"Humidity",humidityValues);
    //showHumidityBarGraph();
    break;
  case 2:
    //showTemperatureGraph();
    showLineGraph(80,-40,"Temp",tempValues);
    break;
  
  case 3: 
  showBarGraph(humidityValues,100,0,"Humidity");
  break;

  case 4: 
  showHumidity();
  break;
}

  //turn on led if temperature exceeds 25 degrees
  /*
  if(temperature > 25){
    digitalWrite(10,HIGH);
    option = 2;
  }else if(temperature < 0){
    option = 1;
  }
  else{
    digitalWrite(10,LOW);
    option = 0;
  }
*/
}

void setDisplayProperties(int cursor1){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(cursor1, 10);
  
}

void introMessage(){
     if(intro == 0){
  setDisplayProperties(15);
  // Display static text
  display.println("Developed");
  display.display();
  delay(1500);
  setDisplayProperties(50);
  display.println("by");
  display.display();
  delay(1500);
  setDisplayProperties(20);
  display.println("M.Travis");
  display.display();
  delay(1500);
  intro = 1;
  }
}

void showTemp(){
  display.setTextSize(2);
  display.setCursor(20,20);
  display.print(dht.readTemperature());
  display.print((char)247);
  display.print("C");
  display.display();
}

void showHumidity(){
  display.print(dht.readHumidity());
  display.setTextSize(1);
  display.print((char)37);
  display.print("rh");
  display.display();
}


void showBarGraph(float arr[],int hi,int lo,String title){
 
  drawOutline(hi,lo,title);
  if(arr[9]){
    //Serial.println("Enough data to create graph");
    int xIndex = 20;
    for(int i=0; i < 9; i++){
      //-40 is the lower temp range, 80 is the high range, 50 is the low point on the graph, 10 is the high point
      int y0 = map(arr[i],lo,hi,50,10);
      display.drawRect(xIndex, y0, 9, 50 - y0, WHITE);
      xIndex += 10;
    }
  }else{
      display.setCursor(25,25);
      display.print("Not enough Data");
    }
  display.display();

}


void showLineGraph(int hi,int lo,String title,float arr[]){
  drawOutline(hi,lo,title);

      if(arr[9]){
    int xIndex = 20;
    for(int i=0; i < 8; i++){
      //-40 is the lower temp range, 80 is the high range, 50 is the low point on the graph, 10 is the high point
      int y0 = map(arr[i],lo,hi,50,10);
      int y1 = map(arr[i + 1],lo,hi,50,10);
      display.drawLine(xIndex,y0,xIndex + 10,y1,WHITE);
      //tempValues[i];
      xIndex += 10;
    }
  }else{
      display.setCursor(25,25);
      display.print("Not enough Data");
    }
     display.display();
  }


void drawOutline(int hi,int lo,String title){
  setDisplayProperties(10);
  //if else statements determine placement for graph temperatures
  if(hi > 20 && hi < 100){
    display.setCursor(4,8);
  }else if(hi < 10 && hi > -10){
    display.setCursor(8,8);
  }else if(hi >= 100 || hi <= -100){
    display.setCursor(0,8);
  }
  display.setTextSize(1);
  display.print(hi);
 
 if(lo > 20 && lo < 100){
   display.setCursor(2,48);
 }else if(lo < 10 && lo > -10){
   display.setCursor(9,48);
 }else if(lo >= 100 || lo <= -11){
   display.setCursor(0,48);
 }
    
  display.print(lo);
  display.setCursor(50,2);
  display.print(title);
  if(title == "Temp"){
    display.print((char)247);
    display.print("C");
  }
 
  display.setCursor(50,55);
  display.print("Time(h)");
  display.setCursor(24,54);
  display.print(0);
  display.setCursor(108,54);
  display.print(10);
  display.drawFastHLine(20,50,SCREEN_WIDTH - 38,WHITE); //represent 10 different temperate readings over a certain period of time
  display.drawFastVLine(20,5,SCREEN_HEIGHT - 10, 0xBDF7); //represent temperature reading values
  

}