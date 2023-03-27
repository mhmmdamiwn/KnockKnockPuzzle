# KnockKnockPuzzle
This project implements a lock system that is unlocked when a specific secret knock pattern is detected.

# Hardware Requirements
Arduino UNO or similar board
Piezo buzzer
LED
Piezo sensor
1k ohm resistor (for LED)

# Wiring
The following pins should be connected:

Piezo sensor: A0
LED: 13
Piezo buzzer: 12
The LED should be connected to ground through the 1k ohm resistor.

# Functionality
The system listens for knocks using the piezo sensor. Once a knock is detected, the system listens for a pattern of knocks that match a predefined secret pattern.

If the detected pattern matches the secret pattern within the specified tolerances, the system unlocks and the LED turns on. Otherwise, the buzzer sounds to indicate an incorrect input.

# Code
The code consists of the following main functions:

setup(): Initializes the pins, plays the secret knock pattern, and calibrates the sensor.

loop(): Listens for knocks and compares the input pattern to the secret pattern.

knockDetected(): Detects if a knock has occurred by reading the piezo sensor.

listenToKnockPattern(): Listens for the pattern of knocks after a knock has been detected.

normaliseKnockPattern(): Normalizes the timings of the input pattern.

comparePattern(): Compares the input pattern to the secret pattern within the specified tolerances.

onPuzzleSolved(): Handles the unlocking of the system and turning on the LED.

onIncorrectInput(): Handles an incorrect input by sounding the buzzer.
Debugging information can be printed to the serial monitor by defining the DEBUG flag. This is useful for troubleshooting the system.

# Usage
Upload the code to the Arduino board.
Connect the circuit according to the wiring instructions.
Play the secret knock pattern to the piezo sensor to calibrate the sensor.
Knock the secret pattern to unlock the system.
