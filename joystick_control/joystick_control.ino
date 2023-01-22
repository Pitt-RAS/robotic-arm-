#include <Servo.h>

// SERVO DEFINITIONS
#define ElbowServoPin 5     // The joint that is directly attached to the carriage
#define WristServoPin 6     //The next joint downstream from the elbow servo.
#define GripperServoPin 3   //The gripper servo motor

// LINEAR MOTOR DEFINITIONS
#define MoveLeftPin 10      // Will only move left if the RED motor wire is wired to OUT1 on the driver
#define MoveRightPin 9      // Will only move right is the BLACK motor wire is wired to OUT2 on the driver
#define PWM_DELAY 1

// JOINT LIMITS
#define ELBOW_SERVO_BENT_90_POS 120   
#define ELBOW_SERVO_STRAIGHT_UP_POS 60
#define GRIPPER_SERVO_CLOSED_POS 70
#define GRIPPER_SERVO_OPEN_POS 0
#define WRIST_SERVO_STRAIGHT_UP 90
#define WRIST_SERVO_BENT_POS 150

// JOYSTICK DEFINES
// X is joystick movement left and right, Y is joystick movement up and down.
#define LEFT_JOY_X_PIN A0     // JoyLX
#define LEFT_JOY_Y_PIN A1     // JoyLY
#define RIGHT_JOY_X_PIN A2    // JoyRX
#define RIGHT_JOY_Y_PIN A3    // JoyRY
#define JOY_INCREMENT_SPEED 1
#define JOY_LED_PIN 2
#define JOY_MAX_POS 1023
#define JOY_MID_POS 512
#define JOY_MIN_POS 0
#define JOY_MID_RANGE_TOP 700
#define JOY_MID_RANGE_BOTTOM 300

// OTHER DEFINES
#define LED_MAX_COUNT 1200

// DEBUG, UNCOMMENT TO ENABLE
// #define DEBUG

// DEFINE SERVO OBJECTS
Servo GripperServo;
Servo ElbowServo;
Servo WristServo;

// DEFINE GLOBAL VARS 
// Vars to hold raw values from the joystick
uint16_t JoyLXpos = JOY_MID_POS;
uint16_t JoyLYpos = JOY_MID_POS;
uint16_t JoyRXpos = JOY_MID_POS;
uint16_t JoyRYpos = JOY_MID_POS; 

// Vars to hold mapped values from joystick range to servo degree range
uint16_t mappedLY = JOY_MID_POS;
uint16_t mappedRX = JOY_MID_POS;
uint16_t mappedRY = JOY_MID_POS;

// Vars to keep track of joystick movements over time
// The joysticks increment / decrement these count values depending on joystick position
uint16_t countLY = JOY_MID_POS;
uint16_t countRX = JOY_MID_POS;
uint16_t countRY = JOY_MID_POS;

// Used for blinking LED
uint32_t LEDCount = 0;
bool LEDState = false;

// SETUP CODE
void setup() {

  // Setup linear motor driver pins and drive low to prevent movement.
  pinMode(MoveLeftPin, OUTPUT);
  pinMode(MoveRightPin, OUTPUT);
  digitalWrite(MoveLeftPin, LOW);
  digitalWrite(MoveRightPin, LOW);

  // Configure joystick LED
  pinMode(JOY_LED_PIN, OUTPUT);
  digitalWrite(JOY_LED_PIN, HIGH);

  // Initialize servo motors
  GripperServo.attach(GripperServoPin);
  ElbowServo.attach(ElbowServoPin);
  WristServo.attach(WristServoPin);

  // Command servo motors to the midpoint by taking average
  GripperServo.write(abs(GRIPPER_SERVO_CLOSED_POS-GRIPPER_SERVO_OPEN_POS)/2);
  ElbowServo.write(abs(ELBOW_SERVO_BENT_90_POS-ELBOW_SERVO_STRAIGHT_UP_POS)/2);
  WristServo.write(abs(WRIST_SERVO_BENT_POS-WRIST_SERVO_STRAIGHT_UP)/2);

  // If debug mode enabled, enable serial monitor.
  #ifdef DEBUG
    Serial.begin(9600);
  #endif 

}

// MAIN CODE

void loop() {
  
  // Read all of the joystick values
  JoyLXpos = analogRead(LEFT_JOY_X_PIN);
  JoyLYpos = analogRead(LEFT_JOY_Y_PIN);
  JoyRXpos = analogRead(RIGHT_JOY_X_PIN);
  JoyRYpos = analogRead(RIGHT_JOY_Y_PIN);

  // If joystick is pushed down and count is greater than the minimum
  if(JoyLYpos < JOY_MID_RANGE_BOTTOM && countLY > JOY_MIN_POS) 
  {
    countLY -= JOY_INCREMENT_SPEED; 
  }
  // else if joystick is poshed up and count is less than the max
  else if(JoyLYpos > JOY_MID_RANGE_TOP && countLY < JOY_MAX_POS)
  {
    countLY += JOY_INCREMENT_SPEED;
  }

  // Repeat for other joystick ranges
  if(JoyRXpos < JOY_MID_RANGE_BOTTOM && countRX > JOY_MIN_POS) 
  {
    countRX -= JOY_INCREMENT_SPEED;
  }
  else if(JoyRXpos > JOY_MID_RANGE_TOP && countRX < JOY_MAX_POS)
  {
    countRX += JOY_INCREMENT_SPEED;
  }


  if(JoyRYpos < JOY_MID_RANGE_BOTTOM && countRY > JOY_MIN_POS) 
  {
    countRY -= JOY_INCREMENT_SPEED;
  }
  else if(JoyRYpos > JOY_MID_RANGE_TOP && countRY < JOY_MAX_POS)
  {
    countRY += JOY_INCREMENT_SPEED;
  }

  // Map the count values to valid servo motor positions
  // Change joystick reading range for LY to degree range of motion of wrist
  mappedLY = map(countLY, JOY_MIN_POS, JOY_MAX_POS, WRIST_SERVO_STRAIGHT_UP, WRIST_SERVO_BENT_POS);
  // Change joystick reading range for RX to degree range of motion of gripper
  mappedRX = map(countRX, JOY_MIN_POS, JOY_MAX_POS, GRIPPER_SERVO_OPEN_POS, GRIPPER_SERVO_CLOSED_POS);
  // Change joystick reading range for RY to degree range of motion of elbow
  mappedRY = map(countRY, JOY_MIN_POS, JOY_MAX_POS, ELBOW_SERVO_STRAIGHT_UP_POS, ELBOW_SERVO_BENT_90_POS);

  // Command the servos
  WristServo.write(mappedLY);
  GripperServo.write(mappedRX);
  ElbowServo.write(mappedRY);

  // Use PWM to command the linear motor. No other axes will move while linear motor is moving
  
  // Move motor to the right if joystick is in right pos
  while(JoyLXpos > JOY_MID_RANGE_TOP)
  { 
    digitalWrite(MoveRightPin, HIGH);
    delay(PWM_DELAY);
    // digitalWrite(MoveRightPin, LOW );
    // delay(PWM_DELAY);
    JoyLXpos = analogRead(LEFT_JOY_X_PIN); 
  }

  // Move motor to the left if joystick is in left pos
  while(JoyLXpos < JOY_MID_RANGE_BOTTOM)
  {   
    // digitalWrite(MoveLeftPin, LOW);
    // delay(PWM_DELAY);
    digitalWrite(MoveLeftPin, HIGH);
    // delay(PWM_DELAY);
    JoyLXpos = analogRead(LEFT_JOY_X_PIN);
  }

  // If joystick is in the mid range position, turn off linear motor
  if(JoyLXpos < JOY_MID_RANGE_TOP && JoyLXpos > JOY_MID_RANGE_BOTTOM)
  {
    digitalWrite(MoveRightPin, LOW);
    digitalWrite(MoveLeftPin, LOW);
  }

  if(++LEDCount > LED_MAX_COUNT)
  {
    digitalWrite(JOY_LED_PIN, LEDState ? HIGH : LOW);
    LEDCount = 0;
    LEDState = !LEDState;
  }

  #ifdef DEBUG
    Serial.print("SwitchL:\t");
    Serial.print("X-axis: ");
    Serial.print(JoyLXpos);
    Serial.print("\tY-axis: ");
    Serial.print(mappedLY);
    Serial.print("\t\t");

    Serial.print("SwitchR:\t");
    Serial.print("X-axis: ");
    Serial.print(mappedRX);
    Serial.print("\tY-axis: ");
    Serial.print(mappedRY);
    Serial.print("\n");

    delay(50);
  #endif
}


 



