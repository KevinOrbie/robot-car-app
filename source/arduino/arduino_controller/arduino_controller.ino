/**
 * @file arduino_controller.h
 * @author Kevin Orbie
 * @version 1.0
 * 
 * @brief Robot control and sense program for the Arduino Mega 2560.
 * 
 * This program controls the robot drive-train and reads IMU data.
 * It can be directed via the USB connection. 
 */


/* ========================= Include ========================= */
#include "core/master_connection.h"
#include "core/drive_controller.h"
#include "core/imu.h"


/* ======================== Constants ======================== */
/* Software Configuration. */
#define TIMEOUT_MS 2000

/* Hardware Configuration. */
#define BAUD_RATE 9600

#define PIN_SPEED_R 5   // PWM
#define PIN_SPEED_L 6   // PWM
#define PIN_DIRECTION_R 4
#define PIN_DIRECTION_L 7


/* ======================== Variables ======================== */
DriveController drive_ctrl = DriveController(PIN_SPEED_R, PIN_SPEED_L, PIN_DIRECTION_R, PIN_DIRECTION_L, TIMEOUT_MS);
MasterConnection master = MasterConnection(Serial, BAUD_RATE);
IMU imu = IMU(Serial3, BAUD_RATE);


/* ========================= Handler ========================= */
/**
 * @brief Defines how a message from the Master is handled.
 */
void handle(Message msg) {
  switch (msg.id) {
    /* No Message Recieved */
    case Message::ID::EMPTY:  
      break;

    case Message::ID::CMD_DRIVE:
      drive_ctrl.setState(msg);
      break;
    
    default: 
      Message err_msg = {};
      char err_string[] = "Invalid Message ID!";
      err_msg.id = Message::ID::ERROR;
      err_msg.data = reinterpret_cast<uint8_t*>(&err_string[0]);
      err_msg.num_data_bytes = sizeof(err_string) - 1; // Ignore null termination
      master.send(err_msg);
      break;
  }
}


/* =========================== Main ========================== */

/**
 * @brief Setup all objects, and set hardware settings.
 */
void setup() {
  drive_ctrl.setup();
  master.setup();
  imu.setup();
}

/**
 * @brief The main execution loop.
 */
void loop() {
  /* Read Message Bytes. */
  master.recieve();

  /* Handle recieved message. */
  handle(master.getMessage());
  
  /* Motor Control */
  drive_ctrl.iteration();

  /* IMU Control. */
  // imu.recieve();
  // if (imu.updated()) {
  //   // TODO: send updated imu data
  // }
  
  delay(20);
}

