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
#include "arduino_types.h"
#include "logger.h"

#include "master_connection.h"
#include "drive_controller.h"
#include "imu.h"


using namespace arduino;
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
MasterConnection master = MasterConnection(Serial, BAUD_RATE);
DriveController drive_ctrl = DriveController(PIN_SPEED_R, PIN_SPEED_L, PIN_DIRECTION_R, PIN_DIRECTION_L, TIMEOUT_MS);
Logger logger = Logger::instance(&master);
IMU &imu = IMU::instance(&Serial3);


/* ========================= Handler ========================= */
/**
 * @brief Defines how a message from the Master is handled.
 */
void handle(Message msg) {
  switch (msg.id) {
    /* No Message Recieved */
    case MessageID::EMPTY:  
      break;

    case MessageID::CMD_DRIVE:
      drive_ctrl.setState(msg);
      break;
    
    default: 
      logger.error(ErrorID::UNKOWN_MSG_ID);
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

  logger.info("Setup Finished!");
}

/**
 * @brief The main execution loop.
 */
void loop() {
  /* Read Message Bytes. */
  master.recieve();

  /* Handle recieved message. */
  handle(master.getMessage());
  
  /* Motor Control. */
  drive_ctrl.iteration();

  /* IMU Data. */
  imu.recieve();
  while (imu.available()) {
    master.send(imu.getDataMessage());
  }
  
  delay(5000);
}

