/*
Robert Culyer
October 2023


R2D2 Arms controller
Uses DFRobot RoMeo BLE board
Servo 7 controls the upper arm
Servo 8 controls the lower arm
Servo 9 controls the SCOMP
Servo 10 controls the SCOMP door
Motor 1 drives the SCOMP linear actuator
Servo 11 controls the grabber
Servo 12 controls the grabber door 

Interface from the main controller (see r2d2_master) uses AT command set
*/

#include <Servo.h>     
#include <SoftwareSerial.h>
#include <SerialCommand.h>

SoftwareSerial HostController(2, 3); // RX, TX
SerialCommand HostCommand;

Servo servoUpperArm;    
int SERVO_UPPERARM = 7;
int UPPERARM_OUT = 0;
int UPPERARM_IN = 170;

Servo servoLowerArm;       
int SERVO_LOWERARM = 8;
int LOWERARM_OUT = 0;
int LOWERARM_IN = 170;

Servo servoGrabberArmDoor;       
Servo servoGrabberArm; 

Servo servoScompArmDoor;       
Servo servoScompArm;     
int SERVO_SCOMPARM = 9;
int SCOMP_DOWN = 150;
int SCOMP_UP = 0;
int E1 = 5;     //M1 Speed Control
int M1 = 4;     //M1 Direction Control

bool is_upperarm_open = false;
bool is_lowerarm_open = false;
bool is_scomp_out = false;
bool is grabber_out = false;

// *************************************
// UPPER ARM
// *************************************
void close_upperarm() {
  servoUpperArm.attach(SERVO_UPPERARM); 
  Serial.println("UPPERARM in");
  servoUpperArm.write(UPPERARM_IN);
   is_upperarm_open = false;
}
void open_upperarm() {
  Serial.println("UPPERARM out");
  servoUpperArm.write(UPPERARM_OUT);
  is_upperarm_open = true;
}

// *************************************
// LOWER ARM
// *************************************
void close_lowerarm() {
  Serial.println("LOWERARM in");
  servoLowerArm.write(LOWERARM_IN);
   is_lowerarm_open = false;
}
void open_lowerarm() {
  Serial.println("LOWERARM out");
  servoLowerArm.write(LOWERARM_OUT);
  is_lowerarm_open = true;
}

// *************************************
// SCOMP
// *************************************
void close_scomp() {
  Serial.println("SCOMP in");
  scomp_in();
  close_scomp_door();
  is_scomp_out = false;
}

void close_scomp_door() {

}

void open_scomp_door() {

}

void open_scomp() {
  Serial.println("SCOMP out");
  open_scomp_door();
  scomp_out();
  is_scomp_out = true;
}

void scomp_in() {
  analogWrite (E1,255);
  digitalWrite(M1,LOW);
  delay(5000);
  servoScompArm.write(SCOMP_DOWN);
  delay(2000);  
}

void scomp_out() {
  servoScompArm.write(SCOMP_UP);
  delay(2000);
  analogWrite (E1,255);      //PWM Speed Control
  digitalWrite(M1,HIGH);
  delay(2200);
  Serial.println("scomp stop");
  digitalWrite(E1,LOW);
}

// *************************************
// Grabber
// *************************************
void close_grabber() {
  Serial.println("GRABBER in");
  is_grabber_out = false;
}

void open_grabber() {
  Serial.println("GRABBER out");
  is_grabber_out = true;
}

void setup() { 
  Serial.begin(115200);
  HostController.begin(9600); // from main controller
  HostCommand.addExecuteCommand((char *)"AT+UPPERARM", &upperarm_handler);
  HostCommand.addExecuteCommand((char *)"AT+LOWERARM", &lowerarm_handler);
  HostCommand.addExecuteCommand((char *)"AT+SCOMP", &scomp_handler);
  HostCommand.addExecuteCommand((char *)"AT+GRABBER", &grabber_handler);
  HostCommand.begin(HostController);
  
  servoUpperArm.attach(SERVO_UPPERARM); 
  servoLowerArm.attach(SERVO_LOWERARM); 
  servoScompArm.attach(SERVO_SCOMPARM); 

  Serial.println("Setting arms to default position");
  close_upperarm();
  close_lowerarm();
  close_scomp();
  close_grabber();
}

void upperarm_handler()
{
    if (is_upperarm_open) close_upperarm();
    else open_upperarm();
}

void lowerarm_handler()
{
    if (is_lowerarm_open) close_lowerarm();
    else open_lowerarm();
}

void scomp_handler()
{
    if (is_scomp_out) close_scomp();
    else open_scomp();
}

void grabber_handler()
{
    if (is_grabber_out) close_grabber();
    else open_grabber(); 
}

void loop() {
  HostCommand.loop();
}
