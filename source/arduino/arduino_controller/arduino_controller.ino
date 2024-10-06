/**
 * @brief Arduino controller to cmd the car's motor drivers.
 * 
 * The car can be controlled with a single byte over serial:
 *  > 0bxx000000: Direction Control [straight, left   , right  , NOT USED]
 *  > 0b00xx0000: Throttle  Control [standby , forward, reverse, brake   ]
 *  > 0b0000xxxx: PWM value to select the forward or reverse speed
 *
 * If no command is recieved in the last TIMEOUT_MS, the car goes to IDLE.
 */

/* Software Configuration. */
#define TIMEOUT_MS 2000

/* Hardware Configuration. */
#define BAUD_RATE 9600
#define PIN_SPEED_R 5   // PWM
#define PIN_SPEED_L 6   // PWM
#define PIN_DIRECTION_R 4
#define PIN_DIRECTION_L 7

/* Command Variables. */
bool timedout                = true;    // Only timeout once.
unsigned long last_cmd_time  = 0;       // Last time we recieved a command (msec).
size_t bytes_read            = 0;       // Number of bytes read last from serial.
uint8_t cmd                  = 0;       // The last command recieved.
uint8_t cmd_buffer[8]        = {0};     // A buffer to store recieved commands.

/* State Variables. */
enum class Throttle: uint8_t {STANDBY, FORWARD, REVERSE, BRAKE};
enum class Direction: uint8_t {STRAIGHT, LEFT, RIGHT};

struct State {
  Direction direction   = Direction::STRAIGHT;  // Direction state
  Throttle  throttle    = Throttle::STANDBY;    // Throttle state
  int pwm               = 0;                    // PWM value for speed control
};

State state  = {};     // Current state of the motors.
bool updated = false;  // If the state was updated this loop() iteration.


/* ======================== SETUP ======================== */
void setup() {
  /* Configure serial communication. */
  Serial.begin(BAUD_RATE);
  Serial.setTimeout(0);  // Don't wait for input (by default 1000 ms)

  /* Configure pins. */
  pinMode(PIN_SPEED_R, OUTPUT);
  pinMode(PIN_SPEED_L, OUTPUT);
  pinMode(PIN_DIRECTION_R, OUTPUT);
  pinMode(PIN_DIRECTION_L, OUTPUT);
}


/* ======================== LOOP ========================= */
void loop() {
  /* ------------------ Process Input ----------------- */
  if (Serial.available() > 0) {
    /**
     * NOTE: If you send multiple chars at a time,
     * the first 8 chars are written to into the 
     * buffer, and the last one is chosen.
     */

    /* Process up to 8 commands at a time. */
    bytes_read = Serial.readBytes(cmd_buffer, 8);

    /* Detect when nothing recieved. */
    if (bytes_read <= 0) {
      updated = false;
    }

    /* Read the last command. */
    cmd = cmd_buffer[bytes_read - 1];

    /* Update state. */
    last_cmd_time = millis();
    timedout = false;
    updated = true;
  }

  /* ------------------ Update State ------------------ */
  if (updated) {
    /* Direction Control Bytes. */
    // Bytes: 0bxx000000
    switch((cmd & 0xC0) >> 6) {
      case 0:
        state.direction = Direction::STRAIGHT;
        break;
      case 1:
        state.direction = Direction::LEFT;
        break;
      case 2:
        state.direction = Direction::RIGHT;
        break;
      case 3:
      default:
        state.direction = Direction::STRAIGHT;
        break;
    }

    /* Throttle Control Bytes. */
    // Bytes: 0b00xx0000
    switch((cmd & 0x30) >> 4) {
      case 0:
        state.throttle = Throttle::STANDBY;
        break;
      case 1:
        state.throttle = Throttle::FORWARD;
        break;
      case 2:
        state.throttle = Throttle::REVERSE;
        break;
      case 3:
      default:
        state.throttle = Throttle::BRAKE;
        break;
    }

    /* Speed Control Bytes. */
    // Bytes: 0b0000xxxx
    state.pwm = (cmd & 0x0F) << 4;
  }

  /* If we don't reviece commands for a while, go idle. */
  if (!timedout && (millis() - last_cmd_time > TIMEOUT_MS)) {
    state.direction = Direction::STRAIGHT;
    state.throttle = Throttle::STANDBY;
    state.pwm = 0;
    timedout = true;  // Make sure we only go to idle once.
    updated = true;   // Update Motor Commands to idle.
  }

  /* ----------------- Command Motors ----------------- */
  if (updated) {
    /* Control Direction. */
    switch(state.direction) {

      /* NOTE: Always turn at full speed and in place for now. */
      case Direction::LEFT: {
        analogWrite (PIN_SPEED_R, 255);
        digitalWrite(PIN_DIRECTION_R, HIGH);
        analogWrite (PIN_SPEED_L, 255);
        digitalWrite(PIN_DIRECTION_L, HIGH);
        break;
      }
      case Direction::RIGHT: {
        analogWrite (PIN_SPEED_R, 255);
        digitalWrite(PIN_DIRECTION_R, LOW);
        analogWrite (PIN_SPEED_L, 255);
        digitalWrite(PIN_DIRECTION_L, LOW);
        break;
      }

      /* Control Throttle. */
      case Direction::STRAIGHT: {
        switch(state.throttle) {
          case Throttle::FORWARD: {
            analogWrite (PIN_SPEED_R, state.pwm);
            digitalWrite(PIN_DIRECTION_R, HIGH);
            analogWrite (PIN_SPEED_L, state.pwm);
            digitalWrite(PIN_DIRECTION_L, LOW);
            break;
          }
          case Throttle::REVERSE: {
            analogWrite (PIN_SPEED_R, state.pwm);
            digitalWrite(PIN_DIRECTION_R, LOW);
            analogWrite (PIN_SPEED_L, state.pwm);
            digitalWrite(PIN_DIRECTION_L, HIGH);
            break;
          }
          case Throttle::STANDBY:
          case Throttle::BRAKE:
          default: {// Normally not reached
            analogWrite (PIN_SPEED_R, 0);
            digitalWrite(PIN_DIRECTION_R, HIGH);
            analogWrite (PIN_SPEED_L, 0);
            digitalWrite(PIN_DIRECTION_L, LOW);
            break;
          }
        }
        break;
      }

      default: { // Normally not reached 
        analogWrite (PIN_SPEED_R, 0);
        digitalWrite(PIN_DIRECTION_R, HIGH);
        analogWrite (PIN_SPEED_L, 0);
        digitalWrite(PIN_DIRECTION_L, LOW);
        break;
      }
    }

    /* ########### DEBUG ########### */
    // Serial.print("STATE: ");

    // switch(state.direction) {
    //   case Direction::STRAIGHT:
    //     Serial.print("straight");
    //     break;
    //   case Direction::LEFT:
    //     Serial.print("left    ");
    //     break;
    //   case Direction::RIGHT:
    //     Serial.print("right   ");
    //     break;
    //   default: // Normally not reached
    //     Serial.print("other   ");
    //     break;
    // }

    // Serial.print(", ");
    // switch(state.throttle) {
    //   case Throttle::STANDBY:
    //     Serial.print("standby");
    //     break;
    //   case Throttle::FORWARD:
    //     Serial.print("forward");
    //     break;
    //   case Throttle::REVERSE:
    //     Serial.print("reverse");
    //     break;
    //   case Throttle::BRAKE:
    //     Serial.print("brake  ");
    //     break;
    //   default: // Normally not reached
    //     Serial.print("other  ");
    //     break;
    // }

    // Serial.print(", ");
    // Serial.print(state.pwm);

    // Serial.println("");
    /* ############################# */
  }

  updated = false;
  delay(20);
}













