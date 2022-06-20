# NTU CE4172: Internet of Things: Tiny Machine Learning (tinyML)

## Project Description
The objective of the project is to:
- Use a low-power solution to turn on a Wake-on-LAN (WoL) compatible computer
> Useful when the computer has to be on remotely or if the power button is faulty.
- Understand the advantages and limitations of implementing Artificial Intelligence (AI) in a low-powered embedded system
> Details are included in ```Project Report.pdf```.

## Hardware Required
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
1. Clone the repository or download the project code to your preferred disk location.
2. Connect the Arduino Nano Sense board to the computer and install the required drivers.
3. Open ```IMU_Caputer.ino.ino``` file, flash the code and capture the data.
4. Store all the captured data into a csv file.
5. Repeat steps 3 & 4 for the different kind of gestures you want to capture.
6. Open the ```Wake on LAN(WoL).ipynb```, modify the code to read from the csv files you captured, and do the training. 
> Note: Training can take a long time, so do consider running the code remotely through Google Colab with GPU/TPU on.
7. Save the models, and convert the selected model into the HEX file.
> Reference code is available in the ```Wake on LAN(WoL).ipynb``` file.
8. Import the HEX file into ```IMU_Classifier.ino.ino``` file, and modify the code to suit your gestures and flash the code into the Arudino Nano Sense board.
9. Connect the Raspberry Pi to the Arduino Nano Sense, install ```Node-Red``` and ```etherwake```, and import the codes in the ```Codes/Node-Red``` folder into the Raspberry Pi.
10. Open ```wol.sh``` file, change the ```XX:XX:XX:XX:XX:XX``` to the targeted MAC address. Ensure the computer's LAN port allows for Wake-on-LAN functionaity. 
11. Enjoy the remote WoL!
