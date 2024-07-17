//#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h> //LCD and TouchScreen are using SPI
#include <XPT2046_Touchscreen.h> //Touchscreen Lib
#include <Wire.h> // For 12C
#include <Adafruit_Sensor.h> 
#include <Adafruit_BMP280.h>
#define I2C_SDA 27  //GPIO Pin 27 on CN1 to be used as SDA
#define I2C_SCL 22  //SCL Pin on CN1

#define SEALEVELPRESSURE_HPA (1013.25) //Used to Convert pressure to Altitude (BME Sensor)
TwoWire I2CBMP = TwoWire(0);  //Creates a new TwoWire instance. In this case, it’s called I2CBMP. This simply creates an I2C bus
Adafruit_BMP280 bmp(&I2CBMP); //the &I2CBMP is added here in case of a BMP Sensor, for BME leave blank







// Touchscreen pins
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);


TFT_eSPI tft = TFT_eSPI(); //Define a tft Instance

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define FONT_SIZE 5

bool arc_end = 1;
// Define the colors for the arcs
uint16_t tempColor = TFT_RED;
uint16_t pressureColor = TFT_BLUE;
uint16_t altitudeColor = TFT_GREEN;
uint16_t humidityColor = TFT_CYAN;
uint16_t bg = TFT_BLACK;
uint16_t fg = TFT_WHITE;
// Define the start and end angles for the arcs
int startAngle = 0;
int endAngle = 180;

// Define the radius for the arcs
int radius = 50;  // Adjust as needed
int iradius = radius - 10;

// Define the positions for the arcs
/* int tempX = radius + 10, tempY = radius + 10;
int pressureX = 320 - radius - 10, pressureY = radius + 10;
int altitudeX = radius + 10, altitudeY = 240 - radius - 10;
int humidityX = 320 - radius - 10, humidityY = 240 - radius - 10; */

// Define the positions for the arcs
int tempX = 160 - radius, tempY = 120 - radius;
int pressureX = 160 + radius, pressureY = 120 - radius;
int altitudeX = 160 - radius, altitudeY = 120 + radius;
int humidityX = 160 + radius, humidityY = 120 + radius;



// Define placeholders for the previous end angles
int prevTempEndAngle = startAngle;
int prevPressureEndAngle = startAngle;
int prevAltitudeEndAngle = startAngle;
int prevHumidityEndAngle = startAngle;


void setup() {
  Serial.begin(115200);
    // Small sprite for spot demo

  // Start BMP Sensor
  I2CBMP.begin(I2C_SDA, I2C_SCL, 100000); //initialize the I2C communication with the pins defined earlier.third parameter is clock freq.
  bool status;
  status = bmp.begin(0x76);  //In case of a BME Sensor, use bme.begin(0x76, &I2CBME);
  if (!status) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }


//Touch Screen
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);  // Start the SPI for the touchscreen and init the touchscreen
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(1);// Set the Touchscreen rotation in landscape mode


  //TFT Display
  int centerX = SCREEN_WIDTH / 2;// Set X and Y coordinates for center of display
  int centerY = SCREEN_HEIGHT / 2;
  tft.init();  // Start the tft display
  tft.setRotation(1);// Set the TFT display rotation in landscape mode
  tft.invertDisplay(1);// this changes the color assignment for CYD2USB Variant
  tft.fillScreen(bg); // Clear the screen before writing to it
  tft.setTextColor(fg, bg); // Sets the Text Color and Text-Background Color
  tft.setTextDatum(MC_DATUM);  // Set the text datum to middle center

  
//Starting Message
  //tft.drawCentreString("Hello, world!", centerX, 30, FONT_SIZE);
  //tft.drawCentreString("Touch screen to See Temperature", centerX, centerY, FONT_SIZE);
  //delay(1000);

 // Draw the filled circles as background for the arc dials
 /* 
  tft.fillCircle(tempX, tempY, radius, TFT_LIGHTGREY);
  tft.fillCircle(pressureX, pressureY, radius, TFT_LIGHTGREY);
  tft.fillCircle(altitudeX, altitudeY, radius, TFT_LIGHTGREY);
  tft.fillCircle(humidityX, humidityY, radius, TFT_LIGHTGREY); */

// Draw the circles as outlines for the arc dials
  tft.drawCircle(tempX, tempY, radius, tempColor);
  tft.drawCircle(pressureX, pressureY, radius, pressureColor);
  tft.drawCircle(altitudeX, altitudeY, radius, altitudeColor);
  tft.drawCircle(humidityX, humidityY, radius, humidityColor);


  // Draw the labels below the numerical values
tft.drawString("Temperature", tempX, tempY + 10);  // Adjust the y-coordinate as needed
  tft.drawString("Pressure", pressureX, pressureY + 10);  // Adjust the y-coordinate as needed
  tft.drawString("Altitude", altitudeX, altitudeY + 10);  // Adjust the y-coordinate as needed
  tft.drawString("Humidity", humidityX, humidityY + 10);  // Adjust the y-coordinate as needed




}




void loop() {


  int centerX = SCREEN_WIDTH / 2;// Set X and Y coordinates for center of display
  int centerY = SCREEN_HEIGHT / 2;

//tft.drawFloat(bmp.readTemperature(), 2, centerX, centerY, FONT_SIZE);
Serial.print(bmp.readTemperature());
  Serial.println(" *C");
    delay(100);

  // Read the sensor values
  int tempValue = bmp.readTemperature();  // Replace with actual function to read temperature
  int pressureValue = bmp.readPressure();  // Replace with actual function to read pressure
  int altitudeValue = bmp.readAltitude(1013.25);  // Replace with actual function to read altitude
  int humidityValue = 69;  // Replace with actual function to read humidity

  // Map the sensor values to the range of the arc angles
  int tempEndAngle = map(tempValue, -5, 50, startAngle, endAngle);  // Adjust the range as needed
  int pressureEndAngle = map(pressureValue, 94000, 96000, startAngle, endAngle);  // Adjust the range as needed
  int altitudeEndAngle = map(altitudeValue, 0, 1000, startAngle, endAngle);  // Adjust the range as needed
  int humidityEndAngle = map(humidityValue, 0, 100, startAngle, endAngle);  // Adjust the range as needed

  //Wipe  
//tft.fillScreen(TFT_WHITE);

 // Erase the previous arcs by redrawing them in the background color
  tft.drawSmoothArc(tempX, tempY, radius, iradius, startAngle, prevTempEndAngle, bg, bg, arc_end);
  tft.drawSmoothArc(pressureX, pressureY, radius, iradius, startAngle, prevPressureEndAngle, bg, bg, arc_end);
  tft.drawSmoothArc(altitudeX, altitudeY, radius, iradius, startAngle, prevAltitudeEndAngle, bg, bg, arc_end);
  tft.drawSmoothArc(humidityX, humidityY, radius, iradius, startAngle, prevHumidityEndAngle, bg, bg, arc_end);

  // Draw the arcs
  
  tft.drawSmoothArc(tempX, tempY, radius, iradius, startAngle, tempEndAngle, tempColor, bg, arc_end);
  tft.drawSmoothArc(pressureX, pressureY, radius, iradius, startAngle, pressureEndAngle, pressureColor, bg, arc_end);
  tft.drawSmoothArc(altitudeX, altitudeY, radius, iradius, startAngle, altitudeEndAngle, altitudeColor, bg, arc_end);
  tft.drawSmoothArc(humidityX, humidityY, radius, iradius, startAngle, endAngle, humidityColor, bg, arc_end);

  // Update the previous end angles
  prevTempEndAngle = tempEndAngle;
  prevPressureEndAngle = pressureEndAngle;
  prevAltitudeEndAngle = altitudeEndAngle;
  prevHumidityEndAngle = humidityEndAngle;


  delay(5600);



  // Draw the numerical values
  tft.setCursor(tempX, tempY);
  tft.println(String(tempValue));
  tft.setCursor(pressureX, pressureY);
  tft.println(String(pressureValue));
  tft.setCursor(altitudeX, altitudeY);
  tft.println(String(altitudeValue));
  tft.setCursor(humidityX, humidityY);
  tft.println(String(humidityValue));

  //delay(1000);  // Wait for a second before updating the values again


//uint16_t end_angle   = map(bmp.readTemperature(),25,35,45,225);




}
