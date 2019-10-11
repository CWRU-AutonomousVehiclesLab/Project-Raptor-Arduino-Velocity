#include <Servo.h>
#define SERIAL_PORT_SPEED 115200  // Serial Speed DO NOT CHANGE!
#define MESSAGE_LENGTH 4

//! Pin Setup:
#define LEFT_MOTOR 22
#define RIGHT_MOTOR 23

//! Servo Definition:
#define THROTTLE_MIN 1250
#define THROTTLE_MAX 1750

//! Message Decoding:
#define STATE 0     // DO NOT CHANGE!
#define STEERING 1  // DO NOT CHANGE!
#define THROTTLE 2  // DO NOT CHANGE!

//! State Machine:
int current_state = 0;
#define EMERGENCY_STOP 0
#define IDLE 1
#define RC_MODE 2
#define AUTONOMOUS_MODE_EN 3

//! Global Variables
volatile uint8_t messagein[MESSAGE_LENGTH];

//! Global Struct
Servo LEFT, RIGHT;

void setup() {
    Serial.begin(SERIAL_PORT_SPEED);
    Serial3.begin(SERIAL_PORT_SPEED);
    RIGHT.attach(RIGHT_MOTOR, THROTTLE_MIN, THROTTLE_MAX);
    LEFT.attach(LEFT_MOTOR, THROTTLE_MIN, THROTTLE_MAX);
    messagein[THROTTLE] = 0;
    delay(10000);
}
//! Used for convering the pulse back to the throttle angle
int pulse2percentage() {
    //* map is used for easier converting 1ms~2ms for remote to 700us~2300us for
    // servo map(value, fromLow, fromHigh, toLow, toHigh)
    int temp = messagein[THROTTLE];
    if (temp > 100) {
        temp = (temp - 100) * -1;
        int val = map(temp, -100, 100, THROTTLE_MIN, THROTTLE_MAX);
        Serial.println(val);
        return val;
    } else if (temp >= 200 || temp < 0) {
        return 0;
    } else {
        int val = map(temp, -100, 100, THROTTLE_MIN, THROTTLE_MAX);
        Serial.println(val);
        return val;
    }
}
void print_recieved_message() {
    Serial.print("State:");
    Serial.print((int)messagein[STATE]);
    Serial.print("\t");
    Serial.print("STEERING:");
    Serial.print((int)messagein[STEERING]);
    Serial.print("\t");
    Serial.print("THROTTLE:");
    Serial.print((int)messagein[THROTTLE]);
    Serial.print("\t");
    Serial.print("Command:");
    Serial.print(pulse2percentage());
    Serial.println("\t");
}

void SerialInterpretation() {
    if (Serial3.available() >= MESSAGE_LENGTH) {
        Serial.println("Recieved...");
        int totalBytes = Serial3.available();
        Serial.print("Total Bytes:");
        Serial.print("\t");
        Serial.print(totalBytes);
        Serial.println("\t");
        for (int counter = 0; counter < MESSAGE_LENGTH; counter++) {
            messagein[counter] = Serial3.read();
        }

        while (Serial3.available() > 0) {
            Serial3.read();
        }
    } else {
        Serial3.flush();
        while (Serial3.available() > 0) {
            Serial3.read();
        }
        Serial.println("Not Recieved...");
    }
    print_recieved_message();
}
void loop() {
    SerialInterpretation();
    switch (messagein[STATE]) {
        case EMERGENCY_STOP:
            LEFT.writeMicroseconds(pulse2percentage());
            RIGHT.writeMicroseconds(pulse2percentage());
            break;

        case IDLE:
            break;

        case RC_MODE:
            LEFT.writeMicroseconds(pulse2percentage());
            RIGHT.writeMicroseconds(pulse2percentage());
            break;

        case AUTONOMOUS_MODE_EN:
            LEFT.writeMicroseconds(pulse2percentage());
            RIGHT.writeMicroseconds(pulse2percentage());
            break;

        default:
            Serial.println("System fucked up!");
            break;
    }
}

