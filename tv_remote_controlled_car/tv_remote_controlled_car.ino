#include <AFMotor.h>
#include<IRremote.h>

unsigned long GO_FORWARD = 2160022444;
unsigned long GO_BACKWARD = 2160020404;
unsigned long TURN_LEFT = 2160040294;
unsigned long TURN_RIGHT = 2160034684;
unsigned long CONTINUE = 4294967295;
unsigned long OK_PIN = 2160030604;
unsigned long POWER_ON_OFF = 2160016324;
unsigned long STOP = 0;

AF_DCMotor motorLeft(4);
AF_DCMotor motorRight(3);

const int RECV_PIN = A5;
IRrecv irrecv(RECV_PIN);

decode_results results;

unsigned long CURRENT_DIRECTION = STOP;
unsigned long TURN_DIRECTION = STOP;

unsigned long PREVIOUS_TIME = millis();

int FULL_SPEED = 200;
int TURN_SPEED_FAST = 230;
int TURN_SPEED_SLOW = 80;

void setup() {
  Serial.begin(9600);
  
  motorLeft.setSpeed(FULL_SPEED);
  motorRight.setSpeed(FULL_SPEED);

  irrecv.enableIRIn();
  irrecv.blink13(true);
}

void stop() {
  motorLeft.setSpeed(0);
  motorRight.setSpeed(0);
  motorLeft.run(RELEASE);
  motorRight.run(RELEASE);
  Serial.println("Stop");
  CURRENT_DIRECTION = STOP;
  TURN_DIRECTION = STOP;
}

void forward() {
  motorLeft.setSpeed(FULL_SPEED);
  motorRight.setSpeed(FULL_SPEED);
  motorLeft.run(FORWARD);
  motorRight.run(FORWARD);
  CURRENT_DIRECTION = GO_FORWARD;
  TURN_DIRECTION = STOP;
  Serial.println("FORWARD");
}

void backward() {
  motorLeft.setSpeed(FULL_SPEED);
  motorRight.setSpeed(FULL_SPEED);
  motorLeft.run(BACKWARD);
  motorRight.run(BACKWARD);
  CURRENT_DIRECTION = GO_BACKWARD;
}

void turnLeft() {
  motorLeft.setSpeed(TURN_SPEED_SLOW);
  motorRight.setSpeed(TURN_SPEED_FAST);
  motorLeft.run(FORWARD);
  motorRight.run(FORWARD);
  Serial.println("TURN LEFT");
  TURN_DIRECTION = TURN_LEFT;
}

void turnRight() {
  motorLeft.setSpeed(TURN_SPEED_FAST);
  motorRight.setSpeed(TURN_SPEED_SLOW);
  motorLeft.run(FORWARD);
  motorRight.run(FORWARD);
  Serial.println("TURN RIGHT");
  TURN_DIRECTION = TURN_RIGHT;
}

void movingForward(unsigned long NEW_DIRECTION) {
  if (NEW_DIRECTION == GO_FORWARD) {
      forward();      
    } else if (NEW_DIRECTION == GO_BACKWARD) {
      // Change direction to stop
      stop();
    } else if (NEW_DIRECTION == TURN_LEFT) {
      turnLeft();
    } else if (NEW_DIRECTION == TURN_RIGHT) {
      turnRight();
    } else if (NEW_DIRECTION == CONTINUE) {
      if (TURN_DIRECTION == TURN_LEFT) {
        turnLeft();
      } else if (TURN_DIRECTION == TURN_RIGHT) {
        turnRight();
      }
    }
}

void movingBackward(unsigned long NEW_DIRECTION) {
  if (NEW_DIRECTION == GO_BACKWARD) {
      backward();
    } else if (NEW_DIRECTION == GO_FORWARD) {
      // Stop
      stop();
    }
}

void loop() {
  if (irrecv.decode(&results)) {
    unsigned long NEW_DIRECTION = results.value;
    Serial.print("Current direction: ");
    Serial.println(CURRENT_DIRECTION);
    Serial.print("New direction: ");
    Serial.println(NEW_DIRECTION);

    if (CURRENT_DIRECTION == GO_FORWARD) {
      movingForward(NEW_DIRECTION);
    } else if (CURRENT_DIRECTION == GO_BACKWARD) {
      movingBackward(NEW_DIRECTION);
    } else if (CURRENT_DIRECTION == STOP) {
      if (NEW_DIRECTION == GO_FORWARD) {
        forward();
      } else if (NEW_DIRECTION == GO_BACKWARD) {
        backward();
      } else if (NEW_DIRECTION == TURN_LEFT) {
        turnLeft();
      } else if (NEW_DIRECTION == TURN_RIGHT) {
        turnRight();
      }
    }
    PREVIOUS_TIME = millis();
    irrecv.resume();
  } else {
    // If no other key presses within the tolerance time, set back the previous direction
    if (millis() - PREVIOUS_TIME > 500) {
      if (CURRENT_DIRECTION == GO_FORWARD) {
        forward();
      } else {
        stop();
      }
    }
  }
}
