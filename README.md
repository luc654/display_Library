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

---
### Button

A button component has five parameters, 
- Text
- xPos
- yPos
- active
- callback

**Text** is like on he text component, the text that appears inside the button

**xPos** is the position in pixels from the right most side

**yPos** is the position in pixels from the right most side

**active** is whether the button is active or not, it is recommended to only have one active button on the display at once.

**callback** is the name of the function that gets called when the button is pressed. To call a function native to the library you must pass the library name with it, such as ```DisplayLib::back```. 
If for some reason you dont need a callback then you can pass a null pointer as parameter instead. 
You can also add lambda style callbacks so you can pass parameters through functions like here 
```
displayController.addButton("next", 90, 50, false, [](void) {
      displayController.loadScreen("settings");
  });
```

There are two requirements that have to be met before a callback runs, first the button with the callback must  be active & the ```flush()``` function has to be called.

---
setup()
```
  displayController.addButton("one", 2, 50, true, DisplayLib::back);
  displayController.addButton("two", 45, 50, false, nullptr);
  displayController.addButton("three", 90, 50, false, nullptr);
```

loop()
```
void loop() {
  displayController.updateScreen();

    if(digitalRead(BUTTON_A)){
    displayController.cycle(true);
  } 
  if(digitalRead(BUTTON_B)){
    displayController.flush();
  } 

  if(digitalRead(BUTTON_C)){
    displayController.cycle(false);
  } 
  
  
  delay(200);
}
```
In this example three buttons will appear on the bottom of the screen. Cycling between buttons happens from top to down, so if you press right while on button ```one``` you will cycle to button ```two```. If there are no further buttons then it will cycle back to the first button.

Analog button B will **flush** the active digital button, is digital button 'one' is active it will run function ```DisplayLib::back```. Digital button 'two' and 'three' both have a null pointer as parameter meaning they have no callback. 

Analog button A and C both cycle between the digital buttons. The ```true``` parameter means cycle forward and ```false``` will cycle backwards. 

---


## Screens

Each screen is defined in the ```setup()``` part of the program. first you define each component and afterwards you give it a name. There are two functions related to screens, ```safeScreen(name)``` to safe a newly made screen (used in setup()) and ```loadScreen(name)``` to load a screen (used in both setup() and loop()).

```setup()```
```
//  Home screen 
  displayController.addText("Hello, World!!", 10, 30);
  displayController.addButton("one", 2, 50, true, DisplayLib::back);
  displayController.addButton("two", 45, 50, false, nullptr);
  displayController.addButton("next", 90, 50, false, [](void) {
      displayController.loadScreen("settings");
  });
  displayController.safeScreen("home");
  delay(20);

  // Settings screen
  displayController.addText("Wow another page?", 10, 30);
  displayController.addButton("Back", 2, 50, true, [](void) {
      displayController.loadScreen("home");
  });
  displayController.addButton("two", 45, 50, false, nullptr);
  displayController.addButton("three", 90, 50, false, nullptr);
  displayController.safeScreen("settings");

  displayController.loadScreen("home");
```
In this example two screens are made, the first screen gets the name ```home``` and is loaded at the end of the function.



## Handy functions

### Back()
Each call to load screen adds the name of the a history vector. By calling the back() function the latest entry in the history vector gets destroyed and the previous entry is called as parameter to the ```loadScreen(name)``` function.

Example
```
  displayController.addButton("Go back", 2, 50, true, DisplayLib::back);
```

### Flush()
Flush() activates the callback of the current active digital button. 

Example
```
  if(digitalRead(BUTTON_B)){
    displayController.flush();
  } 
```