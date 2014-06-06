/*
SparkFun Reel Cutter Prototype
Code V1.0 (April 3rd, 2014)
by: Matt McCarty

V1.1 By Erik Josh 
  Edited for code readability
  Added interrupt functionality for safety
  Re-assigned pins to match hardware layout
*/

#include <AccelStepper.h> //stepper motor library
#include <Servo.h> //servo motor library

const int clrPin = 2; //Clear value button pin
const int entPin = 3; //Enter values button pin
const int cycPin = 12; //Cycle selection button pin
const int decPin = 5; //Decrement value button pin
const int incPin = 7; //Increment value button pin

const int servPinA = 10;
const int servPinB = 11;

const int motorDirPin = 9; 
const int motorStepPin = 8; 
const int motorSpeed = 9600; //maximum steps per second (about 3rps / at 16 microsteps)
const int motorAccel = 80000; //steps/second/second to accelerate

AccelStepper stepper(1, motorStepPin, motorDirPin); //set up stepper motor object (1 indicates using a driver)
Servo servA; //create servo objects
Servo servB;

int pos = 0; //create value for servo initial positions
int strip = 0; //initialize variables used later
int num = 0;
boolean cyc = false;
boolean errFlag = false;
char stripString[8];
char numString[8]; // create string arrays

void setup(){
	Serial.begin(9600); // set up serial port for 9600 baud
	delay(500); // wait for display to boot up

	pinMode(clrPin, INPUT); //set pin modes for buttons
	pinMode(entPin, INPUT);
	pinMode(cycPin, INPUT);
	pinMode(decPin, INPUT);
	pinMode(incPin, INPUT);


	stepper.setMaxSpeed(motorSpeed); //set up stepper motor settings
	stepper.setSpeed(motorSpeed);
	stepper.setAcceleration(motorAccel);

	servA.attach(servPinA);
	servB.attach(servPinB);
	servA.write(pos);
	servB.write(180 - pos);
}

void problemMode(){
	Serial.write("ERROR           ");
	Serial.write("ERROR           ");
	while(!digitalRead(entPin)){;}
        errFlag = false;
	Serial.write("   CUTTING...   ");
	Serial.write("                ");
}

void errState(){
	errFlag = true;
	return;
}

void updateUI(){
	Serial.write(254); // cursor to beginning of first line
	Serial.write(128);

	Serial.write("STRIPS:         "); // clear display + legends
	Serial.write("OF:             ");

	sprintf(stripString,"%4d",strip); // create strings from the numbers
	sprintf(numString,"%4d",num); // right-justify to 4 spaces

	Serial.write(254); // cursor to 9th position on first line
	Serial.write(136);

	Serial.write(stripString); // write out the strip value

	Serial.write(254); // cursor to 9th position on second line
	Serial.write(200);

	Serial.write(numString); // write out the num value

	delay(50); // short delay
}

void cut(){
	for(pos = 0; pos < 89; pos++){
		servA.write(pos);
		delay(5);
		servB.write(180 - pos);
		delay(5);
		if(errFlag){
			problemMode();
		}
	}
	delay(100);
	strip--;
	for(pos = 89; pos >= 0; pos--){
		servA.write(pos);
		delay(5);
		servB.write(180 - pos);
		delay(5);
		if(errFlag){
			problemMode();
		}
	}
	delay(1000);
}

void feed(){
				stepper.move(num*-160);
			while(stepper.distanceToGo() != 0){
				stepper.run();
				if(errFlag){
					problemMode();
				}
			}
}

void cutCycle(){
	attachInterrupt(0, errState, RISING);
	while(strip){
		feed();
		cut();
	}
        detachInterrupt(1);
}

void validateCut(){
	Serial.write(254); // cursor to beginning of first line
	Serial.write(128);

	Serial.write("CUT      OF    ?");
	Serial.write("ENTER TO CONFIRM");

	sprintf(stripString,"%4d",strip); // create strings from the numbers
	sprintf(numString,"%4d",num); // right-justify to 4 spaces

	Serial.write(254); // cursor to 5th position on first line
	Serial.write(132);

	Serial.write(stripString); // write out the Strip value

	Serial.write(254); // cursor to 12th position on first line
	Serial.write(139);

	Serial.write(numString); // write out the num value


	delay(500);
	
	while(!(digitalRead(clrPin) || digitalRead(entPin))){;}

	if(digitalRead(entPin)){
		Serial.write(254); // cursor to beginning of first line
		Serial.write(128);

		Serial.write("   CUTTING...   ");
		Serial.write("                ");

		delay(500);

		cutCycle();

		delay(1000);

		Serial.write(254); // cursor to beginning of first line
		Serial.write(128);

		Serial.write("     DONE!      ");
		Serial.write("                ");

		delay(5000);

		updateUI();
	}

	if(digitalRead(clrPin)){
		updateUI();
                while(digitalRead(clrPin)){;}
	}
}

void loop(){

	if(digitalRead(clrPin) ){ // If button is pressed reset both values and display
		strip = 0;
		num = 0;
		updateUI();
		while(digitalRead(clrPin)){;}
	}
	if(digitalRead(entPin) ){ // If button is pressed initiate cut() function
		while(digitalRead(entPin)){;}
		validateCut();
	}
	if(digitalRead(cycPin)){ //if on "strips" switch to "of"
		cyc = !cyc;
		while(digitalRead(cycPin)){;}
	}
	if(digitalRead(decPin)){
		for(int shift = 15; digitalRead(decPin); shift--){
			if(cyc && num){
				num--;
			}else if (strip){
				strip--;
			}
			updateUI();
			if(digitalRead(decPin)){
				delay((shift/3+1)*20);
			}
			if(!shift) {shift++;}
		}
	}
	if(digitalRead(incPin)){
		for(int shift = 15; digitalRead(incPin); shift--){
			if(cyc && num<=9999){
				num++;
			}else if (strip<=999){
				strip++;
			}
			updateUI();
			if(digitalRead(incPin)){
				delay((shift/3+1)*20);
			}
			if(!shift) {shift++;}
			}
		}
	}



