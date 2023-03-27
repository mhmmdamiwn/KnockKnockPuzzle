// Define the debugging flag to help with debugging
#define DEBUG

// Define the pins used for the sensor, LED, and buzzer
const byte sensorPin = A0;
const byte ledPin = 13;
const byte buzzerPin = 12;

// Define the secret knock pattern to listen for
const int numKnocksInPattern = 10;
const int secretKnockPattern[numKnocksInPattern - 1] = {512, 1024, 512, 1024, 512, 1024, 512, 1024, 1024};

// Define the sensor threshold and various tolerances
const int sensorThreshold = 15;
const int knockErrorTolerance = 128;
const int averageKnockErrorTolerance = 128;
const int minGapBetweenKnocks = 125;
const int maxGapBetweenKnocks = 1500;

// Define variables for storing the sensor's base reading and the input pattern
int baseReading = 512;
int inputPattern[numKnocksInPattern - 1];

// Setup function that runs once at the beginning
void setup() {
  // Initialize serial communication
  Serial.begin(9600);

#ifdef DEBUG
  Serial.println(F("Entering Setup"));
#endif

  // Set the pin modes
#ifdef DEBUG
  Serial.println(F("Setting pin modes"));
  delay(500);
#endif
  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Play back the secret knock pattern
#ifdef DEBUG
  Serial.println(F("Playing back knock pattern"));
  delay(500);
#endif
  playbackKnockPattern(secretKnockPattern, 500);

#ifdef DEBUG
  // Calibrate the sensor
  Serial.println(F("Calibrating sensor"));
#endif
  int numSamples = 10;
  int sumReadings = 0;
  for (int i = 0; i < numSamples; i++) {
    sumReadings += analogRead(sensorPin);
    delay(100);
  }

  float avgReading = (float)sumReadings / numSamples;
  baseReading = avgReading;

#ifdef DEBUG
  // Print the sensor's base reading and complete the setup
  Serial.print(F("Sensor base reading: "));
  Serial.println(avgReading);
  Serial.println(F("Setup complete"));
#endif
}

// Main loop function that runs repeatedly
void loop() {
  // If a knock is detected, listen for the knock pattern
  if (knockDetected()) {
    listenToKnockPattern();

#ifdef DEBUG
    // Print the timings before and after normalizing the input pattern
    Serial.print(F("Timings before normalization: "));
    printPattern(inputPattern);
#endif

    normaliseKnockPattern(inputPattern);

#ifdef DEBUG
    Serial.print(F("Timings after normalization: "));
    printPattern(inputPattern);
    Serial.print(F("Correct normalized timings: "));
    printPattern(secretKnockPattern);
#endif

    // Compare the input pattern with the secret knock pattern
    if (comparePattern(inputPattern, secretKnockPattern)) {
      onPuzzleSolved();
    } else {
      onIncorrectInput();
    }
  }
}

// Function to detect if a knock has occurred
bool knockDetected() {
  int knockSensorValue = analogRead(sensorPin);
  if (abs(knockSensorValue - baseReading) > sensorThreshold) {
    return true;
  } else {
    return false;
  }
}

// This function compares the input pattern received from the user with the secret pattern.
// It returns true if the input pattern matches the secret pattern within the specified error tolerances.
// It returns false otherwise.
bool comparePattern(int inputPattern[], int secretKnockPattern[]) {

// Check if the input pattern has any missing knocks. Return false if it does.
for (int i = 0; i < numKnocksInPattern-1; i++) {
if (inputPattern[i] == 0) {
#ifdef DEBUG
Serial.print(F("TEST #1 FAILED. input pattern had"));
Serial.print(i+1);
Serial.print(F("knocks.Secret pattern has"));
Serial.print(numKnocksInPattern);
Serial.println(F(" knocks."));
#endif
return false;
}
}

// Compare the timing of each knock in the input pattern with the secret pattern.
// Return false if the timing difference between any two corresponding knocks is greater than the specified tolerance.
int totalDelta = 0;
for (int i = 0; i < numKnocksInPattern-1; i++) {
int knockDelta = abs(inputPattern[i] - secretKnockPattern[i]);
if (knockDelta > knockErrorTolerance) {
#ifdef DEBUG
Serial.print(F("TEST #2 FAILED. input pattern had delay after knock#"));
Serial.print(i+1);
Serial.print(F(" of "));
Serial.print(inputPattern[i]);
Serial.print(F(". Secret pattern has "));
Serial.print(secretKnockPattern[i]);
Serial.println(F("."));
#endif
return false;
}
totalDelta += knockDelta;
}

// Calculate the average timing error of the input pattern and compare it with the specified tolerance.
// Return false if the average error is greater than the specified tolerance.
if (totalDelta / numKnocksInPattern > averageKnockErrorTolerance) {
#ifdef DEBUG
Serial.print(F("TEST #3 FAILED. input pattern had average timing error of "));
Serial.print(totalDelta / numKnocksInPattern);
Serial.print(F(".Secret pattern had"));
Serial.print(averageKnockErrorTolerance);
Serial.println(F("ms."));
#endif
return false;
}

// If none of the above conditions are met, the input pattern matches the secret pattern within the specified tolerances.
return true;
}


// Function to listen for the knock pattern
void listenToKnockPattern() {
#ifdef DEBUG
Serial.println(F("Listening for knock pattern"));
#endif
int currentKnockIndex = 0;
unsigned long currentKnockTime = 0;
while (currentKnockIndex < numKnocksInPattern - 1) {
// Wait for a knock to occur
int knockSensorValue = analogRead(sensorPin);
if (abs(knockSensorValue - baseReading) > sensorThreshold) {
#ifdef DEBUG
Serial.println(F("Knock detected"));
#endif
// Record the time of the knock
inputPattern[currentKnockIndex] = millis() - currentKnockTime;
currentKnockTime = millis();

#ifdef DEBUG
// Print the timing of the knock
Serial.print(F("Knock timing: "));
Serial.println(inputPattern[currentKnockIndex]);
#endif
  // Increment the knock index
  currentKnockIndex++;

  // Wait for a gap between knocks
  while (abs(analogRead(sensorPin) - baseReading) < sensorThreshold) {
    delay(10);
  }
}
}
}


// Function to normalize the knock pattern timings
void normaliseKnockPattern(int pattern[]) {
#ifdef DEBUG
Serial.println(F("Normalizing pattern timings"));
#endif
// Determine the average timing of the pattern
int totalPatternTime = 0;
for (int i = 0; i < numKnocksInPattern - 1; i++) {
totalPatternTime += pattern[i];
}
int averagePatternTime = totalPatternTime / (numKnocksInPattern - 1);

// Normalize the pattern timings around the average
for (int i = 0; i < numKnocksInPattern - 1; i++) {
pattern[i] = pattern[i] * averagePatternTime / totalPatternTime;
}

#ifdef DEBUG
// Print the normalized pattern timings
Serial.print(F("Normalized pattern timings: "));
printPattern(pattern);
#endif
}

// Function to play back the knock pattern
void playbackKnockPattern(int pattern[], int delayTime) {
#ifdef DEBUG
Serial.println(F("Playing back knock pattern"));
#endif
for (int i = 0; i < numKnocksInPattern - 1; i++) {
tone(buzzerPin, 1000, pattern[i]);
digitalWrite(ledPin, HIGH);
delay(pattern[i] + delayTime);
digitalWrite(ledPin, LOW);
}
}

// Function to print a pattern of knock timings
void printPattern(int pattern[]) {
for (int i = 0; i < numKnocksInPattern - 1; i++) {
Serial.print(pattern[i]);
Serial.print(F(", "));
}
Serial.println();
}

// Function to handle what happens when the puzzle is solved
void onPuzzleSolved() {
#ifdef DEBUG
Serial.println(F("Puzzle solved!"));
#endif
// Blink the LED 3 times and play the success melody
for (int i = 0; i < 3; i++) {
digitalWrite(ledPin, HIGH);
tone(buzzerPin, 2000, 200);
delay(200);
digitalWrite(ledPin, LOW);
delay(200);
}
delay(500);
for (int i = 0; i < numKnocksInPattern - 1; i++) {
tone(buzzerPin, 1000, 100);
digitalWrite(ledPin, HIGH);
delay(100);
digitalWrite(ledPin, LOW);
delay(100);
}
