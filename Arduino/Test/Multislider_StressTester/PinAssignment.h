#define BAUD 57600


// ============ STEPPER ================

#define lxStep 3
#define lxDir 2
#define lyStep 5
#define lyDir 4
#define rxStep 7
#define rxDir 6
#define ryStep 9
#define ryDir 8

// ============ Limit Switch / IR Sensor ================

#define LimitSwitchLx A0
#define LimitSwitchLy A1
#define LimitSwitchRx A2
#define LimitSwitchRy A3

// ============ Brake ================
/*
#define BrakeLx A4
#define BrakeLy A5
#define BrakeRx A6
#define BrakeRy A7
*/
// ============ Encoder ================

#define encoderLxA 22
#define encoderLxB 23
#define encoderLyA 25
#define encoderLyB 26
#define encoderRxA 28
#define encoderRxB 29
#define encoderRyA 31
#define encoderRyB 32

// ============ SERVO CONTROLLER ================

#define DI1_SERVO_ON_lx 14
#define DI1_SERVO_ON_ly 15
#define DI1_SERVO_ON_rx 16
#define DI1_SERVO_ON_ry 17

#define DI2_ALARM_RESET_lx 50
#define DI2_ALARM_RESET_ly 51
#define DI2_ALARM_RESET_rx 52
#define DI2_ALARM_RESET_ry 53

#define DO1_SERVO_READY_lx 34
#define DO1_SERVO_READY_ly 35
#define DO1_SERVO_READY_rx 36
#define DO1_SERVO_READY_ry 37

//Zero-speed reached
#define DO2_ZSPD_lx 38
#define DO2_ZSPD_ly 39
#define DO2_ZSPD_rx 40
#define DO2_ZSPD_ry 41

//Reach the target position
#define DO4_TPOS_lx 42
#define DO4_TPOS_ly 43
#define DO4_TPOS_rx 44
#define DO4_TPOS_ry 45

//Servo Alarm
#define DO5_ALRM_lx 46
#define DO5_ALRM_ly 47
#define DO5_ALRM_rx 48
#define DO5_ALRM_ry 49

// ============ SERIAL ================

#define RX1 19
#define TX1 18


