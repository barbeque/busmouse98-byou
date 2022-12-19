//ps2busmouse98
//PS/2 - pc-98 bus mouse converter
#include "PS2MouseHandler.h"

#define PS2DATA A4
#define PS2CLK A5
#define LED 13
#define XA PD2
#define XB PD3
#define YA PD4
#define YB PD5
#define LB PD6
#define RB PD7
#define ERROR_NUM 3
#define MAX_MOVE 20

//#define DEBUG

PS2MouseHandler mouse(PS2CLK, PS2DATA, PS2_MOUSE_STREAM);

void setup() {
  int data = 0;
#ifdef DEBUG
  Serial.begin(9600);
  Serial.println("Setup");
#endif
  pinMode(LED, OUTPUT);
  pinMode(XA, INPUT);
  pinMode(XB, INPUT);
  pinMode(YA, INPUT);
  pinMode(YB, INPUT);
  pinMode(LB, INPUT);
  pinMode(RB, INPUT);
  digitalWrite(XA, LOW);
  digitalWrite(XB, LOW);
  digitalWrite(YA, LOW);
  digitalWrite(YB, LOW);
  digitalWrite(LB, LOW);
  digitalWrite(RB, LOW);

  if((data = mouse.initialise()) != 0) {
    #ifdef DEBUG
    Serial.print("Mouse error: ");
    Serial.println(data);
    #endif

    while(1) {
      // Alert light for error
      digitalWrite(LED, HIGH);
      delay(150);
      digitalWrite(LED, LOW);
      delay(150);
    }
  }

  #ifdef DEBUG
  Serial.println("PS/2 mouse initialized.");
  #endif
}

int receiveData(void) {
  mouse.get_data();
  return mouse.status(); // same byte as before
}

int sgn(int value) {
  if(value == 0) { return 0; }
  else if(value > 0) { return 1; }
  return -1;
}

void loop() {
  int data = 0;
  static int data_cnt = 0;
  static int stateX = 0;
  static int stateY = 0;
  static int dataX = 0;
  static int dataY = 0;

  data = receiveData();

  // Buttons
  pinMode(LB, mouse.button(0) ? OUTPUT : INPUT);
  pinMode(RB, mouse.button(2) ? OUTPUT : INPUT); // button(1) is middle mouse

  // Prepare state machine to clock out any position changes
  int delta_x = max(-MAX_MOVE, min(MAX_MOVE, mouse.x_movement()));
  int delta_y = max(-MAX_MOVE, min(MAX_MOVE, mouse.y_movement()));

#ifdef DEBUG
  /*Serial.print("X: ");
  Serial.print(delta_x);
  Serial.print(" Y: ");
  Serial.println(delta_y);*/
#endif

  while(abs(delta_x) > 0) {
    switch(stateX) {
      case 0:
        pinMode(XA, OUTPUT);
        pinMode(XB, INPUT);
        break;
      case 1:
        pinMode(XA, OUTPUT);
        pinMode(XB, OUTPUT);
        break;
      case 2:
        pinMode(XA, INPUT);
        pinMode(XB, OUTPUT);
        break;
      case 3:
        pinMode(XA, INPUT);
        pinMode(XB, INPUT);
        break;
      default:
        stateX = 0; // shouldn't happen, but...
    }
    
    int increment = sgn(delta_x);
    delta_x -= increment;
    stateX = (stateX + increment) % 4;
    if(stateX < 0) {
      stateX = 3; // how does modulo not work??
    }
    delayMicroseconds(150);
  }

  while(abs(delta_y) > 0) {
    switch(stateY) {
      case 3:
        pinMode(YA, OUTPUT);
        pinMode(YB, INPUT);
        break;
      case 2:
        pinMode(YA, OUTPUT);
        pinMode(YB, OUTPUT);
        break;
      case 1:
        pinMode(YA, INPUT);
        pinMode(YB, OUTPUT);
        break;
      case 0:
        pinMode(YA, INPUT);
        pinMode(YB, INPUT);
        break;
      default:
        stateY = 0; // shouldn't happen, but...
    }
    
    int increment = sgn(delta_y);
    delta_y -= increment;
    stateY = (stateY + increment) % 4;
    if(stateY < 0) {
      stateY = 3; // how does modulo not work??
    }
    delayMicroseconds(175);
  }
  
  delayMicroseconds(5);

}
