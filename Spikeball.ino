//Light-Up Spikeball code for the project shown here https://youtu.be/7nR_p6-sPqM

// Include Adafruit NeoPixel library in sketch
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <OneButton.h>
#include <math.h>

// LED control pin on arduino
#define LED_PIN   6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 169   //depends on diameter/perimeter of rim

// Declare NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)


// Setup a new button on pin A1
OneButton button(A1, false);
int longPress = 1;

int animation = 1;
int function = 1;
int redStart = 100;
int greenStart = 0;
int blueStart = 0;
int redAmount = 20;
int greenAmount = 0;
int blueAmount = 0;
int animationColor = 0;


int scoreKeeper;
int waitForServe;
long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;
uint32_t team1Color = strip.Color(255, 0, 0); // Red
uint32_t team2Color = strip.Color(0, 0, 255); // Blue
uint32_t serveColor;
uint32_t playerColor1 = strip.Color(255, 0, 0); //Red
uint32_t playerColor2 = strip.Color(0, 0, 255); //Blue
uint32_t playerColor3 = strip.Color(0, 255, 0); //Green
uint32_t playerColor4 = strip.Color(255, 255, 0); //Yellow
int servingPlayer;
int previousServingPlayer;
int serveTeam;
int previousScore;
int previousServe;
int previousTeam;
int scoreTeam1;
int scoreTeam2;
int whoWon = 0;
unsigned long waitForBounce;
unsigned long bounceTime;
int teamWithBall;
int pixelStart;
int pixelStart1;
int pixelStart2;
int pixelStart3;
int pixelStart4;
int brightness = 90;
long waitForButton;
int eraseLastPoint;





void setup() {
  button.attachClick(click1);
  button.attachDoubleClick(doubleclick1);
  button.attachLongPressStart(longPressStart1);

  setupMPU();// Initialize the MPU-6050

  strip.begin();           // Initialize NeoPixel strip object
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(brightness); // Set brightness

  if (analogRead(A1) > 500) {
    scoreKeeper = 1;
  }
  else {
    scoreKeeper = 0;
  }

}





void loop() {
  startUp();// Play startup animation

  if (scoreKeeper == 1) {
    int nextPixel;
    int tracker;
    int looper;
    newGame();
    while (scoreKeeper = 1) {
      if (waitForServe == 1) {
        serve();
        waitForServe = 0;
        strip.fill(strip.Color(100, 0, 0), 0 , 42);
        strip.fill(strip.Color(0, 0, 100), 42, 42);
        strip.fill(strip.Color(0, 100, 0), 84, 42);
        strip.fill(strip.Color(100, 100, 0), 126, 42);
        strip.show();
      }
      nextPixel = -1;
      tracker = 0;
      looper = 100;
      bounce();
      bounceTime = millis();
      while (bounceTime - waitForBounce < 7000) {
        if (tracker < 24 && looper == 100) {
          nextPixel++;
        }
        else if (tracker > 23 && tracker < 47 && looper == 100) {
          nextPixel--;
        }
        else if (tracker > 46 && looper == 99) {
          tracker = 0;
          nextPixel = -1;
        }
        if (looper == 100) {
          looper = -1;
          tracker++;
          int j = nextPixel + 43;
          int k = j + 42;
          int l = k + 42;
          strip.fill(strip.Color(100, 0, 0), nextPixel + 1 , 42);
          strip.fill(strip.Color(0, 0, 100), j, 42);
          strip.fill(strip.Color(0, 100, 0), k, 42);
          strip.fill(strip.Color(100, 100, 0), l, 42);
          strip.fill(strip.Color(100, 100, 0), 0 , nextPixel + 1);
          strip.show();
        }
        looper ++;
        recordAccelRegisters(); // Record the accelerometer values
        if (abs(gForceX) > .5 || abs(gForceY) > .5) {
          bounce();
        }
        bounceTime = millis();
      }
      pointScored();
      displayScore();
    }
  }

  else {
    if (animation == 1) {
      for (int i = 0; i < 169; i++) {
        strip.fill(strip.Color(redStart, greenStart, blueStart), i, 1);
        strip.show();
      }
      while (animation == 1) {
        button.tick();
        recordAccelRegisters(); // Record the accelerometer values
        if (abs(gForceX) > .5 || abs(gForceY) > .5) {
          bounce2();
        }
      }
      strip.clear();
    }



    if (animation == 2) {
      int loops = 0;
      int bounce = 0;
      int b = 0;
      int count = 0;
      int brightnessChange;
      long firstPixelHue = 0;
      for (int i = 0; i < 168; i++) { // For each pixel in strip...
        // Offset pixel hue by an amount to make one full revolution of the
        // color wheel (range of 65536) along the length of the strip
        // (strip.numPixels() steps):
        int pixelHue = (i * 65536L / 168);
        // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
        // optionally add saturation and value (brightness) (each 0 to 255).
        // Here we're using just the single-argument hue variant. The result
        // is passed through strip.gamma32() to provide 'truer' colors
        // before assigning to each pixel:
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
        strip.show(); // Update strip with new contents
      }
      while (animation == 2) {
        button.tick();
        if (bounce == 0) {
          brightnessChange = brightness / 4;
        }
        if (count == 75) {
          if (firstPixelHue < 3 * 65536) {
            if (bounce == 1) {
              brightness -= brightnessChange;
              strip.setBrightness(brightness);
              strip.show();
              loops++;
            }
            else if (bounce == 2) {
              brightness += brightnessChange;
              strip.setBrightness(brightness);
              strip.show();
              loops++;
            }
            if (loops == 3) {
              bounce = 2;
            }
            else if (loops == 6) {
              bounce = 0;
              loops = 0;
            }
            strip.clear();
            button.tick();
            for (int i = 0; i < 168; i++) { // For each pixel in strip...
              // Offset pixel hue by an amount to make one full revolution of the
              // color wheel (range of 65536) along the length of the strip
              // (strip.numPixels() steps):
              int pixelHue = firstPixelHue + (i * 65536L / 168);
              // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
              // optionally add saturation and value (brightness) (each 0 to 255).
              // Here we're using just the single-argument hue variant. The result
              // is passed through strip.gamma32() to provide 'truer' colors
              // before assigning to each pixel:
              strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
            }
            strip.show(); // Update strip with new contents
            firstPixelHue += 256;
          }
          else {
            firstPixelHue = 0;
          }
          count = 0;
        }
        button.tick();
        recordAccelRegisters(); // Record the accelerometer values
        button.tick();
        if (abs(gForceX) > .5 || abs(gForceY) > .5) {
          if ( bounce == 0) {
            bounce = 1;
          }
        }
        count ++;
      }
      strip.clear();
    }



    if (animation == 3) {
      int loops = 0;
      int bounce = 0;
      int b = 0;
      int count = 0;
      int brightnessChange;
      for (int i = 0; i < 169; i += 3) {
        strip.fill(strip.Color(redStart, greenStart, blueStart), i, 1);
        strip.show();
        delay(7);
      }
      while (animation == 3) {
        button.tick();
        if (bounce == 0) {
          brightnessChange = brightness / 4;
        }
        if (count == 75) {
          if (b < 3) {
            if (bounce == 1) {
              brightness -= brightnessChange;
              strip.setBrightness(brightness);
              strip.show();
              loops++;
            }
            else if (bounce == 2) {
              brightness += brightnessChange;
              strip.setBrightness(brightness);
              strip.show();
              loops++;
            }
            if (loops == 3) {
              bounce = 2;
            }
            else if (loops == 6) {
              bounce = 0;
              loops = 0;
            }
            strip.clear();
            button.tick();
            for (int c = b; c < 169; c += 3) {
              strip.setPixelColor(c, strip.Color(redStart, greenStart, blueStart)); // Set pixel 'c' to value color
            }
            strip.show(); // Update strip with new contents
            b++;
          }
          else {
            b = 0;
          }
          count = 0;
        }
        button.tick();
        recordAccelRegisters(); // Record the accelerometer values
        button.tick();
        if (abs(gForceX) > .5 || abs(gForceY) > .5) {
          if ( bounce == 0) {
            bounce = 1;
          }
        }
        count ++;
      }
      strip.clear();
    }


    if (animation == 4) {
      int loops = 0;
      int bounce = 0;
      int b = 0;
      int count = 0;
      int brightnessChange;
      int firstPixelHue = 0;
      for (int i = 0; i < 168; i += 3) { // For each pixel in strip...
        // Offset pixel hue by an amount to make one full revolution of the
        // color wheel (range of 65536) along the length of the strip
        // (strip.numPixels() steps):
        int pixelHue = (i * 65536L / 168);
        // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
        // optionally add saturation and value (brightness) (each 0 to 255).
        // Here we're using just the single-argument hue variant. The result
        // is passed through strip.gamma32() to provide 'truer' colors
        // before assigning to each pixel:
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
        strip.show(); // Update strip with new contents
        delay(5);
      }
      while (animation == 4) {
        button.tick();
        if (bounce == 0) {
          brightnessChange = brightness / 4;
        }
        if (count == 75) {
          if (b < 3) {
            if (bounce == 1) {
              brightness -= brightnessChange;
              strip.setBrightness(brightness);
              strip.show();
              loops++;
            }
            else if (bounce == 2) {
              brightness += brightnessChange;
              strip.setBrightness(brightness);
              strip.show();
              loops++;
            }
            if (loops == 3) {
              bounce = 2;
            }
            else if (loops == 6) {
              bounce = 0;
              loops = 0;
            }
            strip.clear();
            button.tick();
            for (int c = b; c < 168; c += 3) {
              // hue of pixel 'c' is offset by an amount to make one full
              // revolution of the color wheel (range 65536) along the length
              // of the strip (strip.numPixels() steps):
              int      hue   = firstPixelHue + c * 65536L / 168;
              uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
              strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
            }
            strip.show(); // Update strip with new contents
            b++;
            firstPixelHue += 65536 / 90;
          }
          else {
            b = 0;
          }
          count = 0;
        }
        button.tick();
        recordAccelRegisters(); // Record the accelerometer values
        button.tick();
        if (abs(gForceX) > .5 || abs(gForceY) > .5) {
          if ( bounce == 0) {
            bounce = 1;
          }
        }
        count ++;
      }
      strip.clear();
    }



    if (animation == 5) {
      randomSeed(analogRead(A2));
      int hue = random(0, 65536L);
      uint32_t color = strip.gamma32(strip.ColorHSV(hue));
      for (int i = 0; i < 85; i ++) {
        int j = 168 - i;
        strip.setPixelColor(i, color);
        strip.setPixelColor(j, color);
        strip.show();
      }
      while (animation == 5) {
        recordAccelRegisters(); // Record the accelerometer values
        button.tick();
        if (abs(gForceX) > .5 || abs(gForceY) > .5) {
          randomSeed(analogRead(A2));
          hue = random(0, 65536L);
          color = strip.gamma32(strip.ColorHSV(hue));
          for (int i = 0; i < 169; i ++) {
            strip.setPixelColor(i, color); // Set pixel 'c' to value color
          }
          strip.show(); // Update strip with new contents
          delay(100);
        }
      }
      strip.clear();
    }


    if (animation == 6) {
      int loops = 0;
      int bounce = 0;
      int b = 0;
      int count = 0;
      int brightnessChange;
      long firstPixelHue = 0;
      int pixelHue = (0);
      for (int i = 0; i < 168; i++) { // For each pixel in strip...
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(0)));
        strip.show(); // Update strip with new contents
      }
      while (animation == 6) {
        button.tick();
        if (bounce == 0) {
          brightnessChange = brightness / 4;
        }
        if (count == 75) {
          if (pixelHue < 65537) {
            if (bounce == 1) {
              brightness -= brightnessChange;
              strip.setBrightness(brightness);
              strip.show();
              loops++;
            }
            else if (bounce == 2) {
              brightness += brightnessChange;
              strip.setBrightness(brightness);
              strip.show();
              loops++;
            }
            if (loops == 3) {
              bounce = 2;
            }
            else if (loops == 6) {
              bounce = 0;
              loops = 0;
            }
            strip.clear();
            button.tick();
            for (int i = 0; i < 168; i++) { // For each pixel in strip...
              strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
            }
            strip.show(); // Update strip with new contents
            pixelHue += 256;
          }
          else {
            pixelHue = 0;
          }
          count = 0;
        }
        button.tick();
        recordAccelRegisters(); // Record the accelerometer values
        button.tick();
        if (abs(gForceX) > .5 || abs(gForceY) > .5) {
          if ( bounce == 0) {
            bounce = 1;
          }
        }
        count ++;
      }
      strip.clear();
    }
  }
}



// Set up the MPU
void setupMPU() {
  Wire.beginTransmission(0b1101000);// Access the MPU-6050
  Wire.write(0x6B);// Access register 6B power management
  Wire.write(0b00000000);// Turn sleep mode off
  Wire.endTransmission();
  Wire.beginTransmission(0b110100);// Access the MPU-6050
  Wire.write(0x1C);// Access register 1C accelerometer
  Wire.write(0b00000000);// Setting accelerometer to ±2g
  Wire.endTransmission();
  Wire.beginTransmission(0b110100);// Access the MPU-6050
  Wire.write(0x1A);// Access register 1A low pass filter
  Wire.write(0b00000011);// Setting low pass filter to 44Hz
  Wire.endTransmission();
}


// Record the accelerometer values
void recordAccelRegisters() {
  Wire.beginTransmission(0b1101000);// Access the MPU-6050
  Wire.write(0x3B);// Starting register for accelerometer readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, 6); // Request accelerometer readings from 6 registers (3B - 40)
  while (Wire.available() < 6);
  accelX = Wire.read() << 8 | Wire.read(); // Store first two bytes in X
  accelY = Wire.read() << 8 | Wire.read(); // Store second two bytes in Y
  accelZ = Wire.read() << 8 | Wire.read(); // Store last two bytes in Z
  processAccelData();
}


// Process the accelerometer values
void processAccelData() {
  gForceX = accelX / 16384.0;
  gForceY = accelY / 16384.0;
  gForceZ = accelZ / 16384.0;
}

// Startup animation
void startUp() {
  uint32_t color = strip.Color(255, 255, 0);
  int j;
  int k;
  int l;
  for (int i = 0; i < 42; i++) {
    j = 83 - i;
    k = 84 + i;
    l = 167 - i;
    strip.setPixelColor(i, color);
    strip.setPixelColor(j, color);
    strip.setPixelColor(k, color);
    strip.setPixelColor(l, color);
    strip.show();
    delay(5);
  }
  delay(75);
  for (int i = 41; i > -1; i--) {
    j = 83 - i;
    k = 84 + i;
    l = 167 - i;
    strip.setPixelColor(i, 0);
    strip.setPixelColor(j, 0);
    strip.setPixelColor(k, 0);
    strip.setPixelColor(l, 0);
    strip.show();
    delay(5);
  }
  delay(75);
  strip.clear();
  strip.show();
}



// Serving animation
void serve() {
  int c = 0;
  int red = 0;
  int green = 0;
  int blue = 0;
  int redAmount;
  int greenAmount;
  int blueAmount;

  //if (brightnessIncrease == 0) {
  //Player 1 color and position
  if (servingPlayer == 1) {
    redAmount = 0;
    greenAmount = 0;
    blueAmount = 10;
    if (previousServingPlayer == 1 && pixelStart1 == 8) {
      pixelStart1 = 50;
      pixelStart2 = 8;
    }
    else if (previousServingPlayer == 1 && pixelStart1 == 50) {
      pixelStart1 = 8;
      pixelStart2 = 50;
    }
    pixelStart = pixelStart1;
    previousServingPlayer = 1;
  }

  //Player 2 color and position
  if (servingPlayer == 2) {
    redAmount = 10;
    greenAmount = 0;
    blueAmount = 0;
    if (previousServingPlayer == 2 && pixelStart2 == 50) {
      pixelStart2 = 8;
      pixelStart1 = 50;
    }
    else if (previousServingPlayer == 2 && pixelStart2 == 8) {
      pixelStart2 = 50;
      pixelStart1 = 8;
    }
    pixelStart = pixelStart2;
    previousServingPlayer = 2;
  }


  //Player 3 color and position
  if (servingPlayer == 3) {
    redAmount = 10;
    greenAmount = 10;
    blueAmount = 0;
    if (previousServingPlayer == 3 && pixelStart3 == 92) {
      pixelStart3 = 134;
      pixelStart4 = 92;
    }
    else if (previousServingPlayer == 3 && pixelStart3 == 134) {
      pixelStart3 = 92;
      pixelStart4 = 134;
    }
    pixelStart = pixelStart3;
    previousServingPlayer = 3;
  }


  //Player 4 color and position
  if (servingPlayer == 4) {
    redAmount = 0;
    greenAmount = 10;
    blueAmount = 0;
    if (previousServingPlayer == 4 && pixelStart4 == 134) {
      pixelStart4 = 92;
      pixelStart3 = 134;
    }
    else if (previousServingPlayer == 4 && pixelStart4 == 92) {
      pixelStart4 = 134;
      pixelStart3 = 92;
    }
    pixelStart = pixelStart4;
    previousServingPlayer = 4;
  }


  //Make player 1 colors
  strip.fill(playerColor1, pixelStart1, 26);
  strip.fill(playerColor2, pixelStart1 + 9, 8);

  //Make player 2 colors
  strip.fill(playerColor2, pixelStart2, 26);
  strip.fill(playerColor1, pixelStart2 + 9, 8);

  //Make player 3 colors
  strip.fill(playerColor3, pixelStart3, 26);
  strip.fill(playerColor4, pixelStart3 + 9, 8);

  //Make player 4 colors
  strip.fill(playerColor4, pixelStart4, 26);
  strip.fill(playerColor3, pixelStart4 + 9, 8);

  //Write color to LEDs
  strip.show();

  recordAccelRegisters();

  while (abs(gForceX) < 1 && abs(gForceY) < 1 ) {
    if (c < 24) {
      button.tick();
      strip.fill(strip.Color(red, green, blue), pixelStart + 9, 8);
      strip.show();
      red += redAmount;
      green += greenAmount;
      blue += blueAmount;
    }
    else if ( c > 23 && c < 48) {
      button.tick();
      strip.fill(strip.Color(red, green, blue), pixelStart + 9, 8);
      strip.show();
      red -= redAmount;
      green -= greenAmount;
      blue -= blueAmount;
    }
    else if ( c > 47) {
      button.tick();
      c = -1;
    }

    c++;
    button.tick();
    recordAccelRegisters();       // Record the accelerometer values
    delay(10);
  }
}



// Bounce animation
void bounce() {
  waitForBounce = millis();
  int brightnessChange = brightness / 10;
  for (int i = 0; i < 7; i++) {
    brightness -= brightnessChange;
    strip.setBrightness(brightness);
    strip.show();
    delay(7);
  }
  for (int i = 0; i < 7; i++) {
    brightness += brightnessChange;
    strip.setBrightness(brightness);
    strip.show();
    delay(7);
  }
  if (teamWithBall == 1) {
    teamWithBall = 2;
  }
  else {
    teamWithBall = 1;
  }
}


//Bounce 2 animation
void bounce2() {
  int red = redStart;
  int green = greenStart;
  int blue = blueStart;
  int brightnessChange = brightness / 10;
  for (int i = 0; i < 7; i++) {
    brightness -= brightnessChange;
    strip.setBrightness(brightness);
    strip.show();
    delay(7);
  }
  for (int i = 0; i < 7; i++) {
    brightness += brightnessChange;
    strip.setBrightness(brightness);
    strip.show();
    delay(7);
  }
  strip.fill(strip.Color(red, green, blue), 0, 169);
  strip.show();
  delay(100);
}




// Point scored animation
void pointScored() {
  strip.clear();
  int red = 0;
  int green = 0;
  int blue = 0;

  if (teamWithBall == 2 && serveTeam == 1) {    //Point for team 1
    scoreTeam1++;
    serveTeam = 1;
    previousTeam = 1;
    previousServe = servingPlayer;
    previousScore = 1;
    teamWithBall = 1;
    eraseLastPoint = 1;
  }
  else if (teamWithBall == 2 && serveTeam == 2) {
    scoreTeam1++;
    serveTeam = 1;
    previousTeam = 2;
    previousServe = servingPlayer;
    previousScore = 1;
    servingPlayer += 2;
    teamWithBall = 1;
    eraseLastPoint = 1;
  }
  else if (teamWithBall == 1 && serveTeam == 2) {
    scoreTeam2++;
    serveTeam = 2;
    previousTeam = 2;
    previousServe = servingPlayer;
    previousScore = 2;
    teamWithBall = 2;
    eraseLastPoint = 1;
  }
  else if (teamWithBall == 1 && serveTeam == 1) {
    scoreTeam2++;
    serveTeam = 2;
    previousTeam = 1;
    previousServe = servingPlayer;
    previousScore = 2;
    servingPlayer += 2;
    teamWithBall = 2;
    eraseLastPoint = 1;
  }
  if (servingPlayer == 5) {
    servingPlayer = 2;
  }
  else if (servingPlayer == 6) {
    servingPlayer = 1;
  }
  waitForServe = 1;

  if (serveTeam == 1) {
    for (int x = 0; x < 2; x++) {
      int a = 0;
      for (int i = 0; i < 85; i++) {
        red = 0;
        blue = 0;
        int l = i + 59;
        for (int j = i - 26; j < i; j++) {
          strip.setPixelColor(j, strip.Color(red, 0, 0));
          strip.setPixelColor(l, strip.Color(0, 0, blue));
          l++;
          red += 10;
          blue += 10;
        }
        red = 0;
        blue = 0;
        for (int h = 143 + a; h < 169; h++) {
          strip.setPixelColor(h, strip.Color(red, 0, 0));
          red += 10;
        }
        a++;
        strip.show();
      }
      a = 1;
      for (int i = 1; i < 85; i++) {
        red = 0;
        blue = 0;
        int l = i + 58;
        for (int j = i - 26; j < i; j++) {
          strip.setPixelColor(j, strip.Color(0, 0, blue));
          strip.setPixelColor(l, strip.Color(red, 0, 0));
          l++;
          red += 10;
          blue += 10;
        }
        red = 0;
        blue = 0;
        for (int h = 143 + a; h < 169; h++) {
          strip.setPixelColor(h, strip.Color(0, 0, blue));
          blue += 10;
        }
        a++;
        strip.show();
      }
    }
    strip.clear();
  }
  else if (serveTeam == 2) {
    for (int x = 0; x < 2; x++) {
      int a = 0;
      for (int i = 0; i < 85; i++) {
        red = 0;
        green = 0;
        int l = i + 59;
        for (int j = i - 26; j < i; j++) {
          strip.setPixelColor(j, strip.Color(red, green, 0));
          strip.setPixelColor(l, strip.Color(0, green, 0));
          l++;
          red += 10;
          green += 10;
        }
        red = 0;
        green = 0;
        for (int h = 143 + a; h < 169; h++) {
          strip.setPixelColor(h, strip.Color(red, green, 0));
          red += 10;
          green += 10;
        }
        a++;
        strip.show();
      }
      a = 1;
      for (int i = 1; i < 85; i++) {
        red = 0;
        green = 0;
        int l = i + 58;
        for (int j = i - 26; j < i; j++) {
          strip.setPixelColor(j, strip.Color(0, green, 0));
          strip.setPixelColor(l, strip.Color(red, green, 0));
          l++;
          red += 10;
          green += 10;
        }
        red = 0;
        green = 0;
        for (int h = 143 + a; h < 169; h++) {
          strip.setPixelColor(h, strip.Color(0, green, 0));
          green += 10;
        }
        a++;
        strip.show();
      }
    }
    strip.clear();
  }
}

void displayScore() {
  int red = 0;
  int green = 0;
  int blue = 0;
  int alternate = 1;

  strip.clear();
  for (int i = 0; i < scoreTeam1 * 2; i += 2) {
    if (alternate > 0) {
      strip.setPixelColor(i, playerColor1);
    }
    else {
      strip.setPixelColor(i, playerColor2);
    }
    alternate *= -1;
  }
  alternate = 1;
  for (int j = 70 ; j < 70 + (scoreTeam2 * 2); j += 2) {
    if (alternate > 0) {
      strip.setPixelColor(j, playerColor3);
    }
    else {
      strip.setPixelColor(j, playerColor4);
    }
    alternate *= -1;
  }
  strip.show();
  delay(1500);
  strip.clear();
  if (scoreTeam1 >= 15 && scoreTeam1 >= (scoreTeam2 + 2)) {   //check to see if Team1 has won
    for (int k = 0; k < 6; k++) {                             //and if so, play winning animation
      for (int c = 0; c < 26; c++) {
        strip.fill(strip.Color(red, green, blue), 0, 169);
        red += 10;
        strip.show();
      }
      for (int c = 0; c < 26; c++) {
        strip.fill(strip.Color(red, green, blue), 0, 169);
        red -= 10;
        strip.show();
      }
    }
    strip.clear();
    whoWon = 1;
    newGame();
  }
  else if (scoreTeam1 >= 15 && scoreTeam2 >= (scoreTeam1 + 2)) {   //check to see if Team2 has won
    for (int k = 0; k < 6; k++) {                                  //and if so, play winning animation
      for (int c = 0; c < 13; c++) {
        strip.fill(strip.Color(red, green, blue), 0, 169);
        blue += 10;
        strip.show();
      }
      for (int c = 0; c < 13; c++) {
        strip.fill(strip.Color(red, green, blue), 0, 169);
        blue -= 10;
        strip.show();
      }
    }
    strip.clear();
    whoWon = 2;
    newGame();
  }
  strip.clear();
}


// New game animation and parameters
void newGame() {
  scoreTeam1 = 0;
  scoreTeam2 = 0;
  waitForServe = 1;
  previousServingPlayer = 0;
  pixelStart1 = 8;
  pixelStart2 = 50;
  pixelStart3 = 92;
  pixelStart4 = 134;
  randomSeed(analogRead(A2));
  if (whoWon == 1) {
    teamWithBall = 1;
    serveTeam = 1;
    servingPlayer = random(1, 3);
    if (servingPlayer == 1) {
      pixelStart = 8;
    }
    else {
      pixelStart = 50;
    }
  }
  else if (whoWon == 2) {
    teamWithBall = 2;
    serveTeam = 2;
    servingPlayer = random(3, 5);
    if (servingPlayer == 3) {
      pixelStart = 92;
    }
    else {
      pixelStart = 134;
    }
  }
  else {
    serveTeam = random(1, 3);
    if (serveTeam == 1) {
      teamWithBall = 1;
      servingPlayer = random(1, 3);
      if (servingPlayer == 1) {
        pixelStart = 8;
      }
      else {
        pixelStart = 50;
      }
    }
    else if (serveTeam == 2) {
      teamWithBall = 2;
      servingPlayer = random(3, 5);
      if (servingPlayer == 3) {
        pixelStart = 92;
      }
      else {
        pixelStart = 134;
      }
    }
  }

  int red = 0;
  int blue = 0;
  int green = 0;
  strip.clear();
  for (int x = 0; x < 2; x++) {
    int a = 0;
    for (int i = 0; i < 42; i++) {
      red = 0;
      green = 0;
      blue = 0;
      int l = i + 16;
      int k = l + 42;
      int m = k + 42;
      for (int j = i - 26; j < i; j++) {
        strip.setPixelColor(j, strip.Color(red, green, 0));
        strip.setPixelColor(l, strip.Color(0, green, 0));
        strip.setPixelColor(k, strip.Color(red, 0, 0));
        strip.setPixelColor(m, strip.Color(0, 0, blue));
        l++;
        k++;
        m++;
        red += 10;
        green += 10;
        blue += 10;
      }
      red = 0;
      green = 0;
      blue = 0;
      for (int h = 143 + a; h < 169; h++) {
        strip.setPixelColor(h, strip.Color(red, green, 0));
        red += 10;
        green += 10;
      }
      a++;
      strip.show();
      delay(5);
    }
    a = 1;
    for (int i = 0; i < 42; i++) {
      red = 0;
      green = 0;
      blue = 0;
      int l = i + 16;
      int k = l + 42;
      int m = k + 42;
      for (int j = i - 26; j < i; j++) {
        strip.setPixelColor(l, strip.Color(red, green, 0));
        strip.setPixelColor(k, strip.Color(0, green, 0));
        strip.setPixelColor(m, strip.Color(red, 0, 0));
        strip.setPixelColor(j, strip.Color(0, 0, blue));
        l++;
        k++;
        m++;
        red += 10;
        green += 10;
        blue += 10;
      }
      red = 0;
      green = 0;
      blue = 0;
      for (int h = 143 + a; h < 169; h++) {
        strip.setPixelColor(h, strip.Color(0, 0, blue));
        blue += 10;
      }
      a++;
      strip.show();
      delay(5);
    }
    a = 0;
    for (int i = 0; i < 42; i++) {
      red = 0;
      green = 0;
      blue = 0;
      int l = i + 16;
      int k = l + 42;
      int m = k + 42;
      for (int j = i - 26; j < i; j++) {
        strip.setPixelColor(k, strip.Color(red, green, 0));
        strip.setPixelColor(m, strip.Color(0, green, 0));
        strip.setPixelColor(j, strip.Color(red, 0, 0));
        strip.setPixelColor(l, strip.Color(0, 0, blue));
        l++;
        k++;
        m++;
        red += 10;
        green += 10;
        blue += 10;
      }
      red = 0;
      green = 0;
      blue = 0;
      for (int h = 143 + a; h < 169; h++) {
        strip.setPixelColor(h, strip.Color(red, 0, 0));
        red += 10;
      }
      a++;
      strip.show();
      delay(5);
    }
    a = 1;
    for (int i = 0; i < 42; i++) {
      red = 0;
      green = 0;
      blue = 0;
      int l = i + 16;
      int k = l + 42;
      int m = k + 42;
      for (int j = i - 26; j < i; j++) {
        strip.setPixelColor(m, strip.Color(red, green, 0));
        strip.setPixelColor(j, strip.Color(0, green, 0));
        strip.setPixelColor(l, strip.Color(red, 0, 0));
        strip.setPixelColor(k, strip.Color(0, 0, blue));
        l++;
        k++;
        m++;
        red += 10;
        green += 10;
        blue += 10;
      }
      red = 0;
      green = 0;
      blue = 0;
      for (int h = 143 + a; h < 169; h++) {
        strip.setPixelColor(h, strip.Color(0, green, 0));
        green += 10;
      }
      a++;
      strip.show();
      delay(5);
    }
  }
  strip.clear();
}


// Erase last point animation
void erasePoint() {
  uint32_t color1 = strip.Color(255, 0, 0);
  uint32_t color2 = strip.Color(0, 0, 255);
  int j;
  int k;
  int l;
  for (int i = 0; i < 42; i++) {
    j = 83 - i;
    k = 84 + i;
    l = 167 - i;
    strip.setPixelColor(i, color1);
    strip.setPixelColor(j, color2);
    strip.setPixelColor(k, color2);
    strip.setPixelColor(l, color1);
    strip.show();
    //delay(5);
  }
  delay(75);
  for (int i = 41; i > -1; i--) {
    j = 83 - i;
    k = 84 + i;
    l = 167 - i;
    strip.setPixelColor(i, 0);
    strip.setPixelColor(j, 0);
    strip.setPixelColor(k, 0);
    strip.setPixelColor(l, 0);
    strip.show();
    //delay(5);
  }
  //delay(75);
  strip.clear();
  strip.show();
}



// This function will be called when the button1 was pressed 1 time (and no 2. button press followed).
void doubleclick1() {
  if (scoreKeeper == 1) {
    if (eraseLastPoint == 1) {
      if (previousScore == 1) {
        if (scoreTeam1 > 0) {
          displayScore();
          scoreTeam1-- ;
          serveTeam = previousTeam;
          servingPlayer = previousServe;
          teamWithBall = previousTeam;
          erasePoint();
          displayScore();
        }
      }
      else if (previousScore == 2) {
        if ( scoreTeam2 > 0) {
          displayScore();
          scoreTeam2-- ;
          serveTeam = previousTeam;
          servingPlayer = previousServe;
          teamWithBall = previousTeam;
          erasePoint();
          displayScore();
        }
      }
      eraseLastPoint = 0;
      serve();
    }
  }
  //Funtions for type 2
  else {
    if (animation == 1 || animation == 3) {
      animationColor++;
      if (animationColor == 1) {    //Green
        redStart = 0;
        greenStart = 255;
        blueStart = 0;
        redAmount = 0;
        greenAmount = 20;
        blueAmount = 0;
      }
      else if (animationColor == 2) {   //Blue
        redStart = 0;
        greenStart = 0;
        blueStart = 255;
        redAmount = 0;
        greenAmount = 0;
        blueAmount = 20;
      }
      else if (animationColor == 3) {   //Yellow
        redStart = 255;
        greenStart = 255;
        blueStart = 0;
        redAmount = 20;
        greenAmount = 20;
        blueAmount = 0;
      }
      else if (animationColor == 4) {    //Orange
        redStart = 255;
        greenStart = 127;
        blueStart = 0;
        redAmount = 20;
        greenAmount = 20;
        blueAmount = 0;
      }
      else if (animationColor == 5) {   //Pink
        redStart = 255;
        greenStart = 0;
        blueStart = 127;
        redAmount = 20;
        greenAmount = 0;
        blueAmount = 20;
      }
      else if (animationColor == 6) {   //Magenta
        redStart = 255;
        greenStart = 0;
        blueStart = 127;
        redAmount = 20;
        greenAmount = 0;
        blueAmount = 20;
      }
      else if (animationColor == 7) {   //Purple
        redStart = 127;
        greenStart = 0;
        blueStart = 255;
        redAmount = 20;
        greenAmount = 0;
        blueAmount = 20;
      }
      else if (animationColor == 8) {   //Sky Blue
        redStart = 0;
        greenStart = 127;
        blueStart = 255;
        redAmount = 0;
        greenAmount = 20;
        blueAmount = 20;
      }
      else if (animationColor == 9) {   //Turqoise
        redStart = 0;
        greenStart = 255;
        blueStart = 255;
        redAmount = 0;
        greenAmount = 20;
        blueAmount = 20;
      }
      else if (animationColor == 10) {    //Teal
        redStart = 0;
        greenStart = 255;
        blueStart = 127;
        redAmount = 0;
        greenAmount = 20;
        blueAmount = 20;
      }
      else if (animationColor == 11) {   //Lime Green
        redStart = 127;
        greenStart = 255;
        blueStart = 0;
        redAmount = 20;
        greenAmount = 20;
        blueAmount = 0;
      }
      else if (animationColor == 12) {
        redStart = 255;
        greenStart = 0;
        blueStart = 0;
        redAmount = 20;
        greenAmount = 0;
        blueAmount = 0;
        animationColor = 0;
      }
    }
    if (animation == 1) {
      bounce2();
    }
  } // click1
}

// This function will be called when the button1 was pressed 2 times in a short timeframe.
void click1() {

  if (longPress == 1) {
    if (brightness < 100) {
      brightness += 10;
      strip.setBrightness(brightness);
    }
    else {
      brightness = 10;
    }
    strip.show();
  }
  longPress = 1;
} // doubleclick1


// This function will be called once, when the button1 is pressed for a long time.
void longPressStart1() {
  longPress = 0;
  if (scoreKeeper == 1) {
    whoWon = 0;
    newGame();
    serve();
  }
  else {
    if (animation < 6) {
      animation++;
    }
    else {
      animation = 1;
    }
  }
} // longPressStart1
