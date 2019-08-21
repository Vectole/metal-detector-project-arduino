#include <Arduino.h>

const char speakerPin = PB9;
const char shiftRegDataPin = PA0; //Common anode LED, active LOW
const char shiftRegOutputEnablePin = PA1; //active LOW
const char shiftRegStorageRegisterClockPin = PA2;
const char shiftRegDataClockPin = PA3;
const char shiftRegClearPin = PA4; //active LOW
const char eightSegmentDisplayPin3 = PB0;
const char eightSegmentDisplayPin2 = PA7;
const char eightSegmentDisplayPin1 = PA6;
const char barGraphDriverPin = PA8;
const char voltmeterPin = PB8;
const char inductorDriverPin = PB1;
const char inductorOutputPin = PA5;

const char digitBits[] = {
    0b10111110, //0
    0b10000010, //1
    0b11101100, //2
    0b11100110, //3
    0b11010010, //4
    0b01110110, //5
    0b01111110, //6
    0b10100010, //7
    0b11111110, //8
    0b11110110  //9
};

void setup() {
    pinMode(speakerPin, OUTPUT);
    pinMode(shiftRegDataPin, OUTPUT);
    pinMode(shiftRegOutputEnablePin, OUTPUT);
    pinMode(shiftRegStorageRegisterClockPin, OUTPUT);
    pinMode(shiftRegDataClockPin, OUTPUT);
    pinMode(shiftRegClearPin, OUTPUT);
    pinMode(barGraphDriverPin, OUTPUT);
    pinMode(voltmeterPin, OUTPUT);
    pinMode(inductorDriverPin, OUTPUT);
    pinMode(inductorOutputPin, INPUT);
    pinMode(eightSegmentDisplayPin1, OUTPUT);
    pinMode(eightSegmentDisplayPin2, OUTPUT);
    pinMode(eightSegmentDisplayPin3, OUTPUT);

    digitalWrite(eightSegmentDisplayPin1, HIGH);
    digitalWrite(eightSegmentDisplayPin2, HIGH);
    digitalWrite(eightSegmentDisplayPin3, HIGH);

    digitalWrite(shiftRegOutputEnablePin, LOW);
    digitalWrite(shiftRegClearPin, LOW);
    digitalWrite(shiftRegClearPin, HIGH);
    digitalWrite(shiftRegDataClockPin, LOW);
}


void setBarGraph(unsigned short inputValue) {
    unsigned char barGraphValue = map(inputValue, 0, 4095, 90, 0);
    analogWrite(barGraphDriverPin, barGraphValue);
}

void pulseSpeaker(unsigned short inputValue) {
    unsigned char speakerValue = map(inputValue, 0, 4095, 50, 0);
    analogWrite(speakerPin, speakerValue);
}

unsigned short number = 0;
unsigned char display = 0;
unsigned char tick = 0;
void displayDigits(unsigned short inputValue) {
    unsigned int digit = 0;
    if(tick == 0) {
        number = map(inputValue, 0, 4095, 999, 0);  
        tick = 25;
    } else {
        tick--;
    }

    if(display == 0) {
        digitalWrite(eightSegmentDisplayPin1, HIGH);
        digitalWrite(eightSegmentDisplayPin2, LOW);
        digitalWrite(eightSegmentDisplayPin3, LOW);
        digit = number / 100; // most significant digit
        display++;
    }
    if(display == 1) {
        digitalWrite(eightSegmentDisplayPin1, LOW);
        digitalWrite(eightSegmentDisplayPin2, HIGH);
        digitalWrite(eightSegmentDisplayPin3, LOW);
        digit = (number / 10) % 10; // middle digit
        display++;
    }
    if(display == 2) {
        digitalWrite(eightSegmentDisplayPin1, LOW);
        digitalWrite(eightSegmentDisplayPin2, LOW);
        digitalWrite(eightSegmentDisplayPin3, HIGH);
        digit = number % 10; // least signifcant digit
        display = 0;
    }

    for(char i = 0; i < 8; i++) {
        digitalWrite(shiftRegDataPin, ~digitBits[digit] >> i & 0b00000001);
        digitalWrite(shiftRegDataClockPin, HIGH);
        digitalWrite(shiftRegDataClockPin, LOW);
    }
    digitalWrite(shiftRegStorageRegisterClockPin, HIGH);
    digitalWrite(shiftRegStorageRegisterClockPin, LOW);
}


void pulseVoltmeter(unsigned short inputValue) {
    char voltmeterValue = 0;
    voltmeterValue = map(inputValue, 0, 4095, 255, 0);
    analogWrite(voltmeterPin, voltmeterValue);
}

void pulseInductor(unsigned short pulseWidth) {
    delayMicroseconds(6666);
    digitalWrite(inductorDriverPin, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(inductorDriverPin, LOW);
}

void loop() {    
    pulseInductor(700); // 700 us pulse width
    delayMicroseconds(40); // measure after 40 us
    unsigned short inductorValue = analogRead(inductorOutputPin);
    if(inductorValue < 2036) {
        inductorValue = 4095;
    } else if(inductorValue > 2654) {
        inductorValue = 0;
    } else {
        inductorValue = map(inductorValue, 2036, 2654, 4095, 0); 
    }
    setBarGraph(inductorValue);
    pulseVoltmeter(inductorValue);
    pulseSpeaker(inductorValue); 
    displayDigits(inductorValue);
}