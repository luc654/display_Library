#include <Adafruit_SH110X.h> 

class DisplayLib {
  public:
    DisplayLib(Adafruit_SH1106G* displayObject); 
    void begin();
    void addText(const char* text, int xPos, int yPos);
    void addButton(const char* text, int xPos, int yPos, boolean active, void(*new_action)() = back);
    void cycle(boolean cycle);
    void updateScreen();
    void flush();
    void safeScreen(const char* name);
    void loadScreen(const char* name);
    static void back();

    private:
    Adafruit_SH1106G* _display; 
    void showText(const char* text, int xPos, int yPos);
    void showButton(const char* text, int xPos, int yPos, boolean active);
    void handleElement(struct DisplayElement);
};