/*
* R2D2 main controller
* PWM 3, 5, 6, 9, 10, 11, 13
*
* CONNECTIONS:
*  0 - RX
*  1 - TX     
*  2 -                             
*  3 -             ------------  Audio Board TX
*  4 - RESERVED    ------------  M1 (Dome) Direction
*  5 - RESERVED    ------------- M1 (Dome) Speed
*  6 - RMV JMP     ------------  Motor Board PWM 1      
*  7 - RMV JMP     ------------  Motor Board DIR 1    9 -- 13 reserved?
*  8 -             ------------  Motor Board DIR 2
*  9 -             ------------  Motor Board PWM 2                           
* 10 - USB Host Shield
* 11 - USB Host Shield
* 12 - USB Host Shield
* 13 - USB Host Shield
*
*------------------------------------------
*
* 14 -             ------------  TX to RX (pin 2) on RoMeo BLE
* 15 -             ------------  RX from TX (pin 3) on RoMeo BLE
* 16 -
* 17 -
* 18 -             ------------  TX to RX pin on Adafruit SoundFX board                   
* 19 -             ------------  RX from TX pin on Adafruit SoundFX board
* 20 -             
* 21 -               
*/


/*
Edit C:\Users\robcu\AppData\Local\Arduino15\packages\arduino\hardware\sam\1.6.12\variants\arduino_due_x
Change:
#define PWM_FREQUENCY		8000
#define TC_FREQUENCY    8000
*/


#include <PS3BT.h>
#include <usbhub.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>
#include "CytronMotorDriver.h"
//#include <SoftwareSerial.h>
#include "Adafruit_Soundboard.h"
#include <SerialCommands.h>

USB Usb;
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
USBHub Hub1(&Usb);
PS3BT PS3(&Btd); // This will just create the instance

// RomeoBLE PWM DC control
CytronMD dome(PWM_DIR, 3, 2); 
#define DOME_SPEED 255

// Leg Motors
CytronMD motor1(PWM_DIR, 5, 4); 
CytronMD motor2(PWM_DIR, 6, 7); 
#define DRIVE_SPEED 100
#define TURN_SPEED 100

// Audio board
#define SFX_RST 53
//#define VOL 255
Adafruit_Soundboard sfx = Adafruit_Soundboard(&Serial1, NULL, SFX_RST);



void setup() {
  halt();

  Serial.begin(115200);
  Serial.println("R2D2 starting ...");

#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.println("OSC did not start");
    while (1); //halt
  }
  Serial.println("PS3 Bluetooth Library started");

  Serial.println("Starting host serial communications");
  
  Serial1.begin(9600); // audio
  //Serial2.begin(9600); // dome
  Serial3.begin(9600); // arms

  //setPwmFrequency(5, 8);
  //setPwmFrequency(6, 8);

  if (!sfx.reset()) {
    Serial.println("Audio board not found");
  }
  else {
    Serial.println("Audio board found");
    sfx.playTrack((uint8_t)0);
  }
  
  /*Serial.println("Setting volume on audio board");
  uint8_t vol = sfx.volDown();
  if (vol < VOL)
  {
    while (vol < VOL) {
      vol = sfx.volUp();
    } 
  }
  else 
  {
    while (vol > VOL) {
      vol=sfx.volDown();
    } 
  }
  
  Serial.print("Volume: "); Serial.println(vol);*/
  Serial.println("R2D2 started");
}

void halt(void) 
{
  domeStop();
  driveStop();
}

boolean isPanelOpen = false;

void body_actions() {
  if (PS3.getAnalogHat(LeftHatX) > 200 || PS3.getAnalogHat(LeftHatX) < 50 || PS3.getAnalogHat(LeftHatY) > 200 || PS3.getAnalogHat(LeftHatY) < 50)
  {  
    if (PS3.getAnalogHat(LeftHatX) > 200)
    {
      Serial.println("Dome right");
      domeRight();
    }
    else if (PS3.getAnalogHat(LeftHatX) < 50)
    {
      Serial.println("Dome left");
      domeLeft();
    }
  }

  if (PS3.getButtonClick(CROSS))
  {
    Serial.println("Beeps");
    sfx.playTrack((uint8_t)2);
  }
  else if (PS3.getButtonClick(UP)) {
        Serial.println("Upper Arm");
        Serial3.write("AT+UPPERARM\r\n");           
    }
    else if (PS3.getButtonClick(DOWN)) {
        Serial.println("Lower Arm");
        Serial3.write("AT+LOWERARM\r\n"); 
    }
    else if (PS3.getButtonClick(LEFT)) {
        Serial.println("Scomp");
        Serial3.write("AT+SCOMP\r\n"); 
    }
    else if (PS3.getButtonClick(RIGHT)) {
        Serial.println("Grabber");
        Serial3.write("AT+GRABBER\r\n"); 
    }
}

void dome_tool_actions() {

/*
 if (PS3.getButtonClick(UP))
    {
      if (L1Pressed)
      {
        Serial.println("Scanner Up");
        Serial2.println("SU");  
      }
      else
      {
        Serial.println("Periscope Up");
        Serial2.println("PU");
      }
    }
  if (PS3.getButtonClick(DOWN))
    {
      if (L1Pressed)
      {
        Serial.println("Scanner Down");
        Serial2.println("SD");  
      }
      else
      {
        Serial.println("Periscope Down");
        Serial2.println("PD");
      }
    }
    */
/// Driving
    /*
      else if (L1Pressed && PS3.getAnalogHat(LeftHatY) < 50) {
        if (!isPanelOpen)
        {
          Serial.println("Panel open");
          Serial2.println("PO");
          isPanelOpen = true;
        }
      }
      else if (L1Pressed && PS3.getAnalogHat(LeftHatY) > 200) {
        if (isPanelOpen)
        {
          Serial.println("Panel close");
          Serial2.println("PC");
          isPanelOpen = false;
        }
      }*/
}

void driving_actions() {
  if (PS3.getAnalogHat(LeftHatX) > 200 || PS3.getAnalogHat(LeftHatX) < 50 || PS3.getAnalogHat(LeftHatY) > 200 || PS3.getAnalogHat(LeftHatY) < 50) {
    if (PS3.getAnalogHat(LeftHatX) > 200) {
      Serial.println("Turn right");
      Serial.print(PS3.getAnalogHat(LeftHatX));
      Serial.print(PS3.getAnalogHat(LeftHatY));   
      turnRight();
    }
    else if (PS3.getAnalogHat(LeftHatX) < 50) {
      Serial.println("Turn left");
      Serial.print(PS3.getAnalogHat(LeftHatX));
      Serial.print(PS3.getAnalogHat(LeftHatY));
      turnLeft();
    }
    else if (PS3.getAnalogHat(LeftHatY) > 50) {
      Serial.println("Drive backward");
      driveBackward();
    }
    else if (PS3.getAnalogHat(LeftHatY) < 200) {
      Serial.println("Drive forward");
      driveForward();
    }
  }
}

void loop()
{
  Usb.Task();
  
  if (PS3.PS3Connected || PS3.PS3NavigationConnected)
  {
    boolean L1Pressed = PS3.getAnalogButton(L1) > 100 ? true : false;
    boolean L2Pressed = PS3.getAnalogButton(L2) > 100 ? true : false;
 
    if (PS3.getButtonClick(PS))
    {
      Serial.println("Controller disconnect");
      PS3.disconnect();
      halt();
      return;
    }

    if (L1Pressed) {
      // dome actions
      dome_tool_actions();
    }
    else if (L2Pressed) {
      // driving actions
      driving_actions();
    }
    else {
      // body actions
      body_actions();
    }
  }
}

/* Dome control */
void domeStop(void)                    
{
  dome.setSpeed(0);
}

void domeLeft()
{
  dome.setSpeed(DOME_SPEED * -1);
}

void domeRight()
{
  dome.setSpeed(DOME_SPEED);
}

/* Drive Control */
void driveStop(void)               
{
  motor1.setSpeed(0);
  motor2.setSpeed(0);
}

void turnLeft()
{
  motor1.setSpeed(TURN_SPEED * -1);   // Motor 1 runs backward
  motor2.setSpeed(TURN_SPEED);  // Motor 2 runs forward
}

void turnRight()          
{
  motor1.setSpeed(TURN_SPEED);  // Motor 1 runs forward
  motor2.setSpeed(TURN_SPEED * -1);   // Motor 2 runs backward
}

void driveForward()  
{
  motor1.setSpeed(DRIVE_SPEED);  
  motor2.setSpeed(DRIVE_SPEED);  
}

void driveBackward()       
{
  motor1.setSpeed(DRIVE_SPEED * -1);  
  motor2.setSpeed(DRIVE_SPEED * -1);  
}

