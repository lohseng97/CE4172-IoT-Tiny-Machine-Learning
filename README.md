# NTU CE4172: Internet of Things: Tiny Machine Learning (tinyML)

## Project Description
The object of the project is to:
- XYZ

## Hardware Requirede
- Arduino Nano 33 BLE Sense
- Raspberry Pi (Preferably Pi 3 Model B and above)
- Computer with Wake-on-LAN functionality turned on and hybrid sleep turned-off
> See ```Communication between the devices``` section in ```Project Report.pdf``` for more details.

## Optional Hardware
- Touch Sensor
- RGB LED
> Note: Please remove the Touch Sensor and/or RGB LED codes from ```Codes/IMU_Classifier.ino``` if unused.

## Contents
In this submission folder, the following files are available:
- ```Presentation Slides.pptx```: Brief overview of the project, including recorded demonstration for the project.
- ```Project Report.pdf```: Detailed Report covering the data collection process, inference performance, optimization techniques and others.
- ```Codes/IMU_Capture.ino``` folder: Code to capture the acceleration and gyroscope data from on-board IMU.
- ```Codes/IMU_Classifier.ino``` folder: Codes for classification based on captured acceleration and gyroscope data. Added codes for Touch Sensor and RGB LED to allow for additional functionality over in-built components.
- ```Codes/Node-Red``` folder: Codes used to control the Raspberry Pi based on the classified results from ```Codes/IMU_Classifier.ino```.
- ```Codes/Wake on LAN(WoL).ipynb```: Code used for training of the machine learning model based on the captured data from ```Codes/IMU_Capture.ino```.

## Running the Project

