#include "I2Cdev.h"
#include "MPU6050.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;

int driveStatus = 0;
int exdelay = 0;
int threshold = 3;
int delayTime = 300;

int forwardLeft = 10;
int backwardLeft = 9;
int forwardRight = 12;
int backwardRight = 11;

#define OUTPUT_READABLE_ACCELGYRO

void setup() {
    pinMode(forwardLeft, OUTPUT);
    pinMode(forwardRight, OUTPUT);
    pinMode(backwardLeft, OUTPUT);
    pinMode(backwardRight, OUTPUT);    
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
    accelgyro.setFullScaleGyroRange(0);

}

void loop() {
        Serial.println(driveStatus);  
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);//Givening in degreee per/sec
    
  if (exdelay == 0){
    gx = gx/131/10;
    gy = gy/131/10;
    gz = gz/131/10;
    }
  else{
    gx = gx/131/(10 + delayTime/1000);
    gy = gy/131/(10 + delayTime/1000);
    gz = gz/131/(10 + delayTime/1000);
      }
    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);

  //Define driveStatus
  //0 no Motion
  //1 Forward
  //2 Backward
  //3 Left
  //4 Rigth
    if (gy > threshold & driveStatus == 0){
      driveStatus = 1;
      }
    if ( gy < -threshold & driveStatus == 1){
      driveStatus = 0;
      delay(delayTime);
      exdelay = 1;
      }
    if (gy < -threshold & driveStatus == 0){
      driveStatus = 2;
      exdelay = 0;
      }
    if ( gy > threshold & driveStatus == 2){
      driveStatus = 0;
      exdelay = 1;
      delay(delayTime);
      } 
    if (gx < -threshold & driveStatus == 0){
      driveStatus = 3;
      exdelay = 0;
      }
    if (gx > threshold & driveStatus == 3){
      driveStatus = 0;
      exdelay = 1;
      delay(delayTime);
      }        
    if (gx > threshold & driveStatus == 0){
      driveStatus = 4;
      exdelay = 0;
      }                          
    if (gx < -threshold & driveStatus == 4){
      driveStatus = 0;
      exdelay = 1;
      delay(delayTime);
      }
    if (driveStatus == 1){
        digitalWrite(forwardLeft, HIGH);
        digitalWrite(forwardRight, HIGH);
        digitalWrite(backwardLeft, LOW);
        digitalWrite(backwardRight, LOW);         
      }
      else if (driveStatus == 2){
        digitalWrite(forwardLeft, LOW);
        digitalWrite(forwardRight, LOW);
        digitalWrite(backwardLeft, HIGH);
        digitalWrite(backwardRight, HIGH);         
        }//Backward
      else if (driveStatus == 3){
        digitalWrite(forwardLeft, HIGH);
        digitalWrite(forwardRight, LOW);
        digitalWrite(backwardLeft, LOW);
        digitalWrite(backwardRight, LOW);         
        }//Left
      else if (driveStatus == 4){
        digitalWrite(forwardLeft, LOW);
        digitalWrite(forwardRight, HIGH);
        digitalWrite(backwardLeft, LOW);
        digitalWrite(backwardRight, LOW);         
        }//Right        
        else{
        digitalWrite(forwardLeft, LOW);
        digitalWrite(forwardRight, LOW);
        digitalWrite(backwardLeft, LOW);
        digitalWrite(backwardRight, LOW);    
          }                            
    #ifdef OUTPUT_READABLE_ACCELGYRO
        // display tab-separated accel/gyro x/y/z values
        Serial.print("a/g:\t");
        Serial.print(gx); Serial.print("\t");
        Serial.print(gy); Serial.print("\t");
        Serial.println(gz);
    #endif

    #ifdef OUTPUT_BINARY_ACCELGYRO
        Serial.write((uint8_t)(ax >> 8)); Serial.write((uint8_t)(ax & 0xFF));
        Serial.write((uint8_t)(ay >> 8)); Serial.write((uint8_t)(ay & 0xFF));
        Serial.write((uint8_t)(az >> 8)); Serial.write((uint8_t)(az & 0xFF));
        Serial.write((uint8_t)(gx >> 8)); Serial.write((uint8_t)(gx & 0xFF));
        Serial.write((uint8_t)(gy >> 8)); Serial.write((uint8_t)(gy & 0xFF));
        Serial.write((uint8_t)(gz >> 8)); Serial.write((uint8_t)(gz & 0xFF));
    #endif

    delay(100);
   
}
