// Helmet Blind Spot Indicator: LEDs + Buzzer + Mute Button

const int trigL = 9, echoL = 10;
const int trigR = 11, echoR = 12;

const int ledL = 3, ledR = 4;
const int buzzer = 5;
const int btn = 2;

const int LIMIT_CM = 100;

bool muted = false;
bool lastBtn = HIGH;

unsigned long previousBuzzerTime = 0;
bool buzzerState = false;

float readCm(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(trig, LOW);

  long t = pulseIn(echo, HIGH, 30000);

  if (t == 0) return 999;

  return t * 0.0343 / 2;
}

void setup() {
  Serial.begin(9600);

  pinMode(trigL, OUTPUT);
  pinMode(echoL, INPUT);

  pinMode(trigR, OUTPUT);
  pinMode(echoR, INPUT);

  pinMode(ledL, OUTPUT);
  pinMode(ledR, OUTPUT);

  pinMode(buzzer, OUTPUT);
  pinMode(btn, INPUT_PULLUP);

  // Startup test: both LEDs ON + buzzer ON
  digitalWrite(ledL, HIGH);
  digitalWrite(ledR, HIGH);

  analogWrite(buzzer, 255);
  delay(1000);

  // Turn everything OFF
  analogWrite(buzzer, 0);
  digitalWrite(ledL, LOW);
  digitalWrite(ledR, LOW);
}

void loop() {

  // Mute button
  bool b = digitalRead(btn);

  if (lastBtn == HIGH && b == LOW) {
    muted = !muted;
    buzzerState = false;
    analogWrite(buzzer, 0);
    delay(200);
  }

  lastBtn = b;

  // Read both sensors
  float l = readCm(trigL, echoL);
  delay(30);
  float r = readCm(trigR, echoR);

  // Detect objects within 100 cm
  bool leftOn = (l > 0 && l < LIMIT_CM);
  bool rightOn = (r > 0 && r < LIMIT_CM);

  // LEDs
  digitalWrite(ledL, leftOn);
  digitalWrite(ledR, rightOn);

  // Buzzer alert: 1 second ON / 1 second OFF
  if ((leftOn || rightOn) && !muted) {

    unsigned long currentTime = millis();

    if (currentTime - previousBuzzerTime >= 1000) {
      previousBuzzerTime = currentTime;
      buzzerState = !buzzerState;
    }

    if (buzzerState) {
      analogWrite(buzzer, 255);
    } else {
      analogWrite(buzzer, 0);
    }

  } else {
    analogWrite(buzzer, 0);
    buzzerState = false;
    previousBuzzerTime = millis();
  }

  // Serial Monitor
  Serial.print("L: ");
  Serial.print(l);
  Serial.print(" cm   R: ");
  Serial.print(r);
  Serial.print(" cm   ");

  Serial.println(muted ? "MUTED" : "Alert Active");

  delay(100);
}
