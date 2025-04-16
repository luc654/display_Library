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
*See [screens](#screens) for indepth explanation*

In the Loop function update the screen with a small delay to spare the microcontroller's processor.

```
void loop() {
  displayController.updateScreen();
  delay(200);
}
```



## Components

Each component has different parameters. There are currently three components, text, buttons and checkboxes. 

There are also [lists](#lists) that store these components, these lists are usefull for quicker styling and nicer code reading.

### Text

A text component has four parameters, 
- Text
- xPos
- yPos
- identifier

**Text** is relatively straight forward, its the text that shows up in the text component. There are currently no fonts or styling so each word will look the same

**xPos** is the position in pixels from the right most side.

**yPos** is the position in pixels from the top most side.

**identifier** (string) is an optional parameter used for the [setText](#settext) function.

```
  displayController.addText("Hello, World!!", 10, 30);
```

In this example the text ```Hello, World!!``` appears somewhere in the middle of the screen.

---
### Button

A button component has four parameters, 
- Text
- xPos
- yPos 
- callback

**Text** is like on he text component, the text that appears inside the button

**xPos** is the position in pixels from the right most side.

**yPos** is the position in pixels from the top most side.

**callback** is the name of the function that gets called when the button is pressed. To call a function you must pass it as a lambda function: 
```
displayController.addButton("next", 90, 50, false, [](void) {
      displayController.loadScreen("settings");
  });
```
If for some reason you dont need a callback then you can pass a null pointer as parameter instead. 

There are two requirements that have to be met before a callback runs, first the button with the callback must  be active & the ```flush()``` function has to be called.

---
setup()
```
  displayController.addButton("one", 2, 50, DisplayLib::back);
  displayController.addButton("two", 45, 50, nullptr);
  displayController.addButton("three", 90, 50, nullptr);
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

## Checkboxes
Checkboxes are the most advanced of the components, each checkbox automatically stores its value in a global array so it keeps its value inbetween screen switches. 


A checkbox component has five parameters, 
- xPos
- yPos
- size
- clicked
- callback



**xPos** is the position in pixels from the right most side.

**yPos** is the position in pixels from the top most side.

**Size** X and Y size in pixels.

**clicked** is whether the checkbox has been clicked (crossed). Switches to TRUE once the flush function is called and a checkbox is active.


**callback** is the name of the function that gets called when the button is pressed. To call a function you must pass it as a lambda function: 
```
displayController.addButton("next", 90, 50, [](void) {
      displayController.loadScreen("settings");
  });
```
If for some reason you dont need a callback then you can pass a null pointer as parameter instead. 

There are two requirements that have to be met before a callback runs, first the button with the callback must  be active & the ```flush()``` function has to be called.



---

## Screens

Each screen is defined in the ```setup()``` part of the program. first you define each component and afterwards you give it a name. There are two functions related to screens, ```safeScreen(name)``` to safe a newly made screen (used in setup()) and ```loadScreen(name)``` to load a screen (used in both setup() and loop()).

```setup()```
```
//  Home screen 
  displayController.addText("Hello, World!!", 10, 30);
  displayController.addButton("one", 2, 50, DisplayLib::back);
  displayController.addButton("two", 45, 50, nullptr);
  displayController.addButton("next", 90, 50, [](void) {
      displayController.loadScreen("settings");
  });
  displayController.safeScreen("home");
  delay(20);

  // Settings screen
  displayController.addText("Wow another page?", 10, 30);
  displayController.addButton("Back", 2, 50, [](void) {
      displayController.loadScreen("home");
  });
  displayController.addButton("two", 45, 50, nullptr);
  displayController.addButton("three", 90, 50, nullptr);
  displayController.safeScreen("settings");

  displayController.loadScreen("home");
```
In this example two screens are made, the first screen gets the name ```home``` and is loaded at the end of the function.

## Active components

Each clickable component has a private attribute 'active'. By default the first clickable element on the screen is active (gets handled by loadScreen). To edit this you can pass the ID of a clickable through the parameters of loadscreen.

```
displayController.loadScreen("checks", 1);
```
In this example, the second clickable element on 'checks' screen gets active.

When ```flush()``` is called the active component's callback gets triggered (or incase the active component is a checkbox, gets selected.)


### Lists
A list is a collection of either buttons or text components, by passing a vector with the necessary parameters into an addList function and a few coordinates and a boolean of direction you can implement a list.

## Text list
To initialise a text list first make a vector of ```const char *```

```
std::vector<const char *> textList = { "One", "Two", "Three"};
```

Each element in the vector will be its own component.
Currently the change parameter isnt supported in text objects. This is on the todo list.


In the setup function call the addList function with the following parameters

**X position** in pixels
**Y position** in pixels
**Spacing** in pixels
**downwards** list direction, true = downwards, false = sideways
**vector** text vector of const * char

```
void setup() {
  Serial.begin(9600);
  dC.begin();
  dC.addList(2,15,15, false, textList);
```

## Button list
To initialise a text list first make a vector of ```ButtonDef```. ButtonDef is a custom struct stored in the header file, it contains a ```const char*``` which contains the button text and a ```void (*callback)()``` which is ofcourse the callback of te button.

```
std::vector<ButtonDef> btnList = {
  {"Home", []() {
    dC.loadScreen("home");
  }},
    {"Sleep", []() {
    dC.loadScreen("sleep");
  }},
    {"Settings", []() {
    dC.loadScreen("Settings");
  }}
};
```

Here I define a vector with three buttons, each button loads another screen. Each element in the vector will be its own button.

In the setup function call the addList function with the following parameters

**X position** in pixels
**Y position** in pixels
**Spacing** in pixels
**downwards** list direction, true = downwards, false = sideways
**vector** text vector of const * char

```
void setup() {
  Serial.begin(9600);
  dC.begin();
  dC.addBtnList( 2,50,5,false, btnList);
```

When setting direction to false (direcion sideways) the function will automatically space the buttons enough to ensure no overlap. THis means that spacing really means spacing.

## Handy functions

### Back()
Each call to load screen adds the name of the a history vector. By calling the back() function the latest entry in the history vector gets destroyed and the previous entry is called as parameter to the ```loadScreen(name)``` function.

Example
```
  displayController.addButton("Go back", 2, 50, [](void){
    displayController.back();
  });
```

### Flush()
Flush() activates the callback of the current active digital button. 

Example
```
  if(digitalRead(BUTTON_B)){
    displayController.flush();
  } 
```


### Settext()
Settext() sets the text of a given identifier.

```
  displayController.addText("Hello, World!!", 10, 30, "change");
  displayController.addButton("one", 2, 50, [](void){
    displayController.setText("change", "Example of changing text");
  });
```
In this example upon clicking button 'one' the text with identifier ```change``` switches to 'Example of changing text'