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

PS2MouseHandler mouse(PS2CLK, PS2DATA, PS2_MOUSE_STREAM);

// Error stuff that is set by the original code,
// I just left it here because there is some error
// recovery stuff
int error_parity = 0;
int error_watchdog = 0;

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

  if (data_cnt == 0) {
    if (data & 0x10) {
      stateX = stateX | 0x10; // Negative X
    }
    else {
      stateX = stateX & 0xEF; // Positive X
    }
    if (data & 0x20) {
      stateY = stateY | 0x20; // Negative Y
    }
    else {
      stateY = stateY & 0xDF; // Positive Y
    }
    data_cnt++;
  }
  else if (data_cnt == 1) {

    if ((stateX & 0x10) && data) {
      dataX = 256 - data;
    }
    else {
      dataX = data;
    }
    data_cnt++;
  }
  else if (data_cnt == 2) {

    if ((stateY & 0x20) && data) {
      dataY = 256 - data;
    }
    else {
      dataY = data;
    }

#ifdef DEBUG
    Serial.print(dataX);
    Serial.print(":");
    Serial.println(dataY);
#endif

// Limiter

    if (error_watchdog || error_parity) {
      dataX = 0;
    }
    else if (dataX > MAX_MOVE) {
      dataX = MAX_MOVE;
    }

    if (error_watchdog || error_parity) {
      dataY = 0;
    }
    else if (dataY > MAX_MOVE) {
      dataY = MAX_MOVE;
    }

    error_watchdog = 0;
    error_parity = 0;

// Cursor
// state      0 1 3 2
//          ___     ___
// A pulse |   |___|   |___
//            ___     ___
// B pulse   |   |___|   |___
//
// declease <--        --> increase
//
// For XA,XB the increasing pulse move the cursor rightward. (Positive for PS/2)
// For YA,YB the increasing pulse move the cursor downward. (Negative for PS/2)

    while (dataX) {
      switch (stateX) {
        case 0x00:
          stateX = 0x01;
          pinMode(XA, OUTPUT);
          pinMode(XB, INPUT);
          break;
        case 0x01:
          stateX = 0x03;
          pinMode(XA, OUTPUT);
          pinMode(XB, OUTPUT);
          break;
        case 0x03:
          stateX = 0x02;
          pinMode(XA, INPUT);
          pinMode(XB, OUTPUT);
          break;
        case 0x02:
          stateX = 0x00;
          pinMode(XA, INPUT);
          pinMode(XB, INPUT);
          break;
        case 0x10:
          stateX = 0x12;
          pinMode(XA, INPUT);
          pinMode(XB, OUTPUT);
          break;
        case 0x12:
          stateX = 0x13;
          pinMode(XA, OUTPUT);
          pinMode(XB, OUTPUT);
          break;
        case 0x13:
          stateX = 0x11;
          pinMode(XA, OUTPUT);
          pinMode(XB, INPUT);
          break;
        case 0x11:
          stateX = 0x10;
          pinMode(XA, INPUT);
          pinMode(XB, INPUT);
          break;
        default:
          stateX = 0x00;
      }
      dataX--;
      delayMicroseconds(150);
      //delayMicroseconds(3);
    }

    while (dataY) {
      switch (stateY) {
        case 0x20:
          stateY = 0x21;
          pinMode(YA, OUTPUT);
          pinMode(YB, INPUT);
          break;
        case 0x21:
          stateY = 0x23;
          pinMode(YA, OUTPUT);
          pinMode(YB, OUTPUT);
          break;
        case 0x23:
          stateY = 0x22;
          pinMode(YA, INPUT);
          pinMode(YB, OUTPUT);
          break;
        case 0x22:
          stateY = 0x20;
          pinMode(YA, INPUT);
          pinMode(YB, INPUT);
          break;
        case 0x00:
          stateY = 0x02;
          pinMode(YA, INPUT);
          pinMode(YB, OUTPUT);
          break;
        case 0x02:
          stateY = 0x03;
          pinMode(YA, OUTPUT);
          pinMode(YB, OUTPUT);
          break;
        case 0x03:
          stateY = 0x01;
          pinMode(YA, OUTPUT);
          pinMode(YB, INPUT);
          break;
        case 0x01:
          stateY = 0x00;
          pinMode(YA, INPUT);
          pinMode(YB, INPUT);
          break;
        default:
          stateY = 0x00;
      }
      dataY--;
      delayMicroseconds(150);
      //delayMicroseconds(3);
    }
    data_cnt = 0;
  } 
  delayMicroseconds(5);

}
