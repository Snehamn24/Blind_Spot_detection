
// ==========================================
// BLIND SPOT HELMET - COMBINED SYSTEM
// 2 Ultrasonic Sensors
// 2 LEDs
// 1 Buzzer
// 1 Coin Vibration Motor
// 1 Push Button
// ==========================================

// ---------- ULTRASONIC SENSORS ----------

const int trigL = 9;
const int echoL = 10;

const int trigR = 11;
const int echoR = 12;


// ---------- LEDs ----------

const int ledL = 3;
const int ledR = 4;


// ---------- BUZZER ----------

const int buzzer = 5;


// ---------- COIN VIBRATION MOTOR ----------

const int vibrationMotor = 6;


// ---------- PUSH BUTTON ----------

const int btn = 2;


// ---------- SETTINGS ----------

const int LIMIT_CM = 100;


// ---------- STATES ----------

bool muted = false;
bool lastBtn = HIGH;

bool ledBlinkState = false;
bool buzzerState = false;

unsigned long previousLedTime = 0;
unsigned long previousBuzzerTime = 0;

const unsigned long ledInterval = 300;
const unsigned long buzzerInterval = 1000;


// ==========================================
// DISTANCE MEASUREMENT FUNCTION
// ==========================================

float readCm(int trig, int echo)
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(trig, LOW);

  long t = pulseIn(echo, HIGH, 30000);

  if (t == 0)
  {
    return 999;
  }

  return t * 0.0343 / 2;
}


// ==========================================
// SETUP
// ==========================================

void setup()
{
  Serial.begin(9600);

  // Ultrasonic sensors
  pinMode(trigL, OUTPUT);
  pinMode(echoL, INPUT);

  pinMode(trigR, OUTPUT);
  pinMode(echoR, INPUT);

  // LEDs
  pinMode(ledL, OUTPUT);
  pinMode(ledR, OUTPUT);

  // Buzzer
  pinMode(buzzer, OUTPUT);

  // Vibration motor
  pinMode(vibrationMotor, OUTPUT);

  // Push button
  pinMode(btn, INPUT_PULLUP);


  // ---------- STARTUP TEST ----------

  Serial.println("Blind Spot Helmet System Started");

  digitalWrite(ledL, HIGH);
  digitalWrite(ledR, HIGH);

  digitalWrite(buzzer, HIGH);
  digitalWrite(vibrationMotor, HIGH);

  delay(1000);

  // Turn everything OFF

  digitalWrite(ledL, LOW);
  digitalWrite(ledR, LOW);

  digitalWrite(buzzer, LOW);
  digitalWrite(vibrationMotor, LOW);
}


// ==========================================
// MAIN LOOP
// ==========================================

void loop()
{
  // ---------- PUSH BUTTON / MUTE ----------

  bool b = digitalRead(btn);

  if (lastBtn == HIGH && b == LOW)
  {
    muted = !muted;

    buzzerState = false;

    digitalWrite(buzzer, LOW);

    Serial.println(muted ? "WARNING MUTED" :
                            "WARNING ENABLED");

    delay(200);
  }

  lastBtn = b;


  // ---------- READ LEFT SENSOR ----------

  float leftDistance = readCm(trigL, echoL);

  delay(50);


  // ---------- READ RIGHT SENSOR ----------

  float rightDistance = readCm(trigR, echoR);


  // ---------- OBJECT DETECTION ----------

  bool leftOn =
    (leftDistance > 0 && leftDistance < LIMIT_CM);

  bool rightOn =
    (rightDistance > 0 && rightDistance < LIMIT_CM);

  bool danger = leftOn || rightOn;


  // ---------- LED BLINKING ----------

  unsigned long currentTime = millis();

  if (currentTime - previousLedTime >= ledInterval)
  {
    previousLedTime = currentTime;
    ledBlinkState = !ledBlinkState;
  }


  // Left LED

  if (!muted && leftOn)
  {
    digitalWrite(ledL, ledBlinkState ? HIGH : LOW);
  }
  else
  {
    digitalWrite(ledL, LOW);
  }


  // Right LED

  if (!muted && rightOn)
  {
    digitalWrite(ledR, ledBlinkState ? HIGH : LOW);
  }
  else
  {
    digitalWrite(ledR, LOW);
  }


  // ---------- BUZZER ALERT ----------
  // 1 second ON / 1 second OFF

  if (danger && !muted)
  {
    if (currentTime - previousBuzzerTime >= buzzerInterval)
    {
      previousBuzzerTime = currentTime;
      buzzerState = !buzzerState;
    }

    digitalWrite(buzzer, buzzerState ? HIGH : LOW);
  }
  else
  {
    digitalWrite(buzzer, LOW);
    buzzerState = false;
    previousBuzzerTime = currentTime;
  }


  // ---------- COIN VIBRATION MOTOR ----------

  if (danger && !muted)
  {
    digitalWrite(vibrationMotor, HIGH);
  }
  else
  {
    digitalWrite(vibrationMotor, LOW);
  }


  // ---------- SERIAL MONITOR ----------

  Serial.print("LEFT: ");
  Serial.print(leftDistance);
  Serial.print(" cm ");

  if (leftOn)
  {
    Serial.print("[LEFT WARNING]");
  }

  Serial.print("   RIGHT: ");
  Serial.print(rightDistance);
  Serial.print(" cm ");

  if (rightOn)
  {
    Serial.print("[RIGHT WARNING]");
  }

  if (muted)
  {
    Serial.print("   [MUTED]");
  }
  else if (danger)
  {
    Serial.print("   [OBJECT DETECTED]");
  }
  else
  {
    Serial.print("   [SAFE]");
  }

  Serial.println();

  delay(50);
}
