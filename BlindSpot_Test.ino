// ==========================================
// BLIND SPOT HELMET - LOW FIDELITY PROTOTYPE
// 2 Ultrasonic Sensors + 2 LEDs + Buzzer + Push Button
// ==========================================


// ---------- SENSOR 1 ----------
const int trig1 = 9;
const int echo1 = 10;


// ---------- SENSOR 2 ----------
const int trig2 = 11;
const int echo2 = 12;


// ---------- LEDs ----------
const int led1 = 3;
const int led2 = 4;


// ---------- BUZZER ----------
const int buzzerPin = 7;   // Buzzer (+) to D7, (-) to GND.


// ---------- BUTTON ----------
const int buttonPin = 2;


// ---------- SETTINGS ----------

// Warning distance in cm
const int TRIGGER_CM = 100;


// ---------- STATES ----------

// false = warnings active
// true  = warnings muted
bool muted = false;


// Used for LED blinking
bool blinkState = false;

unsigned long previousBlinkTime = 0;

const unsigned long blinkInterval = 300;


// ==========================================
// FUNCTION TO MEASURE DISTANCE
// ==========================================

float readCm(int trig, int echo)
{
  // Ensure trigger starts LOW
  digitalWrite(trig, LOW);
  delayMicroseconds(2);


  // Send ultrasonic pulse
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(trig, LOW);


  // Wait for echo
  long duration =
      pulseIn(echo, HIGH, 30000);


  // No echo detected
  if (duration == 0)
  {
    return 999;
  }


  // Convert echo time into centimetres
  float distance =
      duration * 0.0343 / 2;

  return distance;
}


// ==========================================
// SETUP
// ==========================================

void setup()
{
  Serial.begin(9600);


  // Sensor 1
  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);


  // Sensor 2
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);


  // LEDs
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);


  // Buzzer
  pinMode(buzzerPin, OUTPUT);


  // Push button
  // Button connects D2 to GND when pressed
  pinMode(buttonPin, INPUT_PULLUP);


  // Start LEDs and Buzzer OFF
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(buzzerPin, LOW);


  Serial.println("Blind Spot System Started");
}


// ==========================================
// MAIN LOOP
// ==========================================

void loop()
{

  // ----------------------------------------
  // CHECK PUSH BUTTON
  // ----------------------------------------

  if (digitalRead(buttonPin) == LOW)
  {
    // Small delay to avoid false multiple presses
    delay(30);


    if (digitalRead(buttonPin) == LOW)
    {
      // Toggle mute state
      muted = !muted;


      if (muted)
      {
        Serial.println("WARNING MUTED");
      }
      else
      {
        Serial.println("WARNING ENABLED");
      }


      // Wait until user releases button
      while (digitalRead(buttonPin) == LOW)
      {
        delay(10);
      }
    }
  }


  // ----------------------------------------
  // READ SENSOR 1
  // ----------------------------------------

  float distance1 =
      readCm(trig1, echo1);


  // Prevent ultrasonic cross-talk
  delay(50);


  // ----------------------------------------
  // READ SENSOR 2
  // ----------------------------------------

  float distance2 =
      readCm(trig2, echo2);


  // ----------------------------------------
  // CHECK DANGER
  // ----------------------------------------

  bool danger1 =
      distance1 < TRIGGER_CM;

  bool danger2 =
      distance2 < TRIGGER_CM;


  // ----------------------------------------
  // CREATE BLINKING EFFECT
  // ----------------------------------------

  unsigned long currentTime =
      millis();


  if (currentTime - previousBlinkTime
      >= blinkInterval)
  {
    previousBlinkTime =
        currentTime;

    blinkState =
        !blinkState;
  }


  // ----------------------------------------
  // CONTROL LED 1
  // ----------------------------------------

  if (!muted && danger1)
  {
    digitalWrite(
      led1,
      blinkState ? HIGH : LOW
    );
  }
  else
  {
    digitalWrite(
      led1,
      LOW
    );
  }


  // ----------------------------------------
  // CONTROL LED 2
  // ----------------------------------------

  if (!muted && danger2)
  {
    digitalWrite(
      led2,
      blinkState ? HIGH : LOW
    );
  }
  else
  {
    digitalWrite(
      led2,
      LOW
    );
  }


  // ----------------------------------------
  // CONTROL BUZZER
  // ----------------------------------------

  if (!muted && (danger1 || danger2))
  {
    digitalWrite(buzzerPin, HIGH);
  }
  else
  {
    digitalWrite(buzzerPin, LOW);
  }


  // ----------------------------------------
  // SERIAL MONITOR OUTPUT
  // ----------------------------------------

  Serial.print("S1: ");
  Serial.print(distance1);
  Serial.print(" cm");


  if (danger1)
  {
    Serial.print(" [WARNING]");
  }


  Serial.print("    S2: ");
  Serial.print(distance2);
  Serial.print(" cm");


  if (danger2)
  {
    Serial.print(" [WARNING]");
  }


  if (muted)
  {
    Serial.print("    [MUTED]");
  }


  Serial.println();


  delay(50);
}