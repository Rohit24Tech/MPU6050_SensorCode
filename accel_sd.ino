// Libraries for SD card
#include "FS.h"
#include "SD.h"
#include <SPI.h>
//Libraries for BME280 sensor
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
// MPU-6050
Adafruit_MPU6050 mpu;
// Variables to hold sensor readings
unsigned long myTime = 0;
unsigned long s_no = 1;
//int timeoffset = 0;
String dataMessage;
// Init MPU-6050
void initMPU(){
  if (!mpu.begin()) {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    while (1);
  }
}
// Initialize SD card
void initSDCard(){
   if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }
  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}
// Write to the SD card
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}
// Append data to the SD card
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
void setup() {
  Serial.begin(115200);  
  initMPU();
  initSDCard();
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/data.txt");
  if(!file) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/data.txt", "sno,time,Acceleration X-Axis,Acceleration Y-Axis,Acceleration Z-Axis,Rotation X-Axis,Rotation Y-Axis,Rotation Z-Axis\r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();
  delay(5000);
  Serial.print("done");
  delay(10000);
  //timeoffset = millis();
}

void loop() {
  //Get sensor readings
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  myTime = millis();   
  //myTime = myTime - timeoffset; 
  //Concatenate all info separated by commas
  dataMessage = String(s_no) + "," + String(myTime) + "," + String(a.acceleration.x) + "," + String(a.acceleration.y) + "," + String(a.acceleration.z) + "," + String(g.gyro.x) + "," + String(g.gyro.y) + "," + String(g.gyro.z)+ "\r\n";
  //Serial.println(dataMessage);
  //Append the data to file
  appendFile(SD, "/data.txt", dataMessage.c_str());
  s_no++;
}
