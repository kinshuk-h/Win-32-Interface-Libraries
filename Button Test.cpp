#include "H:\All C++ Files\Custom Headers\KV\UI_Element.hpp"
#include <exception>
using namespace std;
int main()
{
    try
    {
        Console::Init(); Console::Cursor(0); Console::MouseInput(1); Console::Size(58,18);
        Console::Center(); Console::BufferSize(58,18);
        Image_Button I1("VEGETARIAN","H:\\All C++ Files\\Graphical Projects\\Pre-GUI\\ASCII Paint\\Saved ASCII Images\\VEG.afi",
                        1,1,16,14,0,0,6,GRAY,WHITE);
        Image_Button I2("NON VEGETARIAN","H:\\All C++ Files\\Graphical Projects\\Pre-GUI\\ASCII Paint\\Saved ASCII Images\\NON_VEG.afi",
                        20,1,16,14,0,0,6,GRAY,WHITE);
        Image_Button I3("EGGETARIAN","H:\\All C++ Files\\Graphical Projects\\Pre-GUI\\ASCII Paint\\Saved ASCII Images\\EGG.afi",
                        39,1,16,14,0,0,6,GRAY,WHITE);
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
