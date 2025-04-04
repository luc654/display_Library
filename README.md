# C++ Microcontroller GUI library
A small and simple to understand library designed for the SSD1306 with SH110X Drivers.

## Dependencies
Requires Adafruit GFX library & AdafruitSH110x library

[Adafruit GFX library](https://github.com/adafruit/Adafruit-GFX-Library)
[Adafruit SH110X](https://github.com/adafruit/Adafruit_SH110x)

## Architecture
The code works by defining an nested array, ```screenElements```, and adding both the type e.g. "text" and parameters (differ per type) e.g. "'Hey', 10, 50". 
Since everything on screen is stored in one nested array its easy to update and display.


## Use
In the setup function start the display and define your elements

```
void setup() {
  Serial.begin(9600);
  displayController.begin(); 
  displayController.addText("Hello, World!!", 10, 30);
  displayController.addText("Hello, yes!!", 10, 50);
}
```

In the Loop function update the screen with a small delay to spare the microcontroller's processor.

```
void loop() {
  displayController.updateScreen();
  delay(2000);
}
```

