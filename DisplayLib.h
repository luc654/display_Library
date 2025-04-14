#include <Adafruit_SH110X.h> 

class DisplayLib {
  public:
    DisplayLib(Adafruit_SH1106G* displayObject); 
    void begin();
    void addText(const char* text, int xPos, int yPos, const char* identifier="");
    void setText(const char* identifier, const char* text, boolean all=true);
    void addButton(const char* text, int xPos, int yPos, boolean active, void(*new_action)());
    void addCheckbox(int xPos, int yPos, int size, boolean active, boolean clicked, void(*new_action)());
    void cycle(boolean cycle);
    void updateScreen();
    void flush();
    void safeScreen(const char* name);
    void resafeScreen(const char* name);
    void loadScreen(const char* name, int startPos = 0);
    void back();

    private:
    Adafruit_SH1106G* _display; 
    void showText(const char* text, int xPos, int yPos);
    void showButton(const char* text, int xPos, int yPos, boolean active);
    void showCheckbox(int xPos, int yPos, int size, boolean clicked, boolean active);
    void handleElement(struct DisplayElement);
    void updateSavedData(const char* screenName, int id, const char* key, boolean newValue);
};