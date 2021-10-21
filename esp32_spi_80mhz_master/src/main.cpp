#include <Arduino.h>
#include <SPI.h>

#define SPI_CLOCK_4MHZ 4000000
#define SPI_CLOCK_10MHZ 10000000
#define SPI_CLOCK_20MHZ 20000000

#define SPI_CS_PIN 5

uint8_t message[32] = { 
    0, 1, 2, 3, 4, 5, 6, 7, 
    8, 9, 10, 11, 12, 13, 14, 15, 
    16, 17, 18, 19, 20, 21, 22, 23, 
    24, 25, 26, 27, 28, 29, 30, 31 };

uint8_t buf[32] = {0};

void setup()
{
    Serial.begin(9600);
    Serial.println("Hello World from master!");

    SPI.begin(18, 19, 23, 5);
    SPI.setFrequency(SPI_CLOCK_20MHZ);

    pinMode(SPI_CS_PIN, OUTPUT);
}

void loop()
{
    digitalWrite(SPI_CS_PIN, LOW);

    SPI.beginTransaction(SPISettings(SPI_CLOCK_4MHZ, MSBFIRST, SPI_MODE0));
    SPI.transferBytes((const uint8_t *)message, (uint8_t *)buf, sizeof(message));

    for (int i = 0; i < sizeof(message); i++)
        if(buf[i] != 0) Serial.print(String(buf[i]) + " ");

    Serial.println();

    SPI.endTransaction();

    digitalWrite(SPI_CS_PIN, HIGH);
    delay(1000);
}