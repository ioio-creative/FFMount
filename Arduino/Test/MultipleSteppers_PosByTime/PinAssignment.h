#define BAUD 57600



// ============ STEPPER ================

#define lyStep 3
#define lyDir 2
#define ryStep 5
#define ryDir 4

// ============ Limit Switch / IR Sensor ================

#define LimitSwitchLy A0 
#define LimitSwitchRy A1

// ============ Brake ================

#define BrakeLy A6 // NOT EXIST
#define BrakeRy A7 // NOT EXIST

// ============ Encoder ================

#define encoderLyA 22
#define encoderLyB 23
#define encoderRyA 25
#define encoderRyB 26

// ============ SERVO CONTROLLER ================

#define DI1_SERVO_ON_ly 14
#define DI1_SERVO_ON_ry 15

#define DI2_ALARM_RESET_ly 50
#define DI2_ALARM_RESET_ry 51

#define DO1_SERVO_READY_ly 34
#define DO1_SERVO_READY_ry 35

//Zero-speed reached
#define DO2_ZSPD_ly 38
#define DO2_ZSPD_ry 39

//Reach the target position
#define DO4_TPOS_ly 42
#define DO4_TPOS_ry 43

//Servo Alarm
#define DO5_ALRM_ly 46
#define DO5_ALRM_ry 47

// ============ SERIAL ================

#define RX1 19
#define TX1 18


