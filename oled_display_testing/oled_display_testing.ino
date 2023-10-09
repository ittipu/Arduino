#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH 16

#if (SH1106_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SH1106.h!");
#endif

void setup() {
  Serial.begin(9600);
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
                                             // init done
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();
}


void loop() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Getting Location Data");
  float lat = 23.781089;
  float lng = 90.402250;
  display.setCursor(0, 15);
  display.println("Lat: ");
  display.setCursor(25, 15);
  display.println(lat,6);
  display.setCursor(0, 30);
  display.println("Lng: ");
  display.setCursor(25, 30);
  display.println(lng,6);
  display.display();
  display.clearDisplay();
  Serial.print("Lat: ");
  Serial.print(lat,6);
  Serial.print(F(","));
  Serial.print("Lng: ");
  Serial.print(lng,6);
  Serial.println();
  delay(2000);
  display.setCursor(0, 0);
  display.println("Invalid Location");
  display.display();
  display.clearDisplay();
  delay(1000);
}
