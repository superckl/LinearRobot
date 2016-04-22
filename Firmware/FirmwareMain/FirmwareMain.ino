/*
 * Firmware for the Gilroy Lab Linear Robot.
 */
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2);

const double STEPS_PER_CM = 1300;
const int SWITCH_PIN = 8;
const int FOCUS_PIN = 6;
const int SHUTTER_PIN = 7;

const int NUM_PLATES = 6;

void setup() {
  Serial.begin(9600);
  AFMS.begin();
  myMotor->setSpeed(150);
  pinMode(SWITCH_PIN, INPUT);
  pinMode(FOCUS_PIN, OUTPUT);
  pinMode(SHUTTER_PIN, OUTPUT);
  digitalWrite(FOCUS_PIN, LOW);
  digitalWrite(SHUTTER_PIN, LOW);
}

void loop() {
  if (Serial.available() > 0){
    for(int i=0; i<10; i++){
      Serial.print("Received\n");
    }
    String incoming_string = Serial.readString();
    //if statement to handle all of the commands
    if(incoming_string.equals("Marco!"))
      call_response();
    else if(incoming_string.charAt(0) == 'm')
      move_bot(incoming_string);
    else if(incoming_string.equals("home"))
      go_home();
    else if(incoming_string.equals("runExp"))
      runExp();
  } 
  myMotor->release();
}

void call_response(){
  for(int i=0; i<100; i++){
    Serial.print("Polo!\n");
  }
}

void move_bot(String input){
  String len = input.substring(1, input.length());
  Serial.println(len);
  double move_len = (double)len.toFloat();
  Serial.println(move_len);
  Serial.println(move_len*STEPS_PER_CM);
  double move_length = move_len*STEPS_PER_CM;
  Serial.println(move_length);
  if (move_length > 0){
    while(move_length > 0){
      myMotor->step(1, BACKWARD, SINGLE);
      move_length--;
    }
  }else{
    move_length = abs(move_length);
    while(digitalRead(SWITCH_PIN) != LOW && move_length > 0){
      myMotor->step(1, FORWARD, SINGLE); 
      move_length--;
    } 
  }
  if(digitalRead(SWITCH_PIN) == LOW)
    for(int i=0; i<100; i++){
      Serial.print("home\n");
    }
  else 
    for(int i=0; i<100; i++){
      Serial.print("done\n");
    }
}

void go_home(){
  while(digitalRead(SWITCH_PIN) != LOW){
    myMotor->step(10, FORWARD, SINGLE);
  }
  for(int i=0; i<100; i++){
    Serial.print("home\n");
  }
}

void runExp(){
  go_home();
  delay(100);
  takePicture();
  for(int i = 0; i < NUM_PLATES - 1; i++){
    move_bot("m30.48");
    delay(100);
    takePicture();
  }
  go_home();
}

void takePicture(){
  //Focus shutter
  digitalWrite(FOCUS_PIN, HIGH);
  delay(800); // May want to adjust this depending on focus time
  digitalWrite(FOCUS_PIN, LOW);
  delay(100);
  //Take picture
  digitalWrite(SHUTTER_PIN, HIGH);
  delay(2000); // May want to adjust this depending on shot type
  digitalWrite(SHUTTER_PIN, LOW);
  delay(100);
}
