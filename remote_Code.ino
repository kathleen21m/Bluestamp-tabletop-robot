
const int FB = 9;              // the number of the forward pushbutton pin
const int BB = 4;              // the number of the backward pushbutton pin
const int LB = 7;             // the number of the left pushbutton pin
const int RB = 6;             // the number of the right pushbutton pin
const int autoB = 8;          // the number of the autonomous pushbutton pin 
const int stopAutoB = 5;      // the number of the stop autonomous pushbutton pin


// variables will change:
int FBState = 0;         // variable for reading the statuses of the pushbuttons
int BBState = 0;
int LBState = 0;
int RBState = 0;
int autoBState = 0;
int stopAutoBState = 0;
char val; //will be set if voice control commands are sent

void setup() {
  //begin serial moniter and XBee moniter
  Serial1.begin(9600);
  Serial.begin(9600);

  // initialize the pushbutton pins as an inputs:
  pinMode(FB, INPUT);
  pinMode(BB, INPUT);
  pinMode(LB, INPUT);
  pinMode(RB, INPUT);
  pinMode(autoB, INPUT);
  pinMode(stopAutoB, INPUT);
}

void loop() {
  // read the state of the pushbutton values:
  FBState = digitalRead(FB);
  BBState = digitalRead(BB);
  LBState = digitalRead(LB);
  RBState = digitalRead(RB);
  autoBState = digitalRead(autoB);
  stopAutoBState = digitalRead(stopAutoB);

  // check if the any of the pushbuttons are pressed.
  // if one is, the buttonState is HIGH:
  //if a button is pressed send the letter that corresponds to that
 // direction in the robot code to the XBee on the robot
  if (FBState == HIGH) {
    Serial1.write('u');
    Serial.write('u'); //prints value on the serial moniter and allows you to
                       //check that correct values are being sent
  } 
  else if (BBState == HIGH) {
    Serial1.write('j');
    Serial.write('j');
  } 
  else if (LBState == HIGH) {
    Serial1.write('h');
    Serial.write('h');
  } 
  else if (RBState == HIGH) {
    Serial1.write('k');
    Serial.write('k');
  } 
  else if (autoBState == HIGH) {
    Serial1.write('a');
    Serial.write('a');
  } 
  else if (stopAutoBState == HIGH) {
    Serial1.write('s');
    Serial.write('s');
  } 

  if (Serial.available() > 0) { //if voice control commands are sent they will be sent over the serial
  //moniter. If a command has been sent through voice control this will make the robot go that direction
  // for a certain amount of time
    val = Serial.read();
    if ((val == 'u') || (val == 'j') || (val == 'k') || (val == 'h') || (val == 'a') || (val == 's') )  { 
      for (int i = 0; i <500; i++) {
        Serial1.write(val);
        delay(1);
      }

    }   

  }
  delay(85); //delay between button readings
}

