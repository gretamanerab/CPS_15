# CPS_15 — DE Arm

**Cyber Physical Systems 2026/27 — A1: Built Robot Arm and System Unit Tests Check-In**
Imperial College London & Royal College of Art — Innovation Design Engineering

## Group

Dario Cosenza, Greta Manera Bertizzolo, Selma Danesi-Vold

---

## 1. Abstract

This project explores how machine learning can control a physical mechanism in real time. We built a laser-cut robotic arm with four servo-driven joints and trained a supervised classifier in Wekinator to recognise hand gestures captured through a webcam. The gesture data is sent to Wekinator, which classifies it and passes its output to Processing. Processing sends the class number to the Arduino over serial, and the Arduino maps each recognised gesture to a target pose and smoothly moves the servos toward it. The result is an interactive system in which human movement directly controls the movement of a robotic arm.

Alongside the finished system, this repository documents the **debugging methodology** used to build and test it. Each folder is a *unit test*: a small, self-contained sketch that proves one subsystem works before the next is added. If the full system fails, the tests are run in order until the failing layer is isolated. All code is commented line by line so that any group member can open a file and follow what each section does.

---

## 2. Motivation and objectives

The project connects human input, machine learning, serial communication and physical actuation into one cyber-physical system.

1. Gain an understanding of basic dynamic components, including servo actuation, linkages and mechanical assembly.
2. Learn how to build a complete machine-learning pipeline connecting computation to a physical system.
3. Explore the relationship between human and machine learning.
4. Develop experience with Arduino programming, serial communication and non-blocking control using `millis()`.

---

## 3. Technical details

| Area | Details |
|------|---------|
| Mechanical system | Laser-cut plywood frame, links, joints and base |
| Actuation | Four servo motors controlling the arm's joints |
| Electronics | Arduino Nano microcontroller, external 5 V servo power supply, breadboard / prototyping wiring |
| Input | Hand gestures captured by webcam, tracked in Python with OpenCV and MediaPipe |
| Machine learning | Wekinator supervised classification, three classes |
| Intermediate software | Processing: receives the classifier output and writes to serial |
| Programming | Arduino C++, Processing, Python |
| Communication | OSC between the tracking script and Wekinator; serial at 9600 baud between Processing and Arduino |

### Components and materials

| Component | Specification | Function |
|-----------|---------------|----------|
| Frame and links | Laser-cut plywood | Structural support |
| Joints | Mechanical pivot joints | Allow relative movement |
| Servo motors | 4 × standard hobby servos | Actuate the arm's joints |
| Arduino Nano | ATmega328P microcontroller | Reads serial commands and controls servo positions |
| Power supply | External 5 V supply capable of powering four servos | Powers the servos |
| Webcam | Standard computer webcam | Captures hand-gesture input |
| Laptop | Runs the tracking script, Wekinator and Processing | Camera input, classification and serial communication |

---

## 4. System overview

```
[ Hand ]
   |
   v
Hand tracking (webcam)  --- 42 values via OSC --->  Wekinator
                                                        |
                                               1 output (class 1, 2 or 3)
                                                        |
                                                        v
                                                   Processing
                                                        |
                                           serial message @ 9600 baud
                                                        |
                                                        v
                                                    Arduino Nano
                                                        |
                                               4 servos (pins 9, 10, 11, 12)
                                                + external 5 V supply
```

---

## 5. How it works

### 5.1 Mechanical design

The robot arm was assembled from laser-cut plywood parts. The frame, links and base provide structural support, while the servo motors actuate the joints. The arm uses four servo motors, giving it four controllable degrees of freedom. Each servo moves between programmed angle positions rather than rotating continuously.

*(To add: the dimensions of the laser-cut parts, and the achievable range of motion of each joint in degrees.)*

### 5.2 Electronics and wiring

The four servo motors are connected to Arduino pins **9, 10, 11 and 12**. The Arduino receives commands from Processing through serial communication at **9600 baud**.

Because four servos can draw more current than the Arduino can safely supply, the servos are powered by an **external 5 V supply**. The Arduino and the external supply share a **common ground**. The Arduino itself is powered over USB from the laptop.

#### Circuit diagram

<img width="2720" height="1960" alt="circuit" src="https://github.com/user-attachments/assets/9a8c103d-7174-45a9-a324-e3bfe41cc7cc" />


### 5.3 Software and control

The system uses four pieces of software: the Python hand tracking script, Wekinator, Processing and the Arduino IDE.

The tracking script reads the landmark points of the hand from the webcam and streams them to Wekinator over OSC. Wekinator runs a supervised classifier trained on recorded examples of each gesture, and outputs a class number. Processing receives that output and sends the corresponding number to the Arduino over serial. The Arduino maps each class number to a target servo configuration:

| Class | Gesture | Arm response |
|-------|---------|--------------|
| 1 | Open hand | Pose 1 |
| 2 | Closed hand | Pose 2 |
| 3 | Index and middle finger up (peace sign) | Pose 3 |
| Any other value | — | All servos return to the neutral 90° position |

The servo positions are updated **gradually rather than instantly**. The code uses `millis()` for non-blocking timing, so it keeps checking for new serial commands while the servos are still moving. Every **10 ms** each servo moves **one degree** toward its target position. This produces smoother movement and prevents the program from becoming stuck in a `delay()`.

### 5.4 Machine learning input: why a Python script

We wanted gesture control of the arm. None of the ready-made hand tracking inputs we found would run on the laptop used for this build, so we wrote our own tracking script in Python (`hand_gestures.py`) and run it **from the terminal**.

The script uses OpenCV for the webcam feed and the MediaPipe hand landmarker to find the **21 landmark points** of one hand. Before sending, each point is processed in two ways so that the gesture, and not the position of the hand, is what the classifier learns:

1. **Translation:** the wrist (point 0) is treated as the origin, so moving the whole hand around the frame does not change the values.
2. **Scale:** every coordinate is divided by the distance from the wrist to the base of the middle finger (point 9), so moving closer to or further from the camera does not change the values.
3. **Fallback:** when no hand is visible the script sends 42 zeros, so Wekinator always receives a complete frame.

The 21 points, each with an x and a y value, give **42 inputs**, sent to Wekinator over OSC to `127.0.0.1:6448` on the address `/wek/inputs`. Wekinator is configured as:

* **Inputs:** 42
* **Outputs:** 1
* **Model type:** classifier, "all classifiers", set to **3 classes**

### 5.5 Machine learning workflow

1. Capture hand gestures with the webcam using `hand_gestures.py`.
2. Stream the 42 normalised hand landmark values to Wekinator over OSC (port 6448).
3. Record labelled training examples for each of the three gestures.
4. Train a three-class classifier.
5. Run the classifier and send its output to Processing.
6. Processing sends the class number to the Arduino.
7. The Arduino moves the robot arm to the matching pose.

---

## 6. Repository contents

| Folder | Role | Stage |
|--------|------|-------|
| [`debug_arduino_final`](debug_arduino_final) | General unit test for the Arduino and the servo outputs | Step 1 |
| [`potentiometers_debug`](potentiometers_debug) | Unit test for the potentiometers, one servo at a time | Step 2 |
| [`talk_test_arduino`](talk_test_arduino) | Arduino half of the Processing ↔ Arduino communication test | Step 3 |
| [`talk_test_processing`](talk_test_processing) | Processing half of the same test, triggered with keys 1, 2 and 3 | Step 3 |
| [`processing_final`](processing_final) | Final Processing sketch, receives from Wekinator and writes to serial | Step 4 |
| [`weki_processing_arduino_final`](weki_processing_arduino_final) | Final Arduino sketch for the complete system | Step 4 |
| [`wekinator_final`](wekinator_final) | Trained Wekinator project| Step 4 |
| [`hand_gestures.py`] | Hand tracking script. Reads the webcam, normalises the 21 hand landmarks and sends 42 values to Wekinator over OSC on port 6448. Run from the terminal. |
| [`hand_landmarker.task`] | The MediaPipe hand landmark model |

---

## 7. Running the full system

Run these in order. Do not skip ahead; each step assumes the previous one passed.

1. Connect the external power to the power board and switch it on. Connect the Arduino to the laptop by USB.
2. Upload `weki_processing_arduino_final` to the Arduino.
3. Open `processing_final` and set the correct **serial port number** (see Step 3B below).
4. Start the tracking script from the terminal (`python hand_gestures.py`) and check that the preview window reads "hand found" in green.
5. Open the trained Wekinator project. Confirm **42 inputs and 1 output**, and that **both OSC lights are green**, for OSC in and OSC out.
6. Run the Processing sketch, then press **Run** in Wekinator.
7. Perform the three gestures: open hand, closed hand, peace sign. The arm should move smoothly between the three poses.

---

# 8. Debugging methodology

The system is debugged **from the bottom up**. Start at Step 1. Only move on once the current step passes. If a later step fails, come back down to the step below it.

---

## Step 1 — General debug (physical and power)

Run this before anything else, and again every time anything is moved or unplugged.

1. Every cable on the breadboard is pushed **fully** into its row. A cable that looks seated can still be loose.
2. The plywood structure is screwed in correctly and nothing is cracked or free to rotate.
3. The arm is standing on a **stable, flat base** and will not tip when a joint moves.
4. The power board is **switched on** and connected to the battery or external supply.
5. The Arduino is connected to the laptop by USB and its power LED is on.
6. The servos are powered from the external supply, **not** from the Arduino pins, and the grounds are shared.

**Unit test:** upload `debug_arduino_final` and confirm the servos respond.

**Expected observation:** all four servos move through their range.

* If they do, go to **Step 3**.
* If they do not, or only some do, go to **Step 2**.

---

## Step 2 — Potentiometer debug (isolating the fault)

This step exists to find out *which* component is at fault when Step 1 does not work.

**Checks:** repeat every check in Step 1, then check every wire going to and from each potentiometer, including the shared ground and the 5 V line.

**Unit test:** upload `potentiometers_debug` and test **one servo at a time**, turning each potentiometer individually through its full range.

**Decision tree — when one servo does not respond:**

1. **Check the code first.** Open the sketch and confirm the pin numbers and the value mapping for that servo. The code is fully commented, so the relevant line is quick to find.
2. **If the code is correct, check the wiring** of that potentiometer and that servo: signal, 5 V and ground, at both the breadboard and the component.
3. **If the wiring is correct, the fault is the component itself** — that specific servo or that specific potentiometer. Swap it with a known working one to confirm.

The value of this test is that it narrows a whole non-functioning arm down to a single identified part.

**Expected observation:** each of the four servos moves independently, over its entire expected range, when its own potentiometer is turned.

---

## Step 3 — Messaging debug (Arduino and Processing talking)

**Unit test:** two files that must both be running.

1. Upload `talk_test_arduino` to the Arduino.
2. Open and run `talk_test_processing`.
3. Press the keys **1**, **2** and **3** in the Processing window.

**Expected observation:** each key press sends a message to the Arduino, the Arduino acts on it and sends a signal back, and the reply is printed in the Processing console. Seeing the reply confirms three things at once: the serial link is open, the Arduino is receiving, and the Arduino is running correctly.

**If nothing happens, it is almost always the serial port.** See Step 3B.

---

## Step 3B — Serial port check (applies to Step 3 and Step 4)

The serial port number is **different on every laptop**, so it has to be set on whichever machine is being used.

1. Run the Processing sketch and look at the bottom of the Processing console. It prints the full list of available ports, numbered from 0.
2. Find the number of the port the Arduino is connected to.
3. In the code, find the line where the serial connection is opened (`new Serial(...)`) and make the index match that number.
4. Close the Arduino Serial Monitor if it is open. Only one program can use the port at a time.

Any failure in Step 3 or Step 4 where nothing at all reaches the Arduino should be treated as a wrong port until proven otherwise.

---

## Step 4 — Full system debug (Wekinator)

**Running the tracking script**

```bash
pip install opencv-python mediapipe python-osc
python hand_gestures.py
```

A window opens showing the webcam feed with the tracked points drawn on the hand. It reads **"hand found"** in green when a hand is detected and **"no hand"** in red when it is not. Press **q** to quit.

**Unit test:** `weki_processing_arduino_final` + `processing_final` + `wekinator_final`.

1. Upload `weki_processing_arduino_final` to the Arduino.
2. Start the tracking script from the terminal (`python hand_gestures.py`).
3. **Open Wekinator first.** Check that the inputs and outputs are set correctly (42 in, 1 out, classifier with 3 classes). They should already be saved in the project file, but confirm them.
4. Check that **both OSC lights are green**, for OSC in and OSC out. If the OSC in light is not green, work through three things: the script window is not showing "hand found", the port is not 6448, or `hand_landmarker.task` is missing from the script's folder so the script never started.
5. Run the Processing sketch.
6. Press **Run** in Wekinator. The model is already trained.
7. Perform the three gestures.

**Decision tree — if the system does not respond:**

1. **Nothing reaches the Arduino at all** → wrong serial port in Processing. Go to **Step 3B**.
2. **Wekinator shows no input** → the tracking script is not running, no hand is in frame, or the input count is not 42.
3. **The arm responds but the movement is not smooth** → see Step 5.

---

## Step 5 — Movement quality debug (power)

**Symptom:** the gestures are recognised and the arm moves, but the movement stutters or is weak.

This is a **power** problem, not a code problem. A loose wire on the power board is drawing the supply down, so the servos receive around 5 V when they need slightly more.

1. Reseat every wire on the power board, especially the main supply rails.
2. Confirm the battery or external supply is charged and switched on.
3. Measure the voltage reaching the servos and confirm it sits a little above 5 V rather than at or below it.

Then repeat **Step 1** and work back up.

---

## 9. Quick reference: the order

| Step | Test | Folder | Run it when |
|------|------|--------|-------------|
| 1 | General debug | `debug_arduino_final` | Always first |
| 2 | Potentiometer debug | `potentiometers_debug` | Step 1 fails, or to isolate one faulty part |
| 3 | Messaging debug | `talk_test_arduino` + `talk_test_processing` | Before using the laptop to control the arm |
| 3B | Serial port check | — | Any time Processing and Arduino do not talk |
| 4 | Full system | `weki_processing_arduino_final` + `processing_final` + `wekinator_final` | Once Steps 1 to 3 pass |
| 5 | Power / smoothness | — | The arm moves but not smoothly |

**Rule of thumb:** every time you move the arm, re-plug anything, or change laptop, check the wires and the port before assuming the code is broken.

---

## 10. Operations, testing and results

To test the system we first checked that each servo could move independently and reach its programmed positions. We then tested serial communication by sending class values manually from Processing to the Arduino. Finally, we trained and tested the Wekinator model using hand gestures captured by the webcam.

**Results:**

1. The arm responded to the three trained hand gestures.
2. All four servos moved smoothly between programmed positions.
3. Each servo could be actuated independently over its full expected range.

*(Optional metrics to add: classification accuracy over a set number of attempts per gesture, repeatability of each pose, and the measured range of motion of each joint.)*

---

## 11. Limitations and future work

**Limitations**

1. Classification depends on lighting and on the hand staying within the webcam frame, so recognition degrades in poor conditions.
2. Only three gestures are trained, so the range of control is limited to three fixed poses rather than continuous movement.
3. The arm is powered from an external supply on the bench, so the system is not portable, and the plywood structure limits the load the arm can carry.

**Future work**

1. Move from classification to regression so that hand position maps continuously to joint angles rather than to fixed poses.
2. Add position feedback or end-stops so the arm knows where it actually is rather than where it was commanded to go.
3. Strengthen the joints and integrate the wiring and power into the base for a self-contained unit.

