// White board to draw on its self
// width problem still exists

#include <Servo.h>
#include <SPI.h>
#include <SD.h>

// pins
const int stepPinL = 4; // Stepper Pins
const int dirPinL = 3;
const int stepPinR = A3;
const int dirPinR = A2;
const int motorPowerPin = 9; // toggles relay
const int servoPin = A4;

const int Width = 430; // white board dimensions
const int height = 690;

const float feedRate = 42.4265; // rate that end moves compared to rotations. mm/revolution
const int PPR = 1600; //Pulse Per Revolution of motor
const int speed = 1500; // timer value. The higher this value, the slower the end moves


File Image;
byte dataLocation = 0; // location of where the pixel info starts.
float bitSkip; // bmp files have a property that every row has to be stored as a multiple of 4 bytes. there are 3 bytes per pixel, sometimes resulting in dummy bits that need to be skipped. This value is used to skip them.
float pixel = 0; // pixel info is read into this value before deciding if it should be drawn
int imageWidth = 0;
int imageHeight = 0;


float dotSeparation = 1; // distance from center of dot to center of dot (mm)
float zDist = 55; // Number of degrees the servo will move to push the marker to the board.

float lengthL = 260; // current distances of L/R motors Numbers <<here should be where the end is when the boad is turned on
float lengthR = 260;

Servo Marker;

void setup() {
    // set pins
    pinMode(stepPinL, OUTPUT);
    pinMode(dirPinL, OUTPUT);
    pinMode(stepPinR, OUTPUT);
    pinMode(dirPinR, OUTPUT);
    pinMode(motorPowerPin, OUTPUT);


    digitalWrite(stepPinL, LOW);
    digitalWrite(dirPinL, LOW);
    digitalWrite(stepPinR, LOW);
    digitalWrite(dirPinR, LOW);
    digitalWrite(motorPowerPin, LOW);

    Marker.attach(servoPin);
    Marker.write(180);

    Serial.begin(9600);

    delay(1000);
    digitalWrite(motorPowerPin, HIGH);
    delay(1000);

    while (!SD.begin(10)) { // 10 is cs pin
    }

    Image = SD.open("Art.bmp", FILE_READ);

    if (Image) {

        Image.seek(1);
        if (Image.read() != 77) { // if MD file

            while (1) {
                Serial.println("Wrong Filetype");
            }

        }

        Image.seek(18);
        imageWidth = Image.read();

        Image.seek(22);
        imageHeight = Image.read();

        Image.seek(10);
        dataLocation = Image.read();
        Image.seek(dataLocation); // go to data

        for (int y = 0; y < imageHeight; y++) {
            for (int x = 0; x < imageWidth; x++) {
                
                pixel = 0;
                for (int i = 0; i < 3; i++) { // get rgb data and do math to figure out how bright the pixel is
                    pixel += pow(Image.read(), 2);
                }
                pixel = sqrt(pixel);

                if (pixel < 127) { // if pixel meets requirments to be drawn
                    Move((x * dotSeparation) + (Width / 2) - ((imageWidth * dotSeparation) / 2), (y * dotSeparation) + 100); // move to pixel location
                    Dot(); // draw  the dot
                }
            }

            bitSkip = (imageWidth * 3) % 4; // skip number of bits because bmps are a lil strange
            bitSkip = 1.9998 - (24.7397 * sin(3.06069 - 3.10115 * bitSkip));

            for (float i = 0; i < bitSkip; i++) { // test
                if(Image.read() != 0) {
                    Serial.println("This should be zero. There is a problem.");
                } else {
                    Serial.println("Skipped bit cause bmps are strange");
                }
            }
        }
    }
}

void loop() {


}

void Move (float x, float y) { // input coordinate

    if (x < 30 || x > Width - 30 || y < 100 || y > height - 200) { // Limits so this thing doesn't tear it's self apart.
        Serial.println("Point out of Bounds.");
        return;
    }
    

    float newLengthL = sqrt(pow(x, 2) + pow(y, 2)); // lenths of chain that we need to get to the final destination
    float newLengthR = sqrt(pow((Width - x), 2) + pow(y, 2));

    float deltaL = abs(lengthL - newLengthL); // length needed to travel
    float deltaR = abs(lengthR - newLengthR);

    int pulseL = (deltaL / feedRate) * PPR; // # of pulses to send to steppers
    int pulseR = (deltaR / feedRate) * PPR;


    if (newLengthL > lengthL) {     // Set Direction
        digitalWrite(dirPinL, LOW);
    } else {
        digitalWrite(dirPinL, HIGH);
    }
    if (newLengthR > lengthR) {
        digitalWrite(dirPinR, HIGH);
    } else {
        digitalWrite(dirPinR, LOW);
    }
    
    while ((pulseL + pulseR) > 0) { // while end needs to be moved

        if (pulseL > 0) {
            digitalWrite(stepPinL, HIGH);
            pulseL--;
        }
        if (pulseR > 0) {
            digitalWrite(stepPinR, HIGH);
            pulseR--;
        }

        delayMicroseconds(speed);

        digitalWrite(stepPinL, LOW);
        digitalWrite(stepPinR, LOW);
        
        delayMicroseconds(speed);
    }

    lengthL = newLengthL; // set global variables
    lengthR = newLengthR;

}

void Dot() { // make a dot

    int resolution = 20;

    for (int i = 0; i < (resolution / 2); i++) {
        Marker.write(180 - (zDist / (resolution / 2)) * i);

        delay(35);
    }

    for (int j = resolution; j > 0; j--) {
        Marker.write(180 - (zDist / resolution) * j);

        delay(35);
    }

    delay(50);
    Marker.write(180);
}
