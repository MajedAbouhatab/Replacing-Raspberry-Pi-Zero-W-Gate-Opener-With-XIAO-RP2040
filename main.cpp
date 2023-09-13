#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <Adafruit_NeoPixel.h>

// Remote code
String Dip = "0101011010";

Adafruit_NeoPixel pixels(1, 12, NEO_GRB + NEO_KHZ800);

void setup(void) {}

void loop(void)
{
  // Each switch (digit) is half byte (4 bits)
  int NumberOfBytes = Dip.length() / 2;
  byte BytesToSend[NumberOfBytes];
  ELECHOUSE_cc1101.Init();
  // Also change to SPI pins for Xiao in
  // %USERPROFILE%\.platformio\packages\framework-arduino-mbed\variants\RASPBERRY_PI_PICO\pins_arduino.h
  ELECHOUSE_cc1101.setSpiPin(2, 4, 3, 1);
  ELECHOUSE_cc1101.setSyncMode(0);
  ELECHOUSE_cc1101.setDRate(2.0);
  ELECHOUSE_cc1101.setPktFormat(0);
  ELECHOUSE_cc1101.setLengthConfig(0);
  ELECHOUSE_cc1101.setModulation(2);
  ELECHOUSE_cc1101.setPacketLength(NumberOfBytes);
  // Replace every digit pair with 1000 if zero or 1110 if one
  for (int i = 0; i < NumberOfBytes; i++)
    BytesToSend[i] = ((Dip[2 * i] == '0' ? 0b1000 : 0b1110) << 4) | (Dip[2 * i + 1] == '0' ? 0b1000 : 0b1110);
  ELECHOUSE_cc1101.SpiWriteBurstReg(CC1101_TXFIFO, BytesToSend, NumberOfBytes);
  ELECHOUSE_cc1101.SetTx(300);
  uint8_t chipstate = 0xFF;
  while (chipstate != 0x01)
    chipstate = (ELECHOUSE_cc1101.SpiReadStatus(CC1101_MARCSTATE) & 0x1F);
  ELECHOUSE_cc1101.SpiStrobe(CC1101_SFTX);
  // Blink and delay
  digitalWrite(11, HIGH);
  pixels.setPixelColor(0, pixels.Color(millis() % 200 > 100 ? 255 : 0, 0, 0));
  pixels.show();
}
