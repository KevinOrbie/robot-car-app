/**
 * @brief Arduino controller to command the car's motor drivers.
 * 
 * The car can be controlled with a single byte over serial:
 *  > 0bxx000000: Direction Control [straight, left   , right  , NOT USED]
 *  > 0b00xx0000: Throttle  Control [standby , forward, reverse, brake   ]
 *  > 0b0000xxxx: PWM value to select the forward or reverse speed
 */

/* Hardware Configuration. */
#define BAUD_RATE 9600
#define PIN_F 6   // PWM
#define PIN_B 5   // PWM
#define PIN_R 8
#define PIN_L 7

/* Command Variables. */
size_t bytes_read = 0;
uint8_t command_buffer[8] = {0};
uint8_t command = 0;

/* State Variables. */
enum class Throttle: uint8_t {STANDBY, FORWARD, REVERSE, BRAKE};
enum class Direction: uint8_t {STRAIGHT, LEFT, RIGHT};

struct State {
  int pwm               = 0;
  Throttle  throttle    = Throttle::STANDBY;
  Direction direction   = Direction::STRAIGHT;
} state = {};

bool updated = false;


/* ======================== SETUP ======================== */
void setup() {
  /* Configure serial communication. */
  Serial.begin(BAUD_RATE);
  Serial.setTimeout(0);  // Don't wait for input (by default 1000 ms)

  /* Configure pins. */
  pinMode(PIN_F, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_L, OUTPUT);
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
    bytes_read = Serial.readBytes(command_buffer, 8);

    /* Detect when nothing recieved. */
    if (bytes_read <= 0) {
      updated = false;
    }

    /* Read the last command. */
    command = command_buffer[bytes_read - 1];

    /* Update state. */
    updated = true;
  }

  /* ------------------ Update State ------------------ */
  if (updated) {
    /* Direction Control Bytes. */
    // Bytes: 0bxx000000
    switch((command & 0xC0) >> 6) {
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
    switch((command & 0x30) >> 4) {
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
    state.pwm = (command & 0x0F) << 4;
  }

  /* ----------------- Command Motors ----------------- */
  if (updated) {
    /* Control Direction Motor Driver. */
    switch(state.direction) {
      case Direction::STRAIGHT:
        digitalWrite(PIN_R, LOW);
        digitalWrite(PIN_L, LOW);
        break;
      case Direction::LEFT:
        digitalWrite(PIN_R, LOW);
        digitalWrite(PIN_L, HIGH);
        break;
      case Direction::RIGHT:
        digitalWrite(PIN_R, HIGH);
        digitalWrite(PIN_L, LOW);
        break;
      default: // Normally not reached
        digitalWrite(PIN_R, LOW);
        digitalWrite(PIN_L, LOW);
        break;
    }

    /* Control Throttle Motor Driver. */
    switch(state.throttle) {
      case Throttle::STANDBY:
        digitalWrite(PIN_F, LOW);
        digitalWrite(PIN_B, LOW);
        break;
      case Throttle::FORWARD:
        analogWrite (PIN_F, state.pwm);
        digitalWrite(PIN_B, LOW);
        break;
      case Throttle::REVERSE:
        digitalWrite(PIN_F, LOW);
        analogWrite (PIN_B, state.pwm);
        break;
      case Throttle::BRAKE:
        digitalWrite(PIN_F, HIGH);
        digitalWrite(PIN_B, HIGH);
        break;
      default: // Normally not reached
        digitalWrite(PIN_F, LOW);
        digitalWrite(PIN_B, LOW);
        break;
    }

    /* ########### DEBUG ########### */
    Serial.print("STATE: ");

    switch(state.direction) {
      case Direction::STRAIGHT:
        Serial.print("straight");
        break;
      case Direction::LEFT:
        Serial.print("left    ");
        break;
      case Direction::RIGHT:
        Serial.print("right   ");
        break;
      default: // Normally not reached
        Serial.print("other   ");
        break;
    }

    Serial.print(", ");
    switch(state.throttle) {
      case Throttle::STANDBY:
        Serial.print("standby");
        break;
      case Throttle::FORWARD:
        Serial.print("forward");
        break;
      case Throttle::REVERSE:
        Serial.print("reverse");
        break;
      case Throttle::BRAKE:
        Serial.print("brake  ");
        break;
      default: // Normally not reached
        Serial.print("other  ");
        break;
    }

    Serial.print(", ");
    Serial.print(state.pwm);

    Serial.println("");
    /* ############################# */
  }

  updated = false;
  delay(20);
}













