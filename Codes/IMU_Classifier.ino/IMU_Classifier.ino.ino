//Import Arduino IMU
  #include <Arduino_LSM9DS1.h>

//Import TensorFlow Lite Libraries
  #include <TensorFlowLite.h>
  #include <tensorflow/lite/micro/all_ops_resolver.h>
  #include <tensorflow/lite/micro/micro_error_reporter.h>
  #include <tensorflow/lite/micro/micro_interpreter.h>
  #include <tensorflow/lite/schema/schema_generated.h>
  #include <tensorflow/lite/version.h>
  #include "DebugLog.h"

//Include model
  #include "gesture_model_full_quan.h"

// Set Minimum Acceleration (in Gs) and Sample Counts
  const float accelerationThreshold = 2.5;
  const int numSamples = 119;
  int samplesRead = numSamples;

// For TFLite
  // global variables used for TensorFlow Lite (Micro)
  tflite::MicroErrorReporter tflErrorReporter;
  
  // pull in all the TFLM ops, you can remove this line and
  // only pull in the TFLM ops you need if would like to reduce
  // the compiled size of the sketch.
  tflite::AllOpsResolver tflOpsResolver;
  
  const tflite::Model* tflModel = nullptr;
  tflite::MicroInterpreter* tflInterpreter = nullptr;
  TfLiteTensor* tflInputTensor = nullptr;
  TfLiteTensor* tflOutputTensor = nullptr;
  
  // Create a static memory buffer for TFLM, the size may need to
  // be adjusted based on the model you are using
  constexpr int tensorArenaSize = 8 * 1024;
  byte tensorArena[tensorArenaSize] __attribute__((aligned(16)));
  
  // array to map gesture index to a name
  const char* GESTURES[] = {
    "Twisting", "LiftUpDown"
  };
  
  #define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

  int startTime = 0;
  int endTime = 0;
  int infTime = 0;

//For Touch Sensor
  unsigned long prevMS = 0;   //For previous millis()
  unsigned long elapMS = 0;   //For elapse millis since previous touch
  int ledState = LOW;           //Set current LED State (default: off)
  int debounceTime = 2000;     //Min period between each touch, i.e. debounce period
  int touchPin = 8;

// For RGB LED - Can mix and match for different colours
  int redPin = 7;       // For Red LED
  int greenPin = 6;     // For Green LED
  int bluePin = 5;      // For Blue LED

//For LED_BUILTIN: There is no need to setup at all, as it is pre-configured in Arduino Board.

//To Sending of Message(s) to Raspberry Pi
  String wake = "<turnon>";
  String shut = "<shutdown>";
  String reboot = "<reboot>";
  int inactivityTime = 10000;     //Disable first lift up command if >10 seconds

void setup() {
  //Set up inbuilt and RGB LED
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  //Notify user setting up in progress
  setColour(255,255,0,1500);   //Yellow

  //Set up Serial Port and off LED during setup
  Serial.begin(9600);

  //Set up Touch Sensor
  pinMode(touchPin, INPUT);

  //Set up IMU
  // initialize the IMU
  if (!IMU.begin()) {
    setColour(255,0,0,5000);     // Error LED
    while (1);
  }

  // get the TFL representation of the model byte array
  tflModel = tflite::GetModel(gesture_model_full_quan);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println(shut);         //Shutdown Pi when error detected
    setColour(255,0,0,5000);      // Error LED
    while (1);
  }

  // Create an interpreter to run the model
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);

  // Allocate memory for the model's input and output tensors
  tflInterpreter->AllocateTensors();

  // Get pointers for the model's input and output tensors
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);

  // To show setup is complete
  setColour(128,0,128,1500);   //Violet
}

void loop() {
  float aX, aY, aZ, gX, gY, gZ;

  elapMS = millis() - prevMS;      //Calculate elapsed milliseconds

  //Using Touch Sensor to activate Raspberry Pi
  //digitalRead(touchPin) == HIGH -> Touch Sensor is activated
  //elapMS >= debounceTime -> Make sure the previous touch detected was min 500ms before current touch
  if (digitalRead(touchPin) == HIGH && elapMS > debounceTime){
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println(wake);
    setColour(255,128,0,500);   //Orange
    prevMS = millis();
  }

  // Using IMU to activate Raspberry Pi
  // wait for significant motion
  else
  if (samplesRead == numSamples) {
      if (IMU.accelerationAvailable()) {
        // read the acceleration data
        IMU.readAcceleration(aX, aY, aZ);
  
        // sum up the absolutes
        float aSum = fabs(aX) + fabs(aY) + fabs(aZ);
  
        // check if it's above the threshold
        if (aSum >= accelerationThreshold) {
          // reset the sample read count
          samplesRead = 0;
        }
      }
    }

  // check if the all the required samples have been read since
  // the last time the significant motion was detected
  else if (samplesRead < numSamples) {
    // check if new acceleration AND gyroscope data is available
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      // read the acceleration and gyroscope data
      IMU.readAcceleration(aX, aY, aZ);
      IMU.readGyroscope(gX, gY, gZ);

      // normalize the IMU data between 0 to 1 and store in the model's
      // input tensor
      tflInputTensor->data.f[samplesRead * 6 + 0] = (aX + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 1] = (aY + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 2] = (aZ + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 3] = (gX + 2000.0) / 4000.0;
      tflInputTensor->data.f[samplesRead * 6 + 4] = (gY + 2000.0) / 4000.0;
      tflInputTensor->data.f[samplesRead * 6 + 5] = (gZ + 2000.0) / 4000.0;

      samplesRead++;

      if (samplesRead == numSamples) {
        startTime = millis();
        // Run inferencing
        TfLiteStatus invokeStatus = tflInterpreter->Invoke();
        if (invokeStatus != kTfLiteOk) {
          Serial.println(shut);         //Shutdown Pi when error detected
          setColour(255,0,0,5000);      // Error LED
          while (1);
        }
          
        // Loop through the output tensor values from the model
        for (int i = 0; i < NUM_GESTURES; i++) {
          Serial.print(GESTURES[i]);
          Serial.print(": ");
          Serial.println(tflOutputTensor->data.f[i], 6);
        }
        endTime = millis();
        infTime = endTime - startTime;
        Serial.print("Inference Duration = ");
        Serial.println(infTime);
        
        if(elapMS < inactivityTime){
          if(tflOutputTensor->data.f[0] > 0.99){
            Serial.println(reboot);       //Reboot Pi if Twisting is detected
            setColour(0,255,255, 2000);   //Cyan
          }
          else if(tflOutputTensor->data.f[1] > 0.99){
            Serial.println(wake);     //Wake on LAN if Lift Up / Down is detected
            setColour(255,128,0,3000);   //Orange
          }
        }
        else{
          Serial.println("Inactivity period exceeded. Please try again.");
        }
        prevMS = millis();
        
      }
    }
  }
}

void turnOffLEDs(){
  // Function to turn off all LED activity
  digitalWrite(LED_BUILTIN, LOW);
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);
}

void setColour (int redValue, int greenValue, int blueValue, int duration){
  // Function to set RGB values, from 0-255 each
  // Since it's 8-bit per colour, max colour range = 24-bit, up to ~16.7mil colours
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
  delay(duration);
  turnOffLEDs();
}
