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



## Components

Each component has different parameters. There are currently two components, text and buttons. 

### Text

A text component has three parameters, 
- Text
- xPos
- yPos

**Text** is relatively straight forward, its the text that shows up in the text component. There are currently no fonts or styling so each word will look the same

**xPos** is the position in pixels from the right most side

**yPos** is the position in pixels from the right most side

```
  displayController.addText("Hello, World!!", 10, 30);
```
In this example the text ```Hello, World!!``` appears somewhere in the middle of the screen.

### Button

A button component has three parameters, 
- Text
- xPos
- yPos
- active

**Text** is like on he text component, the text that appears inside the button

**xPos** is the position in pixels from the right most side

**yPos** is the position in pixels from the right most side

**active** is wether the button is active or not, it is recommended to only have one active button on the display at once.

```
  displayController.addButton("one", 2, 50, true);
  displayController.addButton("two", 45, 50, false);
  displayController.addButton("three", 90, 50, false);
```
In this example three buttons will appear on the bottom of the screen. Cycling between buttons happens from top to down, so if you press right while on button ```one``` you will cycle to button ```two```. If there are no further buttons then it will cycle back to the first button.
