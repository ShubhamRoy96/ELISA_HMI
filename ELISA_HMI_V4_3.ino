//Created by Shubham Roy with the help of multiple online websites and various tutorials.
//If used,can be used freely, but a shout-out would be appreciated! :)

#include "Arduino.h"
#include <EDB.h>
#include <EEPROM.h>
#include <UTFT.h>
#include<URTouch.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define SourcePin 9
#define NUMPIXEL 1
#define VoltagePin A0
#define TABLE_SIZE 4096
#define RECORDS_TO_CREATE 10
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUMPIXEL, SourcePin, NEO_GRB + NEO_KHZ800);

struct dataEvent {
  int id;
  int data;
}
dataEvent;
void writer(unsigned long address, byte data)
{
  EEPROM.write(address, data);
}

byte reader(unsigned long address)
{
  return EEPROM.read(address);
}
EDB db(&writer, &reader);


UTFT TLCD(TFT01_32,38,39,40,41); //Display selection and configuration
URTouch Touch(6, 5, 4, 3, 2);

extern uint8_t SmallFont[];
extern uint8_t BigFont[];


int x,y;
int i = 0;//for calculation()
char currentPage;

int xR=38;
int xG=38;
int xB=38;

void setup() {
TLCD.InitLCD(); //maybe use this to refresh touch issue?
TLCD.clrScr();
Touch.InitTouch();
Touch.setPrecision(PREC_MEDIUM);
drawHomeScreen();
currentPage = '0';
pinMode (8, OUTPUT); // For full brightness on display
digitalWrite(8,HIGH);
pixel.begin();
pixel.show();
db.create(0, TABLE_SIZE, (unsigned int)sizeof(dataEvent));

}

void loop() {
    
    
  
  if(currentPage == '0') {
    if(Touch.dataAvailable()) {
      Touch.read();
      x=Touch.getX();
      y=Touch.getY();
      
      if ((x>=35) && (x<285) && (y>=90) && (y<=130)) {
        drawFrame(35, 90, 285, 130);
        currentPage = '1';
        TLCD.clrScr();
        Absorbance();
      }
      
      if ((x>=35) && (x<=285) && (y>=140) && (y<=180)) {
        drawFrame(35, 140, 285, 180);
        currentPage = '2';
        TLCD.clrScr();
        Transmittance();
      }

      if ((x>=35) && (x<=285) && (y>=190) && (y<=230)) {
        drawFrame(35, 190, 285, 230);
        currentPage = '3';
        TLCD.clrScr();
        SourceControl();
      }
    }
  }

  if(currentPage == '1' || currentPage == '2') {
    if(Touch.dataAvailable()) {
      Touch.read();
      x=Touch.getX();
      y=Touch.getY();
     
       if ((y>=75) && (y<=125))  // Upper row
      {
        if ((x>=15) && (x<=65))  // Button: 1
        {
          drawFrame(15, 75, 65, 125);
          Calculation(0);
        }
        if ((x>=75) && (x<=125))  // Button: 2
        {
          drawFrame(75, 75, 125, 125);
          Calculation(1);
        }
        if ((x>=135) && (x<=185))  // Button: 3
        {
          drawFrame(135, 75, 185, 125);
          Calculation(2);
        }
        if ((x>=195) && (x<=245))  // Button: 4
        {
          drawFrame(195, 75, 245, 125);
          Calculation(3);
        }
        if ((x>=255) && (x<=305))  // Button: 5
        {
          drawFrame(255, 75, 305, 125);
          Calculation(4);
        }
      }

      if ((y>=135) && (y<=185))  // Center row
      {
        if ((x>=15) && (x<=65))  // Button: 6
        {
          drawFrame(15, 135, 65, 185);
          Calculation(5);
        }
        if ((x>=75) && (x<=125))  // Button: 7
        {
          drawFrame(75, 135, 125, 185);
          Calculation(6);
        }
        if ((x>=135) && (x<=185))  // Button: 8
        {
          drawFrame(135, 135, 185, 185);
          Calculation(7);
        }
        if ((x>=195) && (x<=245))  // Button: 9
        {
          drawFrame(195, 135, 245, 185);
          Calculation(8);
        }
        if ((x>=255) && (x<=305))  // Button: 0
        {
          drawFrame(255, 135, 305, 185);
          Calculation(9);
        }
      }
      backbutton();
    }
  }

  if (currentPage == '3') {
    setSourceColor();
    if (Touch.dataAvailable()) {
        Touch.read();
        x=Touch.getX();
        y=Touch.getY();
        backbutton();  //Back button
    }
  }
}

void drawHomeScreen() {
  TLCD.fillScr(VGA_WHITE);
  TLCD.setColor(VGA_BLUE);
  TLCD.fillRect(0, 0, 319, 54);
  TLCD.setFont(SmallFont);
  TLCD.setBackColor(VGA_WHITE);
  TLCD.print("Using RGB LED", CENTER,63);
  TLCD.setColor(VGA_WHITE);
  TLCD.setBackColor(VGA_BLUE);
  TLCD.setFont(BigFont);
  TLCD.print("ELISA", CENTER,10);
  TLCD.print("MICROSTRIP READER", CENTER,32);
  TLCD.setColor(VGA_RED);
  TLCD.fillRect(0,54,319,55);
  
  TLCD.setColor(16,167,103);
  TLCD.fillRoundRect(35, 90, 285, 130);
  TLCD.fillRoundRect(35, 140, 285, 180);
  TLCD.fillRoundRect(35, 190, 285, 230);
  TLCD.setColor(VGA_BLACK);   // Sets color to black
  TLCD.drawRoundRect (35, 90, 285, 130);
  TLCD.drawRoundRect (35, 140, 285, 180);// Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  TLCD.drawRoundRect (35, 190, 285, 230);
  TLCD.setColor(VGA_WHITE); // Sets color to white
  TLCD.setBackColor(16, 167, 103); // Sets the background color of the area where the text will be printed to green, same as the button
  TLCD.print("ABSORBANCE", CENTER, 102); // Prints the string
  TLCD.print("TRANSMITTANCE", CENTER, 152);
  TLCD.print("SOURCE CONTROL", CENTER, 202);
  
}

void Absorbance() {
  Background();
  TLCD.setColor(VGA_BLUE);
  TLCD.setBackColor(VGA_WHITE);
  TLCD.print("(For Absorbance)", CENTER,50);
  
  TLCD.setBackColor(VGA_BLUE);
  TLCD.setColor(VGA_WHITE);
  TLCD.print("Absorbance = " ,10, 200);
  drawButtons();
  TLCD.fillRect(230,200,300,220);
  TLCD.setColor(VGA_BLACK);
  TLCD.drawRect(230,200,300,220);
  drawHomeButton();

  
}

void Transmittance() {
  Background();
  TLCD.setColor(VGA_BLUE);
  TLCD.setBackColor(VGA_WHITE);
  TLCD.print("(For Transmittance)", CENTER,46);
  
  TLCD.setBackColor(VGA_BLUE);
  TLCD.setColor(VGA_WHITE);
  TLCD.print("Transmittance" ,10, 200);
  TLCD.print("=" ,220, 200);
  drawButtons();
  TLCD.fillRect(240,200,300,220);
  TLCD.setColor(VGA_BLACK);
  TLCD.drawRect(240,200,300,220);
  drawHomeButton();
  
}

void SourceControl() {
  setSourceColor();
  TLCD.fillScr(VGA_WHITE);
  TLCD.setColor(VGA_BLUE);
  TLCD.setBackColor(VGA_WHITE);
  TLCD.print("LED Color:", 10, 61);
  TLCD.print("R", 10, 105);
  TLCD.print("G", 10, 145);
  TLCD.print("B", 10, 185);
  TLCD.fillRect(0, 220, 319, 239);
  TLCD.fillRect(0, 0, 319, 40);
  TLCD.setBackColor(VGA_BLUE);
  TLCD.setColor(VGA_WHITE);
  drawHomeButton();
  TLCD.setFont(BigFont);
  TLCD.print("RGB LED Control", CENTER, 10);
  TLCD.setColor(VGA_RED);
  TLCD.fillRect(0, 40, 319, 41); 
  TLCD.setColor(VGA_BLACK);
  TLCD.drawRect(30, 108, 310, 118); // R - Slider
  TLCD.drawRect(30, 148, 310, 158);
  TLCD.drawRect(30, 188, 310, 198);
}

void drawFrame(int x1, int y1, int x2, int y2) {
  TLCD.setColor(VGA_RED);
  TLCD.drawRoundRect (x1, y1, x2, y2);
  while (Touch.dataAvailable())
    Touch.read();
    TLCD.setColor(VGA_WHITE);
    TLCD.drawRoundRect (x1, y1, x2, y2);
}

void drawButtons()
{
// Draw the upper row of buttons
  for (x=0; x<5; x++)
  {
    TLCD.setColor(16,167,103);
    TLCD.fillRoundRect (15+(x*60), 75, 65+(x*60), 125);
    TLCD.setColor(VGA_WHITE);
    TLCD.drawRoundRect (15+(x*60), 75, 65+(x*60), 125);
    TLCD.setBackColor(16,167,103);
    TLCD.printNumI(x+1, 32+(x*60), 92);
  }

  for (x=0; x<5; x++)
  {
    TLCD.setColor(16,167,103);
    TLCD.fillRoundRect (15+(x*60), 135, 65+(x*60), 185);
    TLCD.setColor(VGA_WHITE);
    TLCD.drawRoundRect (15+(x*60), 135, 65+(x*60), 185);
    TLCD.setBackColor(16,167,103);
    if (x<4)
      TLCD.printNumI(x+6, 32+(x*60), 152);
  }
  TLCD.print("10", 265, 152);

      
}

          
void setSourceColor() {
  if (Touch.dataAvailable()) {
    Touch.read();
    x=Touch.getX();
    y=Touch.getY();        
    if( (y>=100) && (y<=126)) { // Area of the Red color slider
      xR=x; // Stores the X value where the screen has been pressed in to variable xR
      if (xR<=38) { // Confines the area of the slider to be above 38 pixels
        xR=38;
      }
      if (xR>=303){ /// Confines the area of the slider to be under 310 pixels
        xR=303;
      }
    }
    // Area of the Green color slider
    if( (y>=140) && (y<=166)) {
      xG=x;
      if (xG<=38) {
        xG=38;
      }
      if (xG>=303){
        xG=303;
      }
    }
    // Area of the Blue color slider
    if( (y>=180) && (y<=206)) {
      xB=x;
      if (xB<=38) {
        xB=38;
      }
      if (xB>=303){
        xB=303;
      }
    }    
  }
  // Maps the values of the X - Axis from 38 to 0 and 310 to 255, because we need values from 0 to 255 for turning on the led
  int xRC = map(xR,38,310,0,255);
  int xGC = map(xG,38,310,0,255);
  int xBC = map(xB,38,310,0,255);
  
  pixel.setBrightness(255);
  pixel.setPixelColor(0, pixel.Color(xRC,xGC,xBC));  //set source led color
  pixel.show();
  
  // Draws a rectangle with the latest color combination 
  TLCD.setColor(VGA_BLACK);
  TLCD.drawRoundRect(174, 56, 311, 90);
  TLCD.setColor(xRC, xGC, xBC);
  TLCD.fillRoundRect(175, 57, 310, 89);
  
  
  // Draws the positioners
  TLCD.setColor(VGA_WHITE);
  TLCD.fillRect(xR,109,(xR+4),117);
  TLCD.fillRect(xG,149,(xG+4),157);
  TLCD.fillRect(xB,189,(xB+4),197);// Positioner
  TLCD.setColor(xRC, 0, 0);
  TLCD.fillRect(31, 109, (xR-1), 117);
  TLCD.setColor(0, xGC, 0);
  TLCD.fillRect(31, 149, (xG-1), 157);
  TLCD.setColor(0, 0, xBC);
  TLCD.fillRect(31, 189, (xB-1), 197);
  TLCD.setColor(VGA_BLACK);
  TLCD.fillRect((xR+5), 109, 309, 117);
  TLCD.fillRect((xG+5), 149, 309, 157);
  TLCD.fillRect((xB+5), 189, 309, 197);
}

void backbutton()
{
  if ((x>=10) && (x<=60) &&(y>=225) && (y<=239)) {
          drawFrame(5, 222, 50, 237);
          currentPage = '0';
          TLCD.clrScr();
          drawHomeScreen();
  }
}

void Background()
{
  TLCD.fillScr(VGA_WHITE);
  TLCD.setBackColor(VGA_BLUE);
  TLCD.setColor(VGA_BLUE);
  TLCD.fillRect(0, 0, 319,40);
  TLCD.fillRect(0,195,319,239);
  TLCD.setColor(VGA_WHITE);
  TLCD.setFont(BigFont);
  TLCD.print("Select Well Number", CENTER,10);
  TLCD.setColor(VGA_RED);
  TLCD.fillRect(0, 40, 319, 41);
}

void drawHomeButton()
{  
  TLCD.setFont(SmallFont);
  TLCD.setBackColor(VGA_BLUE);
  TLCD.setColor(VGA_WHITE);
  TLCD.print("<Home" ,10, 225);
}

void Calculation(int i)
{
  int count;
  float VoltageOut;
  float VoltageAverage;
  float VoltageRead;
  for(count = 0;count<5;count++)
  {
    VoltageOut = analogRead(VoltagePin);
    VoltageRead = VoltageRead + VoltageOut / 1024.0 * 5.0;
    delay(1000);
  }
  VoltageAverage = (VoltageRead / 5);
  dataEvent.id = i;
  dataEvent.data = VoltageAverage;
  db.appendRec(EDB_REC dataEvent);
  if(i<10) i++;
  else i=0;
  TLCD.setBackColor(VGA_WHITE); //Data on Screen
  TLCD.setColor(VGA_BLACK);
  TLCD.setFont(BigFont);
  TLCD.printNumF(dataEvent.data, 1, 250 ,204);
  delay(1000);
  TLCD.print("   ", 250 ,200); 
}

