

#include <Servo.h>

Servo servoLeft;  // Define left servo
Servo servoRight;// Define right servo
int incomingByte;
int previousByte;

int IRpinRF = A1;               // IR detector on analog pin A1 (on the right front of the robot)
int IRemitterRF = 4;            // IR emitter LED on digital pin 4 (also on right front)
int IRpinLF= A0;                //detector on left front of robot
int IRemitterLF= 2;             //emitter on right front of robot
int IRpinLB = A2;               //detector left back
int IRemitterLB = 7;            //emitter left back
int IRpinRB = A3;              //detector right back
int IRemitterRB = 8;           //emitter right back

int ambientIR;                // variable to store the IR coming from the ambient
int obstacleIR;               // variable to store the IR coming from the object
int value[10];                // variable to store the IR values
int distanceLF;               //variable that will tell if there is an obstacle or not on left front
int distanceRF;               //if obstacle on right front
int distanceLB;              //if obstacle on left back
int distanceRB;              //if obstacle on right back

int thresholdDF;              //threshold distances on the front to tell if there is a table or not
int thresholdDBR;             //threshold distances for back right
int thresholdDBL;             // threshold distances for back left
int driveDelay;              //delay that occurs once direction has been determined

void setup(){
  servoLeft.attach(9);  // Set left servo to digital pin 10
  servoRight.attach(10);  // Set right servo to digital pin 9
  Serial.begin(9600);     // initializing Serial monitor
  Serial1.begin(9600);   //initialize serial moniter for XBees
  pinMode(IRemitterRF,OUTPUT);   //set emitter for right front as output
  digitalWrite(IRemitterRF,LOW);// setup IR LED as off

  pinMode(IRemitterLF,OUTPUT);  
  digitalWrite(IRemitterLF,LOW);
  
  pinMode(IRemitterRB,OUTPUT);  
  digitalWrite(IRemitterRB,LOW);

  pinMode(IRemitterLB,OUTPUT);  
  digitalWrite(IRemitterLB,LOW); 

  thresholdDF = 5; //threshold distance from table for front
  thresholdDBR = 45;
  thresholdDBL = 90;
  driveDelay = 100; //delay before checking sensors again
  incomingByte = 'q'; //variable for remote function
}

void loop(){
  calculateDistances(); //calculates the distances from each sensor
  if ((distanceLF < thresholdDF) || (distanceRF < thresholdDF) || (distanceLB < thresholdDBL) || (distanceRB < thresholdDBR)) {  //if any of the sensors does not see the table the sensors should override remote control commands
    sensorOverride();
  } else if (Serial1.available() > 0) { //if a signal has been sent from the remote store it in incoming byte
    previousByte = incomingByte;
    incomingByte = Serial1.read();
    XBeeControl(incomingByte); //control robot based on what command was sent from remote
  } else {
    Stop(); //if sensors are all on table and no commands are being sent the robot should be stopped
    incomingByte = 'q';
  }
  
}

void calculateDistances() { //calculates the distance from each sensor
  distanceLF = readIR(5, IRpinLF, IRemitterLF, distanceLF);   
  distanceRF = readIR(5, IRpinRF, IRemitterRF, distanceRF); 
  distanceLB = readIR(5, IRpinLB, IRemitterLB, distanceLB); 
  distanceRB = readIR(5, IRpinRB, IRemitterRB, distanceRB); 
}

int readIR(int times, int IRpin, int IRemitter, int distance){ //calculates the distances from one emitter/detector pair
  for(int x=0;x<times;x++){     
    digitalWrite(IRemitter,LOW);           // turning the IR LEDs off to read the IR coming from the ambient
    delay(1);                                             // minimum delay necessary to read values
    ambientIR = analogRead(IRpin);  // storing IR coming from the ambient
    digitalWrite(IRemitter,HIGH);          // turning the IR LEDs on to read the IR coming from the obstacle
    delay(1);                                             // minimum delay necessary to read values
    obstacleIR = analogRead(IRpin);  // storing IR coming from the obstacle
    value[x] = ambientIR-obstacleIR;   // calculating changes in IR values and storing it for future average
  }
 
  for(int x=0;x<times;x++){        // calculating the average based on the "accuracy"
    distance+=value[x];
  }
  return(distance/times);            // return the final value
}

void XBeeControl(int incomingByte) { //controls the robot based on the command from the robot
      if (incomingByte == 'u')  {
        forward();
        delay(driveDelay/2);
      
      }
      else if (incomingByte == 'j') {
        backward();
        delay(driveDelay/2);
      
      }
      else if (incomingByte == 'h') {
       left();
        delay(driveDelay/2);
      }
      else if (incomingByte == 'k') {
        right();
        delay(driveDelay/2);
      }
      else if (incomingByte == 'a') { //if the start autnomous button is pressed, drive autnomously until the stop autnomous button is pressed
        while (incomingByte != 's') {
          calculateDistances();
          autoDrive();
          incomingByte = Serial1.read();
        }
    } else {
        Stop();
    }
    
}



void autoDrive() { //drives autnomously using the sensors if the autonomous button is pressed
    if ((distanceLF < thresholdDF) && (distanceRF < thresholdDF)) {
    reverse();
    delay(driveDelay*3);
  } else if (distanceLF < thresholdDF) {
    reverseL();
    delay(driveDelay*3);
  } else if(distanceRF < thresholdDF) {
    reverseR();
    delay(driveDelay*3); 
  } else if ((distanceLB < thresholdDBL) && (distanceRB < thresholdDBR)) {
    forward();
    delay(driveDelay*3);
  } else if (distanceLB < thresholdDBL) {
    forwardL();
    delay(driveDelay*3);
  } else if(distanceRB < thresholdDBR) {
    forwardR();
    delay(driveDelay*3); 
  } else {
    forward();
    delay(driveDelay/2);
  } 
}


void sensorOverride() { 
  /*this is called if the robot is in remote control mode and a 
   *IR emitter and detector pair don't sense the table. This is different 
   * from the auto drive because the default is stop instead of driving forward */
  if ((distanceLF < thresholdDF) && (distanceRF < thresholdDF)) {
    reverse();
    delay(driveDelay*3);
  } else if (distanceLF < thresholdDF) {
    reverseL();
    delay(driveDelay*3);
  } else if(distanceRF < thresholdDF) {
    reverseR();
    delay(driveDelay*3); 
  } else if ((distanceLB < thresholdDBL) && (distanceRB < thresholdDBR)) {
    forward();
    delay(driveDelay*3);
  } else if (distanceLB < thresholdDBL) {
    forwardL();
    delay(driveDelay*3);
  } else if(distanceRB < thresholdDBR) {
    forwardR();
    delay(driveDelay*3); 
  }
}

void forward() { //sets servo speeds to drive robot forward
  servoLeft.write(70);
  servoRight.write(110);
}

void reverse() { //drives robot in reverse
  servoLeft.write(110);
  servoRight.write(70);
  
}

void reverseR() { //use when right sensor no longer senses table
  servoRight.write(90);
  servoLeft.write(180);
  
}

void reverseL() { //uses when left front sensor no longer sense table
  servoRight.write(0);
  servoLeft.write(90);
  
}

void forwardR() { //when right back sensor doesn't see table
  servoRight.write(180);
  servoLeft.write(90);
}

void forwardL() { //when left back sensor doesn't see table
  servoRight.write(90);
  servoLeft.write(0);
}

void Stop() { // stops robot
  servoRight.write(90);
  servoLeft.write(90);
}




