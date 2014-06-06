/*
SparkFun Reel Cutter Prototype
Code V1.0 (April 3rd, 2014)
by: Matt McCarty
*/

#include <AccelStepper.h> //stepper motor library

int motorSpeed = 9600; //maximum steps per second (about 3rps / at 16 microsteps)
int motorAccel = 80000; //steps/second/second to accelerate

int motorDirPin = 11; //digital pin 11
int motorStepPin = 12; //digital pin 12

AccelStepper stepper(1, motorStepPin, motorDirPin); //set up stepper motor object (1 indicates using a driver)

#include <Servo.h> //servo motor library

Servo servA; //create servo objects
Servo servB;

int pos = 0; //create value for servo initial positions

const int clrPin = 4; //Clear value button pin
const int entPin = 5; //Enter values button pin
const int cycPin = 6; //Cycle selection button pin
const int decPin = 7; //Decrement value button pin
const int incPin = 8; //Increment value button pin

const int ledPin = 13; //test led

int clrState = 0; //buttonRead initial states
int entState = 0;
int cycState = 0;
int decState = 0;
int incState = 0;

void setup()
{ 
  Serial.begin(9600); // set up serial port for 9600 baud
  delay(500); // wait for display to boot up
  
  pinMode(clrPin, INPUT); //set pin modes for buttons
  pinMode(entPin, INPUT);
  pinMode(cycPin, INPUT);
  pinMode(decPin, INPUT);
  pinMode(incPin, INPUT);
  
  pinMode(ledPin, OUTPUT); // set test led to output
  
  stepper.setMaxSpeed(motorSpeed); //set up stepper motor settings
  stepper.setSpeed(motorSpeed);
  stepper.setAcceleration(motorAccel);
  
  servA.attach(9); //attach servos to pins 9 and 10
  servB.attach(10);
} 

int strip = 0; //initialize variables used later
int num = 0;
int cyc = 0;
char stripstring[8], numstring[8]; // create string arrays

void loop()
{ 
  servA.write(pos);
  servB.write(180 - pos);
  
  clrState = digitalRead(clrPin); //read state of Clear button
  if(clrState == HIGH){ // If button is pressed reset both values and display
    strip = 0;
    num = 0;
    dispUI(strip,num);
  }
  
  entState = digitalRead(entPin); //read state of Enter button
  if(entState == HIGH){ // If button is pressed initiate cut() function
    cut(strip,num);
  }
  
  cycState = digitalRead(cycPin); //read state of cycle button
  if((cycState == HIGH)&&(cyc == 0)){ //if on "strips" switch to "of"
    cyc = 1;
    delay(500);
  }
  else if((cycState == HIGH)&&(cyc == 1)){ //if on "of" switch to "strips"
    cyc = 0;
    delay(500);
  }
  
  decState = digitalRead(decPin); // read state of Decrement button
  if((decState == HIGH)&&(cyc == 0)&&(strip > 0)){ //if on "strips" and value is positive decrement strip value
    --strip;
    dispUI(strip,num);
  }
  else if((decState == HIGH)&&(cyc == 1)&&(num > 0)){ //if on "of" and value is positive decrement of value
    --num;
    dispUI(strip,num);
  }
  
  incState = digitalRead(incPin); // read state of Increment button
  if((incState == HIGH)&&(cyc == 0)&&(strip < 9999)){ //if on "strips" and value is less than upper limit increment strip value
    ++strip;
    dispUI(strip,num);
  }
  else if((incState == HIGH)&&(cyc == 1)&&(num < 999)){ //if on "of" and value is less than upper limit, increment of value
    ++num;
    dispUI(strip,num);
  }
  
}

void dispUI(int strip,int num) //Display function
{ 
  Serial.write(254); // cursor to beginning of first line
  Serial.write(128);

  Serial.write("STRIPS:         "); // clear display + legends
  Serial.write("OF:             ");
  
  sprintf(stripstring,"%4d",strip); // create strings from the numbers
  sprintf(numstring,"%4d",num); // right-justify to 4 spaces

  Serial.write(254); // cursor to 9th position on first line
  Serial.write(136);
 
  Serial.write(stripstring); // write out the strip value

  Serial.write(254); // cursor to 9th position on second line
  Serial.write(200);

  Serial.write(numstring); // write out the num value
  
  delay(150); // short delay
}

void cut(int strip,int num) //Cut initiation function
{
  Serial.write(254); // cursor to beginning of first line
  Serial.write(128);
  
  Serial.write("CUT      OF    ?");
  Serial.write("ENTER TO CONFIRM");
  
  sprintf(stripstring,"%4d",strip); // create strings from the numbers
  sprintf(numstring,"%4d",num); // right-justify to 4 spaces
  
  Serial.write(254); // cursor to 5th position on first line
  Serial.write(132);
 
  Serial.write(stripstring); // write out the Strip value
  
  Serial.write(254); // cursor to 12th position on first line
  Serial.write(139);
 
  Serial.write(numstring); // write out the num value
  
  int stripn = strip;
  
  delay(500);
  
  entState = digitalRead(entPin); //read state of Enter button
  if(entState == HIGH){       
    Serial.write(254); // cursor to beginning of first line
    Serial.write(128);
    
    Serial.write("   CUTTING...   ");
    Serial.write("                ");
    
    delay(1000);
    
    for(stripn; stripn > 0; stripn--){
      stepper.move(num*160);
      while(stepper.distanceToGo() != 0){
        stepper.run();
      }
      digitalWrite(ledPin, HIGH);
      servocut();
      digitalWrite(ledPin, LOW);
      }
    
    delay(2000);    
    
    Serial.write(254); // cursor to beginning of first line
    Serial.write(128);
    
    Serial.write("     DONE!      ");
    Serial.write("                ");
    
    delay(5000);
    
    dispUI(strip,num);
  }
  
  clrState = digitalRead(clrPin); //read state of Clear button
  if(clrState == HIGH){
    dispUI(strip,num);
  }
}

void servocut() //actual cutting function
{
  for(pos = 0; pos < 89; pos++){
    servA.write(pos);
    delay(5);
    servB.write(180 - pos);
    delay(5);
  }
  delay(100);
  for(pos = 89; pos >= 0; pos--){
    servA.write(pos);
    delay(5);
    servB.write(180 - pos);
    delay(5);
  }
  delay(1000);
}
