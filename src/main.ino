#include <Servo.h>
// ramp lib
#include <Ramp.h> 

#define ir 4

Servo servo1;
Servo servo2;
Servo servo3;

int servo1Offset = 2150;      // gripper closed - max 2200
int servo2Offset = 1120;      // shoulder back - min 600  (1120 mid)
int servo3Offset = 1180;      // elbow back - max 2180 (1180 mid)

// moving the foot forwards or backwards in the side plane
float shoulderAngle2;
float shoulderAngle2a;
float shoulderAngle2Degrees;
float shoulderAngle2aDegrees;
float z2;
float x;

// side plane of individual leg only
#define lowerLength 110     
#define upperLength 110
float armLength;
float z;
float shoulderAngle1;
float shoulderAngleDegrees;
float shoulderAngle1a;   
float elbowAngle;  
float elbowAngleDegrees;

// interpolation targets
float zTarget;
float xTarget;

// output scaling
float shoulderMs;
float shoulderMs2;
float elbowMs;

int ball;     // ball proximity

// wheel encoder interrupts
#define encoder0PinA 3      
#define encoder0PinB 2      

// PWM output pins 
#define in1 11
#define in2 10

volatile long encoder0Pos = 0;    // encoder 1
long encoder0Target = 0;

unsigned long currentMillis;
unsigned long previousMillis;

long previousStepMillis = 0;    // set up timers
int stepFlag = 0;

//*****************************
class Interpolation {  
public:
    rampInt myRamp;
    int interpolationFlag = 0;
    int savedValue;    

    int go(int input, int duration) {

      if (input != savedValue) {   // check for new data
          interpolationFlag = 0;
      }
      savedValue = input;          // bookmark the old value  
    
      if (interpolationFlag == 0) {                                        // only do it once until the flag is reset
          myRamp.go(input, duration, LINEAR, ONCEFORWARD);              // start interpolation (value to go to, duration)
          interpolationFlag = 1;
      }
    
      int output = myRamp.update();               
      return output;
    }
};    // end of class

Interpolation interpX;        // interpolation objects
Interpolation interpZ;
//*****************************

/**
 * @brief Initalisation of the servos and linear axis encoder/motor.
 *          1) Attaching the GPIO pins to the (commented) servos.
 *          2) Initialising an offset for each servo - the writeMicroseconds maps to an angle of the shaft 
 *          3) Setting the pin modes for the linear axis ENCODER(bigger motor) as input so we can read the signals.
 *          4) Setting the pin modes for the linear axis MOTOR(bigger motor) as output so we can move the motor.
 *          5) Setting the pin mode for the IR (infrared) Sensor so we can read it's input. 
 *          6) Attaching interupts to the ENCODER(see 3) to determine the direction it's going (clockwise OR counterclockwise) - TODO have a reference document to how this works
 *          7) Initialising default position for arm - this is fed into the interpolation class which will set SMOOTHLY control the arm. 
 */
void setup() 
{
  // (1)
  servo1.attach(5);         // gripper
  servo2.attach(6);         // shoulder
  servo3.attach(9);         // elbow

  // (2)
  servo1.writeMicroseconds(servo1Offset);    // gripper
  servo2.writeMicroseconds(servo2Offset);    // shoulder
  servo3.writeMicroseconds(servo3Offset);    // elbow

  // (3)
  pinMode(encoder0PinA, INPUT_PULLUP);    // encoder pins
  pinMode(encoder0PinB, INPUT_PULLUP);

  // (4)
  pinMode(in2, OUTPUT);         // PWM motor output piuns
  pinMode(in1, OUTPUT);

  // (5)
  pinMode(ir, INPUT_PULLUP);   // proximity sensor input

  // (6)
  // -- 0 and 1 are the interupt numbers NOT the digital pin number 
  attachInterrupt(0, doEncoderA, CHANGE); 
  attachInterrupt(1, doEncoderB, CHANGE);

  // DEBUGGING 
  Serial.begin(9600);

  // (7)
  zTarget = 100;      // set default position for arm
  xTarget = 50;
}

void loop() {

  // Get the current time in MILLISECONDS
  currentMillis = millis();

    //
    // This is the start of the state machine. Each IF statement condition is cycle 
    //  through each state while adding a time buffer between each state
    //
    if (currentMillis - previousMillis >= 10) {  // start timed loop after 10ms has passed 
          previousMillis = currentMillis;

          ball = digitalRead(4);      // detect ball the IR sensor on digital pin 4

          // step sequencer
          if (ball == 0 && stepFlag == 0) {
             Serial.println("Init - 0"); // debugging 
             stepFlag = 1;
          }
        
          //   (Step 1) - Open gripper
          if (stepFlag == 1 && currentMillis - previousStepMillis >= 1000) {
              Serial.println("Gripper Open - 1"); // debugging
              zTarget = -115;
              xTarget = 110;
              servo1.writeMicroseconds(servo1Offset -300);    // gripper open
              encoder0Target = 0;
              previousStepMillis = currentMillis;
              stepFlag = 2;
          } // (Step 2) - Close gripper AKA grab the ball
          else if (stepFlag == 2 && currentMillis - previousStepMillis >= 1200) {
              Serial.println("Gripper Closed - 2"); 
              servo1.writeMicroseconds(servo1Offset);    // gripper closed
              previousStepMillis = currentMillis;
              stepFlag = 3;                  
          } // (Step 3) - Start moving across the linear axis 
          else if (stepFlag == 3 && currentMillis - previousStepMillis >= 1000) {
              Serial.println(" Moving(Linear Axis) - 3 "); 
              zTarget = 100;
              xTarget = 50;
              previousStepMillis = currentMillis;
              stepFlag = 4; 
          } // (Step 4) - cont. 
          else if (stepFlag == 4 && currentMillis - previousStepMillis >= 500) {
              Serial.println(" Moving(Linear Axis) - 4 "); 
              encoder0Target = 20000;
              previousStepMillis = currentMillis;
              stepFlag = 6; 
          } 
          else if (stepFlag == 6 && currentMillis - previousStepMillis >= 4000) {
              Serial.println(" Moving(Linear Axis) - 6 "); 
              zTarget = 100;
              xTarget = 170;
              previousStepMillis = currentMillis;
              stepFlag = 7; 
          }
          else if (stepFlag == 7 && currentMillis - previousStepMillis >= 1500) {
              Serial.println(" Gripper Open - 7 "); 
              servo1.writeMicroseconds(servo1Offset - 300);    // gripper open
              stepFlag = 8; 
          }
          else if (stepFlag == 8 && currentMillis - previousStepMillis >= 2500) {
              Serial.println(" Gripper Closed - 8 "); 
              encoder0Target = 0;
              zTarget = 100;
              xTarget = 50;
              servo1.writeMicroseconds(servo1Offset);    // gripper closed
              previousStepMillis = currentMillis;
              stepFlag = 9; 
          }
          else if (stepFlag == 9 && currentMillis - previousStepMillis >= 4000) {
              //wait to get back to the start before triggering again
              Serial.println(" Moving(Linear Axis) - 9 "); 
              servo1.writeMicroseconds(servo1Offset);    // gripper closed
              previousStepMillis = currentMillis;
              stepFlag = 0; 
          }

          // end of step sequencer

          // start interpolation
          
          z = interpZ.go(zTarget,1000);
          x = interpX.go(xTarget,1000);

          // *** Inverse Kinematics ***
      
          // calculate modification to shoulder angle and arm length
      
          shoulderAngle2a = atan(z/x);
          shoulderAngle2aDegrees = shoulderAngle2a * (180/PI);    // degrees
          shoulderAngle2 = shoulderAngle2a - 0.7853908;  // take away the default 45' offset (in radians)
          shoulderAngle2Degrees = shoulderAngle2 * (180/PI);    // degrees
          shoulderMs2 = shoulderAngle2Degrees * 11;
          
          z2 = x/cos(shoulderAngle2a);     // calc new arm length to feed to the next bit of code below
      
          // ****************
      
          // calculate arm length based on upper/lower length and elbow and shoulder angle
          shoulderAngle1a = (sq(upperLength) + sq(z2) - sq(lowerLength)) / (2 * upperLength * z2);
          shoulderAngle1 = acos(shoulderAngle1a);     // radians
          elbowAngle = PI - (shoulderAngle1 *2);       // radians
      
          // calc degrees from angles
          shoulderAngleDegrees = shoulderAngle1 * (180/PI);    // degrees
          elbowAngleDegrees = elbowAngle * (180/PI);              // degrees 
      
          // calc milliseconds PWM to drive the servo.
          shoulderMs = shoulderAngleDegrees * 11;
          elbowMs = elbowAngleDegrees * 11;

          // *** end of Inverse Kinematics ***
      
          // write to servos, remove 45' and 90' offsets from arm default position
          servo2.writeMicroseconds(servo2Offset - (shoulderMs - 480) - shoulderMs2);    // shoulder
          servo3.writeMicroseconds(servo3Offset + (elbowMs - 1000));    // elbow     
      
          // drive linear slider motor

          if (encoder0Target < encoder0Pos - 150) {   // allow for a deadzone of 100 encoder counts
              analogWrite(in1, 255);
              analogWrite(in2, 0);
          }
          else if (encoder0Target > encoder0Pos + 150) {   // allow for a deadzone of 100 encoder counts
              analogWrite(in1, 0);
              analogWrite(in2, 255);
          }

          else {
            analogWrite(in1, 0);
            analogWrite(in2, 0);
          }
    }  // end of timed loop
}   //end if main loop

/**
 * @brief Interupt callback function. This will read the pins of the encoder(BIG MOTOR)
 *          to determine which direction the motor is spinning and by how far. Each tick
 *          of the encoder (i.e. encoder0Pos = encoder0Pos + 1) can be mapped to the 
 *          distance the motor shaft has rotated. 
 */
void doEncoderA(){  

  // look for a low-to-high on channel A
  if (digitalRead(encoder0PinA) == HIGH) { 
    // check channel B to see which way encoder is turning
    if (digitalRead(encoder0PinB) == LOW) {  
      encoder0Pos = encoder0Pos + 1;         // CW
    } 
    else {
      encoder0Pos = encoder0Pos - 1;         // CCW
    }
  }
  else   // must be a high-to-low edge on channel A                                       
  { 
    // check channel B to see which way encoder is turning  
    if (digitalRead(encoder0PinB) == HIGH) {   
      encoder0Pos = encoder0Pos + 1;          // CW
    } 
    else {
      encoder0Pos = encoder0Pos - 1;          // CCW
    }
  }
}

void doEncoderB(){  

  // look for a low-to-high on channel B
  if (digitalRead(encoder0PinB) == HIGH) {   
   // check channel A to see which way encoder is turning
    if (digitalRead(encoder0PinA) == HIGH) {  
      encoder0Pos = encoder0Pos + 1;         // CW
    } 
    else {
      encoder0Pos = encoder0Pos - 1;         // CCW
    }
  }
  // Look for a high-to-low on channel B
  else { 
    // check channel B to see which way encoder is turning  
    if (digitalRead(encoder0PinA) == LOW) {   
      encoder0Pos = encoder0Pos + 1;          // CW
    } 
    else {
      encoder0Pos = encoder0Pos - 1;          // CCW
    }
  }
}
