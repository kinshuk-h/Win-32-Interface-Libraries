#include "UI_Element.hpp"
#include <exception>
using namespace std;
int main()
{
    try
    {
        /// Initialize the console, remove  cursor visibilty, enable mouse input and resize to 58,18.
        Console::Init(); Console::Cursor(0); Console::MouseInput(1); Console::Size(58,18);
        /// Move the window to the center, and set buffer size to fit the window.
        Console::Center(); Console::BufferSize(58,18);
        /// Declare 3 Image buttons.
        Image_Button I1("VEGETARIAN","VEG.afi",1,1,16,14,0,0,6,GRAY,WHITE);
        Image_Button I2("NON VEGETARIAN","NON_VEG.afi",20,1,16,14,0,0,6,GRAY,WHITE);
        Image_Button I3("EGGETARIAN","EGG.afi",39,1,16,14,0,0,6,GRAY,WHITE);
        /// Print the buttons, so that we can click on them.
        I1.Print(); I2.Print(); I3.Print();
        INPUT_RECORD rec; DWORD ct=0;
        while(true)
        {
            Sleep(100); ReadConsoleInput(Console::In(),&rec,1,&ct);
            switch(rec.EventType)
            {
            case MOUSE_EVENT:
                {
                    if(rec.Event.MouseEvent.dwButtonState==FROM_LEFT_1ST_BUTTON_PRESSED && rec.Event.MouseEvent.dwEventFlags == 0)
                    {
                        /// Check which button was clicked, and proceed accordingly.
                        if(I1.IsClicked(rec.Event.MouseEvent.dwMousePosition))
                        {
                            InfoBox("You Selected VEGETARIAN!","Computer",[](){});
                        }
                        else if(I2.IsClicked(rec.Event.MouseEvent.dwMousePosition))
                        {
                            InfoBox("You Selected NON VEGETARIAN!","Computer",[](){});
                        }
                        else if(I3.IsClicked(rec.Event.MouseEvent.dwMousePosition))
                        {
                            InfoBox("You Selected EGGETARIAN!","Computer",[](){});
                        }
                    }
                }
            }
        }
    }
    catch(exception& e)
    {
        ErrorBox(e.what(),"Error",[](){});
    }
}
