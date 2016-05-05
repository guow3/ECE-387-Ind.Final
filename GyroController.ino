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
int threshold = 1;
int delayTime = 300;

int forwardLeft = 10;
int backwardLeft = 9;
int forwardRight = 12;
int backwardRight = 11;
int flaga = 0;
long beg = 0;
long en = 0;
long count = 0;

#define OUTPUT_READABLE_ACCELGYRO

void setup() {
    pinMode(forwardLeft, OUTPUT);
    pinMode(forwardRight, OUTPUT);
    pinMode(backwardLeft, OUTPUT);
    pinMode(backwardRight, OUTPUT);
    pinMode(47, OUTPUT);        
    pinMode(49, OUTPUT);
    pinMode(51, OUTPUT);
    pinMode(53, OUTPUT);      
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
    accelgyro.setFullScaleAccelRange(0);
}

void loop() {
    if (count == 0){
      beg = millis();
      count = 1;
    }
    else{
      en = millis();
      flaga = 1;
      }
    Serial.println(driveStatus);
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);//Givening in degreee per/sec

    ax = ax/16384;
    ay = ay/16384;
    az = az/16384;
    

    gx = gx/131/100;
    gy = gy/131/100;
    gz = gz/131/100;
    
    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);

  //Define driveStatus
  //0 no Motion
  //1 Forward
  //2 Backward
  //3 Left forward
  //4 Rigth forward
  //5 Left Back
  //6 Right Back

 //Car in Neutral status==========================================================
    if (gy > threshold & gx < threshold & gx > -threshold & driveStatus == 0){
      driveStatus = 1;
      }
    if (gy < -threshold & gx < threshold & gx > -threshold & driveStatus == 0){
      driveStatus = 2;
      }
    if (gy < -threshold & gx < -threshold & driveStatus == 0){
      driveStatus = 5;
      }  
    if (gy < -threshold & gx & threshold& driveStatus == 0){
      driveStatus = 6;
      }          
    if (gy > threshold & gx < -threshold& driveStatus == 0){
      driveStatus = 3;
      }         
    if (gy > threshold & gx > threshold & driveStatus == 0){
      driveStatus = 4;
      }         

//End Car Neutral Status===========================================================   
//Car Forward Status==============================================================   
     if ( gy < -threshold & gx < threshold & gx > -threshold & driveStatus == 1){
      driveStatus = 0;
      }
     if ( gx < -threshold & gy < threshold & gy > -threshold & driveStatus == 1){
      driveStatus = 3;
      }
     if ( gx > threshold & gy < threshold & gy > -threshold & driveStatus == 1){
      driveStatus = 4;
      }    
              
//End Car Forward Status===========================================================   
//Car Backward Status==============================================================        
     if ( gy > threshold & gx < threshold & gx > -threshold & driveStatus == 2){
      driveStatus = 0;
      }
     if ( gx > threshold & driveStatus == 2){
      driveStatus = 6;
      } 
     if ( gx < -threshold & driveStatus == 2){
      driveStatus = 5;
      }                        
//End Car Backward Status===========================================================   
//Car Left Forward Status==============================================================            
     if (gx > threshold & gy < threshold & gy > -threshold  & driveStatus == 3){
      driveStatus = 0;
      }        
     if (gy < -threshold & gy < threshold & gy > -threshold  & driveStatus == 3){
      driveStatus = 5;
      }   
     if (gy < -threshold & gx < threshold & gx > -threshold & driveStatus == 3){
      driveStatus = 0;
      } 

//End Car Left Forward Status===========================================================   
//Car Right Forward Status==============================================================      
    if (gx < -threshold & gy < threshold & gy > -threshold  & driveStatus == 4){
      driveStatus = 1;
      }
     if (gy < -threshold & gx < threshold & gx > -threshold  & driveStatus == 4){
      driveStatus = 6;
      }
     if (gy < -threshold & gx < -threshold & driveStatus == 4){
      driveStatus = 0;
      }
//End Right Forward Status===========================================================   
//Car Left Backward Status==============================================================      
    if (gy > threshold & gx < threshold & gx > -threshold  & driveStatus == 5){
      driveStatus = 3;
      }
     if (gx > threshold & gy < threshold & gy > -threshold  & driveStatus == 5){
      driveStatus = 2;
      }
     if (gy >  threshold & gx > threshold & driveStatus == 5){
      driveStatus = 0;
      }
//End Car Left Backawrd Status===========================================================   
//Car Right Backward Status==============================================================      
    if (gy > threshold & gx < threshold & gx > -threshold  & driveStatus == 6){
      driveStatus = 4;
      }
     if (gx < -threshold & gy < threshold & gy > -threshold  & driveStatus == 6){
      driveStatus = 2;
      }
     if (gy > threshold & gx < -threshold & driveStatus == 6){
      driveStatus = 0;
      }
//End Car Right Backward Status===========================================================   
     
 
    if (driveStatus == 1){
        digitalWrite(forwardLeft, HIGH);
        digitalWrite(forwardRight, HIGH);
        digitalWrite(backwardLeft, LOW);
        digitalWrite(backwardRight, LOW); 
        digitalWrite(47, LOW);   
        digitalWrite(49, LOW);
        digitalWrite(51, LOW);
        digitalWrite(53, HIGH);                
      }//Forward
      else if (driveStatus == 2){
        digitalWrite(forwardLeft, LOW);
        digitalWrite(forwardRight, LOW);
        digitalWrite(backwardLeft, HIGH);
        digitalWrite(backwardRight, HIGH);
        digitalWrite(47, LOW);   
        digitalWrite(49, HIGH);
        digitalWrite(51, LOW);
        digitalWrite(53, LOW);        
        }//Backward
      else if (driveStatus == 3){
        digitalWrite(forwardLeft, HIGH);
        digitalWrite(forwardRight, LOW);
        digitalWrite(backwardLeft, LOW);
        digitalWrite(backwardRight, LOW);
        digitalWrite(47, HIGH);     
        digitalWrite(49, LOW);
        digitalWrite(51, LOW);
        digitalWrite(53, HIGH);         
        }//Left For
      else if (driveStatus == 4){
        digitalWrite(forwardLeft, LOW);
        digitalWrite(forwardRight, HIGH);
        digitalWrite(backwardLeft, LOW);
        digitalWrite(backwardRight, LOW);
        digitalWrite(47, LOW);     
        digitalWrite(49, LOW);
        digitalWrite(51, HIGH);
        digitalWrite(53, HIGH);                
        }//Right For      
      else if (driveStatus == 5){
        digitalWrite(forwardLeft, LOW);
        digitalWrite(forwardRight, LOW);
        digitalWrite(backwardLeft, LOW);
        digitalWrite(backwardRight, HIGH); 
        digitalWrite(47, HIGH);   
        digitalWrite(49, HIGH);
        digitalWrite(51, LOW);
        digitalWrite(53, LOW);                   
        }//Left Back
      else if (driveStatus == 6){
        digitalWrite(forwardLeft, LOW);
        digitalWrite(forwardRight, LOW);
        digitalWrite(backwardLeft, HIGH);
        digitalWrite(backwardRight, LOW); 
        digitalWrite(47, LOW);  
        digitalWrite(49, HIGH);
        digitalWrite(51, HIGH);
        digitalWrite(53, LOW);                   
        }//Right Back
      else{
        digitalWrite(forwardLeft, LOW);
        digitalWrite(forwardRight, LOW);
        digitalWrite(backwardLeft, LOW);
        digitalWrite(backwardRight, LOW);
        digitalWrite(47, LOW);          
        digitalWrite(49, LOW);
        digitalWrite(51, LOW);
        digitalWrite(53, LOW);            
          }                            
    #ifdef OUTPUT_READABLE_ACCELGYRO
        // display tab-separated accel/gyro x/y/z values
        Serial.print(ax); Serial.print("\t");
        Serial.print(ay); Serial.print("\t");
        Serial.print(az); Serial.print("\t");
        Serial.print(gx); Serial.print("\t");
        Serial.print(gy); Serial.print("\t");
        Serial.print(gz);Serial.print("\t");
    #endif

    #ifdef OUTPUT_BINARY_ACCELGYRO
        Serial.write((uint8_t)(ax >> 8)); Serial.write((uint8_t)(ax & 0xFF));
        Serial.write((uint8_t)(ay >> 8)); Serial.write((uint8_t)(ay & 0xFF));
        Serial.write((uint8_t)(az >> 8)); Serial.write((uint8_t)(az & 0xFF));
        Serial.write((uint8_t)(gx >> 8)); Serial.write((uint8_t)(gx & 0xFF));
        Serial.write((uint8_t)(gy >> 8)); Serial.write((uint8_t)(gy & 0xFF));
        Serial.write((uint8_t)(gz >> 8)); Serial.write((uint8_t)(gz & 0xFF));
    #endif
  delay(10);
   
}
