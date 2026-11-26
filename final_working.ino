// ==========================================
// BLIND SPOT HELMET - COMBINED SYSTEM
// ==========================================

// ---------- ULTRASONIC SENSORS ----------
const int trigL = 9;
const int echoL = 10;

const int trigR = 11;
const int echoR = 12;

// ---------- LEDs ----------
const int ledL = 4;
const int ledR = 3;

// ---------- BUZZER ----------
const int buzzer = 8;          // Updated to D8
const int buzzerFreq = 2000;   // 2000 Hz beep tone

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
const unsigned long buzzerInterval = 500; // 0.5 sec ON / 0.5 sec OFF beep

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
  tone(buzzer, buzzerFreq); // Beep on D7 during startup test
  digitalWrite(vibrationMotor, HIGH);

  delay(1000);

  // Turn everything OFF
  digitalWrite(ledL, LOW);
  digitalWrite(ledR, LOW);
  noTone(buzzer);
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
    noTone(buzzer);

    Serial.println(muted ? "WARNING MUTED" : "WARNING ENABLED");
    delay(200);
  }
  lastBtn = b;

  // ---------- READ SENSORS ----------
  float leftDistance = readCm(trigL, echoL);
  delay(50);
  float rightDistance = readCm(trigR, echoR);

  // ---------- OBJECT DETECTION ----------
  bool leftOn = (leftDistance > 0 && leftDistance < LIMIT_CM);
  bool rightOn = (rightDistance > 0 && rightDistance < LIMIT_CM);
  bool danger = leftOn || rightOn;

  // ---------- LED BLINKING ----------
  unsigned long currentTime = millis();

  if (currentTime - previousLedTime >= ledInterval)
  {
    previousLedTime = currentTime;
    ledBlinkState = !ledBlinkState;
  }

  digitalWrite(ledL, (!muted && leftOn && ledBlinkState) ? HIGH : LOW);
  digitalWrite(ledR, (!muted && rightOn && ledBlinkState) ? HIGH : LOW);

  // ---------- BUZZER ALERT (BEEP) ----------
  if (danger && !muted)
  {
    if (currentTime - previousBuzzerTime >= buzzerInterval)
    {
      previousBuzzerTime = currentTime;
      buzzerState = !buzzerState;
    }

    if (buzzerState)
    {
      tone(buzzer, buzzerFreq);
    }
    else
    {
      noTone(buzzer);
    }
  }
  else
  {
    noTone(buzzer);
    buzzerState = false;
    previousBuzzerTime = currentTime;
  }

  // ---------- COIN VIBRATION MOTOR ----------
  digitalWrite(vibrationMotor, (danger && !muted) ? HIGH : LOW);

  // ---------- SERIAL MONITOR ----------
  Serial.print("LEFT: ");
  Serial.print(leftDistance);
  Serial.print(" cm ");
  if (leftOn) Serial.print("[LEFT WARNING]");

  Serial.print("   RIGHT: ");
  Serial.print(rightDistance);
  Serial.print(" cm ");
  if (rightOn) Serial.print("[RIGHT WARNING]");

  if (muted) Serial.print("   [MUTED]");
  else if (danger) Serial.print("   [OBJECT DETECTED]");
  else Serial.print("   [SAFE]");

  Serial.println();
  delay(50);
}
