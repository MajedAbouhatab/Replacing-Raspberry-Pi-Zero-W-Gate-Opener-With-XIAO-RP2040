#include <SPI.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <Adafruit_NeoPixel.h>

// Remote code
String Dip = "0101011010";
// Each switch (digit) is half a byte (4 bits)
byte BytesToSend[5]; // Dip.length() / 2;

#define POWER_PIN 11
#define NEO_PIN 12
Adafruit_NeoPixel pixels(1, NEO_PIN, NEO_GRB + NEO_KHZ800);

void setup(void)
{
  pinMode(POWER_PIN, OUTPUT);
  pinMode(NEO_PIN, OUTPUT);
  pixels.begin();
  pixels.setBrightness(255);
  pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  SPI = arduino::MbedSPI(4, 3, 2);
  SPI.begin();
  ELECHOUSE_cc1101.Init();
  // No longer needed! Also change to SPI pins for Xiao in   // %USERPROFILE%\.platformio\packages\framework-arduino-mbed\variants\RASPBERRY_PI_PICO\pins_arduino.h
  ELECHOUSE_cc1101.setSpiPin(2, 4, 3, 1);
  ELECHOUSE_cc1101.setPA(10);
  ELECHOUSE_cc1101.setSyncMode(0);
  ELECHOUSE_cc1101.setDRate(2.0);
  ELECHOUSE_cc1101.setPktFormat(0);
  ELECHOUSE_cc1101.setLengthConfig(0);
  ELECHOUSE_cc1101.setModulation(2);
  ELECHOUSE_cc1101.setPacketLength(sizeof(BytesToSend));
  // Replace every digit pair , 1000 if zero or 1110 if one
  for (int i = 0; i < sizeof(BytesToSend); i++)
    BytesToSend[i] = ((Dip[2 * i] == '0' ? 0b1000 : 0b1110) << 4) | (Dip[2 * i + 1] == '0' ? 0b1000 : 0b1110);
}

void loop(void)
{
  if (millis() / 1000 % 2)
    digitalWrite(POWER_PIN, LOW);
  else
  {
    // Set buffer
    ELECHOUSE_cc1101.SpiWriteBurstReg(CC1101_TXFIFO, BytesToSend, sizeof(BytesToSend));
    ELECHOUSE_cc1101.SetTx(300);
    uint8_t chipstate = 0xFF;
    while (chipstate != 0x01)
      chipstate = (ELECHOUSE_cc1101.SpiReadStatus(CC1101_MARCSTATE) & 0x1F);
    // Clear buffer
    ELECHOUSE_cc1101.SpiStrobe(CC1101_SFTX);
    digitalWrite(POWER_PIN, HIGH);
    delay(10);
    pixels.show();
  }
}
