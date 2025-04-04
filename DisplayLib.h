#include <Adafruit_SH110X.h> 

class DisplayLib {
  public:
    DisplayLib(Adafruit_SH1106G* displayObject); 
    void begin();
    void addText(const char* text, int xPos, int yPos);
    void updateScreen();

    private:
    Adafruit_SH1106G* _display; 
    void showText(const char* text, int xPos, int yPos);
    void handleElement(struct DisplayElement);
};