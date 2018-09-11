#ifndef UI_ELEMENT_HPP
#define UI_ELEMENT_HPP

#include "Windows.hpp"
#include <locale>
#include <vector>
#include <regex>

/// Wrapper to enforce reserved space checks for use before printing to ostream object or reading from istream object.
template <class T> auto& check(T& obj)
{
    std::map<size_t,SMALL_RECT> Res = Console::Reserved(); CONSOLE_SCREEN_BUFFER_INFO cb; SMALL_RECT s;
    GetConsoleScreenBufferInfo(Console::Out(),&cb); COORD c = cb.dwCursorPosition;
    for(std::pair<size_t,SMALL_RECT> ps : Res)
    {
        s = ps.second; if((c.X>=s.Left&&c.X<=s.Right)&&(c.Y>=s.Top&&c.Y<=s.Bottom))
            throw std::logic_error(">> Console : Space reserved for UI Element!");
    }
    return obj;
}

/// Sealed Button Class (UI Element) for Interface Definition.
class BUTTON_TYPE
{
protected:
    std::string Text; COORD Pos,Size; size_t IDX; bool lckd;
    Color::Foreground Border_FGCol,FGCol; Color::Background Border_BGCol,BGCol;
    /// Reserves space for Buttons on the Console Window.
    void Lock() { IDX = Console::Reserve(Pos.X,Pos.Y,Pos.X+Size.X+1,Pos.Y+Size.Y+1); lckd = true;}
    /// Release space for the Button on the Console Window.
    void Unlock() { Console::Release(IDX); lckd = false; }
    /// Constructor for Button Elements.
    BUTTON_TYPE(std::string T, SHORT X, SHORT Y, SHORT W, SHORT H, Color::Foreground Bord_FC,
                Color::Foreground FC, Color::Background Bord_BC, Color::Background BC) :
    Text(T), Border_FGCol(Bord_FC), FGCol(FC), Border_BGCol(Bord_BC), BGCol(BC)
    {
        Pos = {X,Y}; if(W<=0) {W=1;} if(H<=0) {H=1;}
        Size = {((T.size()+2)>static_cast<unsigned>(W))?static_cast<SHORT>(T.length()+2):W,H};
    }
public:
    /// Checks if Space for UI Element is reserved on the Console Window.
    bool Locked() { return lckd;}
    /// Assignment Operator for the Button Elements.
    BUTTON_TYPE& operator=(const BUTTON_TYPE& b)
    {
        if(this==&b) return *this;
        else
        {
            this->Text = b.Text.data(); this->Pos = b.Pos; this->Size = b.Size; this->FGCol = b.FGCol; this->BGCol = b.BGCol;
            this->Border_FGCol = b.Border_FGCol; this->Border_BGCol = b.Border_BGCol; return *this;
        }
    }
    /// Recolors the Button Border and the Area inside it.
    void Recolor(Color::Foreground FC,Color::Background BC, Color::Foreground B_FC, Color::Background B_BC)
    { FGCol = FC; Border_FGCol = B_FC; BGCol = BC; Border_BGCol = B_BC; }
    /// Checks if the Button was clicked, by passing it the Current Mouse Position from the Mouse Event Record.
    bool IsClicked(COORD C) { return ((C.X>=(Pos.X+1)&&C.X<=(Pos.X+Size.X))&&(C.Y>=(Pos.Y+1)&&C.Y<=(Pos.Y+Size.Y))); }
    /// Prints the button on the Console Window.
    virtual void Print()
    {
        CONSOLE_SCREEN_BUFFER_INFO sbi; GetConsoleScreenBufferInfo(Console::Out(),&sbi);
        SHORT TextLen = Text.size(), i=1, WS = Size.X - TextLen; std::string LineSpc, SuppSpc;
        Console::Color(Border_FGCol,Border_BGCol); GOTO(Pos.X,Pos.Y);
        std::cout<<ULB; for(int i=0;i<Size.X;i++) std::cout<<HT; std::cout<<URB;
        for(int i=0;i<(WS/2);i++) {SuppSpc+=" ";} for(int i=0;i<TextLen;i++) LineSpc+=" ";
        std::string TempText = SuppSpc+Text+SuppSpc; LineSpc+=(SuppSpc+SuppSpc);
        for(;i<=((Size.Y-1)/2);i++)
        {
            GOTO(Pos.X,Pos.Y+i); Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
            Console::Color(FGCol,BGCol); std::cout<<LineSpc;
            Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
        }
        GOTO(Pos.X,Pos.Y+i); Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
        Console::Color(FGCol,BGCol); std::cout<<TempText; i++;
        Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
        for(;i<=Size.Y;i++)
        {
            GOTO(Pos.X,Pos.Y+i); Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
            Console::Color(FGCol,BGCol); std::cout<<LineSpc;
            Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
        }
        GOTO(Pos.X,Pos.Y+Size.Y+1); std::cout<<DLB; for(int i=0;i<Size.X;i++) std::cout<<HT; std::cout<<DRB;
        SetConsoleTextAttribute(Console::Out(),sbi.wAttributes);
    }
    /// Clears the button from the Console Window. Note that the space for the button is still reserved.
    void Clear()
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi; GetConsoleScreenBufferInfo(Console::Out(),&csbi);
        SetConsoleTextAttribute(Console::Out(),csbi.wAttributes);
        for(SHORT i=0;i<Size.Y+2;i++) { GOTO(Pos.X,i+Pos.Y); for(SHORT i=0;i<Size.X+2;i++) std::cout<<" "; }
    }
    /// Deletes the button memory, and releases the reserved space on the console.
    virtual ~BUTTON_TYPE(){ if(Locked()) Unlock(); }
};

/// Standard Button Class : For User Interaction with the Console via Mouse
class Button : public BUTTON_TYPE
{
public:
    /// Constructor for the Button
    Button(std::string data="", SHORT X=0, SHORT Y=0, SHORT W=1, SHORT H=1, Color::Foreground Bord_FC=BLACK,
           Color::Foreground FC=BLACK, Color::Background Bord_BC=BG_BLACK, Color::Background BC=BG_BLACK) :
    BUTTON_TYPE(data,X,Y,W,H,Bord_FC,FC,Bord_BC,BC)
    { if(static_cast<unsigned>(Size.X%2)!=(Text.size()%2)){Size.X++;} if(Size.Y%2==0) {Size.Y++;} Unlock(); Lock(); }
    /// Changes color of button to 'Active' when called. Recommended to be called after IsClicked() returns true.
    void Activate()
    {
        WORD F = static_cast<WORD>(FGCol), B = static_cast<WORD>(BGCol); B/=16; std::swap(F,B); B*=16;
        FGCol = static_cast<Color::Foreground>(F); BGCol = static_cast<Color::Background>(B); Print();
    }
    /// Moves the button on the console by dx and dy units.
    void Move(SHORT dx, SHORT dy)
    {
        Unlock(); Clear(); Pos.X += dx; if((Pos.X+Size.X+1)>=Console::CSize().X) Pos.X=0;
        Pos.Y+=dy; if((Pos.Y+Size.Y+1)>=Console::CSize().Y) Pos.Y=0; Print(); Lock();
    }
    /// Resizes the button on the console button by dx and dy units.
    void Resize(SHORT dx, SHORT dy)
    {
        Unlock(); Clear(); Size.X += dx; if((Pos.X+Size.X+1)>=Console::CSize().X) Size.X=Text.size()+2;
        while((Pos.X+Size.X+1)>=Console::CSize().X) { Pos.X--; } Size.Y+=dy; if((Pos.Y+Size.Y+1)>=Console::CSize().Y) Size.Y=1;
        if(Size.Y%2==0) {Size.Y++;} if(static_cast<unsigned>(Size.X%2)!=(Text.size()%2)){Size.X++;}
        if(static_cast<unsigned>(Size.X)<Text.size()) {Size.X = Text.size()+2;} Print(); Lock();
    }
    /// Changes data of the button on the console.
    void Data(std::string res)
    {
        Unlock(); Clear(); if(res.size()>Text.size()) { Resize((res.size()-Text.size()),0); Text = res; Clear();
                           if(static_cast<unsigned>(Size.X%2)!=(Text.size()%2)){Size.X++;} Print(); }
        else { Text = res; if(static_cast<unsigned>(Size.X%2)!=(Text.size()%2)){Size.X++;} Print(); } Lock();
    }
};

/// Static Button Class : For Immutable Buttons
class Static_Button : public BUTTON_TYPE
{
public:
    /// Constructor for the Static (Immutable) Button
    Static_Button(std::string data="", SHORT X=0, SHORT Y=0, SHORT W=1, SHORT H=1, Color::Foreground Bord_FC=BLACK,
           Color::Foreground FC=BLACK, Color::Background Bord_BC=BG_BLACK, Color::Background BC=BG_BLACK) :
    BUTTON_TYPE(data,X,Y,W,H,Bord_FC,FC,Bord_BC,BC)
    { if(static_cast<unsigned>(Size.X%2)!=(Text.size()%2)){Size.X++;} if(Size.Y%2==0) {Size.Y++;} Lock(); }
};

/// Radio Button Class : For Character Sized Buttons
class Radio_Button : public BUTTON_TYPE
{
    bool chckd;
public:
    /// Constructor for the Radio_Button
    Radio_Button(SHORT X=0, SHORT Y=0, Color::Foreground Bord_FC=BLACK, Color::Foreground FC=BLACK,
                 Color::Background Bord_BC=BG_BLACK, Color::Background BC=BG_BLACK) :
    BUTTON_TYPE(" ",X,Y,1,1,Bord_FC,FC,Bord_BC,BC),chckd(false) { Size.X=1; Lock(); }
    /// Checks if the button is Checked or Not.
    bool Checked() { return chckd; }
    /// Changes color of button to 'Active' when called.
    void Check(bool state)
    {
        if(chckd!=state)
        {
            chckd = state; WORD F = static_cast<WORD>(FGCol), B = static_cast<WORD>(BGCol); B/=16; std::swap(F,B); B*=16;
            FGCol = static_cast<Color::Foreground>(F); BGCol = static_cast<Color::Background>(B); Print();
        }
    }
    /// Moves the button on the console by dx and dy units.
    void Move(SHORT dx, SHORT dy)
    {
        Unlock(); Clear(); Pos.X += dx; if((Pos.X+Size.X+1)>=Console::CSize().X) Pos.X=0;
        Pos.Y+=dy; if((Pos.Y+Size.Y+1)>=Console::CSize().Y) Pos.Y=0; Print(); Lock();
    }
};

/// Standard Image Button Class : For Buttons containing AFI (ASCII Formatted) Images
class Image_Button : public BUTTON_TYPE
{
    std::string ImagePath; COORD ImageOffset; std::vector<std::vector<CHAR_INFO>> Image; SHORT TextOffset;
public:
    /// Constructor for the Image Button.
    Image_Button(std::string data="", std::string path="", SHORT X=0, SHORT Y=0, SHORT W=1, SHORT H=1, SHORT DX = 0,
                 SHORT DY = 0, SHORT TY = 0, Color::Foreground Bord_FC=BLACK, Color::Foreground FC=BLACK,
                 Color::Background Bord_BC=BG_BLACK, Color::Background BC=BG_BLACK) :
    BUTTON_TYPE(data,X,Y,W,H,Bord_FC,FC,Bord_BC,BC), ImagePath(path), TextOffset(TY)
    {
        ImageOffset = {DX,DY}; std::fstream fin; fin.open(ImagePath,std::ios::in|std::ios::binary);
        if(fin.is_open())
        {
            int SX, SY; fin.read(reinterpret_cast<char*>(&SX),sizeof(int)); fin.read(reinterpret_cast<char*>(&SY),sizeof(int));
            std::vector<CHAR_INFO> tmp; CHAR_INFO c;
            for(int i=0;i<SY;i++)
            {
                for(int j=0;j<SX;j++) { fin.read(reinterpret_cast<char*>(&c),sizeof(CHAR_INFO)); tmp.push_back(c); }
                Image.push_back(tmp); tmp.clear();
            }
            fin.close();
            if(!Text.empty())
            {
                if((Image.size()+4)>static_cast<unsigned>(Size.Y)) Size.Y = Image.size()+4;
                if(static_cast<unsigned>(Size.Y%2)!=Image.size()%2) {Size.Y++;}
                if(Image[0].size()>Text.size())
                {
                    if((Image[0].size()+2)>static_cast<unsigned>(Size.X)) Size.X = Image[0].size()+2;
                    if(Image[0].size()%2!=static_cast<unsigned>(Size.X%2)) {Size.X++;}
                    if(Text.size()%2!=static_cast<unsigned>(Size.X%2)) Text += " ";
                }
                else
                {
                    if(static_cast<unsigned>(Size.X%2)!=(Text.size()%2)) {Size.X++;}
                    if(Image[0].size()%2!=static_cast<unsigned>(Size.X%2)) {Size.X++; Text+=" ";}
                }
                if(TextOffset>(Size.Y/2)||TextOffset<=0) TextOffset = (Size.Y/2)-1;
            }
            else
            {
                if((Image.size()+2)>static_cast<unsigned>(Size.Y)) Size.Y = Image.size()+2;
                if(static_cast<unsigned>(Size.Y%2)!=Image.size()%2) {Size.Y++;}
                if((Image[0].size()+2)>static_cast<unsigned>(Size.X)) Size.X = Image[0].size()+2;
                if(Image[0].size()%2!=static_cast<unsigned>(Size.X%2)) {Size.X++;} TextOffset = 0;
            }
        }
        else {throw std::runtime_error(">> Image_Button : No Image Detected!");} Lock();
    }
    /// Assignment Operator for the Image Button.
    Image_Button& operator=(const Image_Button& b)
    {
        if(this==&b) return *this;
        else
        {
            this->Text = b.Text.data(); this->Pos = b.Pos; this->Size = b.Size; this->FGCol = b.FGCol; this->BGCol = b.BGCol;
            this->Border_FGCol = b.Border_FGCol; this->Border_BGCol = b.Border_BGCol; this->ImagePath = b.ImagePath;
            this->Image = b.Image; this->ImageOffset = b.ImageOffset; this->TextOffset = b.TextOffset; return *this;
        }
    }
    /// Prints the Image Button on the Console Window.
    void Print()
    {
        CONSOLE_SCREEN_BUFFER_INFO sbi; GetConsoleScreenBufferInfo(Console::Out(),&sbi);
        SHORT TextLen = Text.size(), i=1, WS = Size.X - TextLen; std::string LineSpc, SuppSpc;
        CHAR_INFO RawImg[Image[0].size()*Image.size()];
        for(size_t i=0;i<Image.size();i++) for(size_t j=0;j<Image[0].size();j++) RawImg[(Image[0].size()*i)+j] = Image[i][j];
        SHORT SX = ImageOffset.X + (Size.X-Image[0].size())/2 + Pos.X + 1;
        if(SX<=0||SX>=(Pos.X+Size.X)) {SX = Pos.X + 1 + ImageOffset.X;}  while(SX>=Pos.X+Size.X) SX--;
        SHORT SY = ImageOffset.Y + (Size.Y-Image.size())/2 + Pos.Y + (Text.empty()?1:0);
        if(SY<=0||SY>=(Pos.Y+Size.Y)) {SX = Pos.Y + 1 + ImageOffset.Y;}  while(SY>=Pos.Y+Size.Y) SY--;
        Console::Color(Border_FGCol,Border_BGCol); GOTO(Pos.X,Pos.Y);
        std::cout<<ULB; for(int i=0;i<Size.X;i++) std::cout<<HT; std::cout<<URB;
        for(int i=0;i<(WS/2);i++) {SuppSpc+=" ";} for(int i=0;i<TextLen;i++) LineSpc+=" ";
        std::string TempText = SuppSpc+Text+SuppSpc; LineSpc+=(SuppSpc+SuppSpc);
        for(;i<=((Size.Y-1)/2);i++)
        {
            GOTO(Pos.X,Pos.Y+i); Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
            Console::Color(FGCol,BGCol); std::cout<<LineSpc;
            Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
        }
        GOTO(Pos.X,Pos.Y+i); Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
        Console::Color(FGCol,BGCol); std::cout<<LineSpc; i++;
        Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
        for(;i<=Size.Y;i++)
        {
            GOTO(Pos.X,Pos.Y+i); Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
            Console::Color(FGCol,BGCol); std::cout<<LineSpc;
            Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
        }
        if(Text.size()>0) {GOTO(Pos.X+1,Pos.Y+(Size.Y/2)+TextOffset); Console::Color(FGCol, BGCol); std::cout<<TempText; }
        COORD Buf = {SHORT(Image[0].size()),SHORT(Image.size())}, CP = {0,0};
        SMALL_RECT Ar = {SX,SY,SHORT(SX+Image[0].size()),SHORT(SY+Image.size())};
        WriteConsoleOutput(Console::Out(),RawImg,Buf,CP,&Ar); Console::Color(Border_FGCol,Border_BGCol);
        GOTO(Pos.X,Pos.Y+Size.Y+1); std::cout<<DLB; for(int i=0;i<Size.X;i++) std::cout<<HT; std::cout<<DRB;
        SetConsoleTextAttribute(Console::Out(),sbi.wAttributes);
    }
    /// Moves the button on the console by dx and dy units.
    void Move(SHORT dx, SHORT dy)
    {
        Unlock(); Clear(); Pos.X += dx; if((Pos.X+Size.X+1)>=Console::CSize().X) Pos.X=0;
        Pos.Y+=dy; if((Pos.Y+Size.Y+1)>=Console::CSize().Y) Pos.Y=0; Print(); Lock();
    }
    /// Resizes the button on the console by dx and dy units.
    void Resize(SHORT dx, SHORT dy)
    {
        if(Locked()) {Unlock();} Clear(); Size.X+=dx; Size.Y+=dy;
        if((Image.size()+4)>static_cast<unsigned>(Size.Y)) Size.Y = Image.size()+4;
        if(static_cast<unsigned>(Size.Y%2)!=Image.size()%2) {Size.Y++;}
        if(Image[0].size()>Text.size())
        {
            if((Image[0].size()+2)>static_cast<unsigned>(Size.X)) Size.X = Image[0].size()+2;
            if(Image[0].size()%2!=static_cast<unsigned>(Size.X%2)) {Size.X++;}
            if(Text.size()%2!=static_cast<unsigned>(Size.X%2)) Text += " ";
        }
        else
        {
            if(static_cast<unsigned>(Size.X)<(Text.size()+2)) {Size.X = Text.size()+2;}
            if(static_cast<unsigned>(Size.X%2)!=(Text.size()%2)) {Size.X++;}
            if(Image[0].size()%2!=static_cast<unsigned>(Size.X%2)) {Size.X++; Text+=" ";}
        }
        if(TextOffset>(Size.Y/2)||TextOffset<=0) TextOffset = (Size.Y/2)-1;
        if((Pos.X+Size.X+1)>=Console::CSize().X) {Pos.X = 0;} if((Pos.Y+Size.Y+1)>=Console::CSize().Y) {Pos.Y=0;}
        if(!Locked()) {Lock();} Print();
    }
    /// Reloads a new Image for the button, and allows changing the Text Offset relative to the button.
    void Reload(std::string Path, SHORT TY=-1)
    {
        if(TY>-1) TextOffset = TY;
        if(Path!=ImagePath||!Path.empty()) ImagePath = Path; else {return;}
        std::fstream fin; fin.open(ImagePath,std::ios::in|std::ios::binary);
        if(fin.is_open())
        {
            Unlock(); Clear();
            int SX, SY; fin.read(reinterpret_cast<char*>(&SX),sizeof(int)); fin.read(reinterpret_cast<char*>(&SY),sizeof(int));
            std::vector<CHAR_INFO> tmp; CHAR_INFO c; Image.clear();
            for(int i=0;i<SY;i++)
            {
                for(int j=0;j<SX;j++) { fin.read(reinterpret_cast<char*>(&c),sizeof(CHAR_INFO)); tmp.push_back(c); }
                Image.push_back(tmp); tmp.clear();
            }
            fin.close();
            if(!Text.empty())
            {
                if((Image.size()+4)>static_cast<unsigned>(Size.Y)) Size.Y = Image.size()+4;
                if(static_cast<unsigned>(Size.Y%2)!=Image.size()%2 && (Image.size()>static_cast<unsigned>(Size.Y-4))) {Size.Y++;}
                if(Image[0].size()>Text.size())
                {
                    if((Image[0].size()+2)>static_cast<unsigned>(Size.X)) Size.X = Image[0].size()+2;
                    if(Image[0].size()%2!=static_cast<unsigned>(Size.X%2)) {Size.X++;}
                    if(Text.size()%2!=static_cast<unsigned>(Size.X%2)) Text += " ";
                }
                else
                {
                    if(static_cast<unsigned>(Size.X%2)!=(Text.size()%2)) {Size.X++;}
                    if(Image[0].size()%2!=static_cast<unsigned>(Size.X%2)) {Size.X++; Text+=" ";}
                }
                if(TextOffset>(Size.Y/2)||TextOffset<=0) TextOffset = (Size.Y/2)-1;
            }
            else
            {
                if((Image.size()+2)>static_cast<unsigned>(Size.Y)) Size.Y = Image.size()+2;
                if(static_cast<unsigned>(Size.Y%2)!=Image.size()%2 && (Image.size()>static_cast<unsigned>(Size.Y-4))) {Size.Y++;}
                if((Image[0].size()+2)>static_cast<unsigned>(Size.X)) Size.X = Image[0].size()+2;
                if(Image[0].size()%2!=static_cast<unsigned>(Size.X%2)) {Size.X++;} TextOffset = 0;
            }
            Lock(); Print();
        }
    }
    /// Changes data of the button on the console.
    void Data(std::string res = "")
    {
        Unlock(); if(res.empty()) { Clear(); Size.Y-=2; Text=res; }
        else if(res.size()>Image[0].size()&&res.size()>static_cast<unsigned>(Size.X-2))
        {
            Clear(); if(Image[0].size()%2!=(res.size()%2)) {res+=" ";} Resize(res.size()-
            (Image[0].size()>static_cast<unsigned>(Size.X-2)?Image[0].size():Size.X-2),0); Text = res;
        }
        else { Text = res; if(static_cast<unsigned>(Size.X%2)!=(Text.size()%2)) {Text+=" ";} } Print(); Lock();
    }
};

/// Static Image Button Class : For Immutable Image Buttons
class Static_Image_Button : public BUTTON_TYPE
{
    std::string ImagePath; COORD ImageOffset; std::vector<std::vector<CHAR_INFO>> Image; SHORT TextOffset;
public:
    /// Constructor for the Static (Immutable) Image Button
    Static_Image_Button(std::string data="", std::string path="", SHORT X=0, SHORT Y=0, SHORT W=1, SHORT H=1, SHORT DX = 0,
                 SHORT DY = 0, SHORT TY = 0, Color::Foreground Bord_FC=BLACK, Color::Foreground FC=BLACK,
                 Color::Background Bord_BC=BG_BLACK, Color::Background BC=BG_BLACK) :
    BUTTON_TYPE(data,X,Y,W,H,Bord_FC,FC,Bord_BC,BC), ImagePath(path), TextOffset(TY)
    {
        ImageOffset = {DX,DY}; std::fstream fin; fin.open(ImagePath,std::ios::in|std::ios::binary);
        if(fin.is_open())
        {
            int SX, SY; fin.read(reinterpret_cast<char*>(&SX),sizeof(int)); fin.read(reinterpret_cast<char*>(&SY),sizeof(int));
            std::vector<CHAR_INFO> tmp; CHAR_INFO c;
            for(int i=0;i<SY;i++)
            {
                for(int j=0;j<SX;j++) { fin.read(reinterpret_cast<char*>(&c),sizeof(CHAR_INFO)); tmp.push_back(c); }
                Image.push_back(tmp); tmp.clear();
            }
            fin.close();
            if(!Text.empty())
            {
                if((Image.size()+4)>static_cast<unsigned>(Size.Y)) Size.Y = Image.size()+4;
                if(static_cast<unsigned>(Size.Y%2)!=Image.size()%2) {Size.Y++;}
                if(Image[0].size()>Text.size())
                {
                    if((Image[0].size()+2)>static_cast<unsigned>(Size.X)) Size.X = Image[0].size()+2;
                    if(Image[0].size()%2!=static_cast<unsigned>(Size.X%2)) {Size.X++;}
                    if(Text.size()%2!=static_cast<unsigned>(Size.X%2)) Text += " ";
                }
                else
                {
                    if(static_cast<unsigned>(Size.X%2)!=(Text.size()%2)) {Size.X++;}
                    if(Image[0].size()%2!=static_cast<unsigned>(Size.X%2)) {Size.X++; Text+=" ";}
                }
                if(TextOffset>(Size.Y/2)||TextOffset<=0) TextOffset = (Size.Y/2)-1;
            }
            else
            {
                if((Image.size()+2)>static_cast<unsigned>(Size.Y)) Size.Y = Image.size()+2;
                if(static_cast<unsigned>(Size.Y%2)!=Image.size()%2) {Size.Y++;}
                if((Image[0].size()+2)>static_cast<unsigned>(Size.X)) Size.X = Image[0].size()+2;
                if(Image[0].size()%2!=static_cast<unsigned>(Size.X%2)) {Size.X++;} TextOffset = 0;
            }
        }
        else {throw std::runtime_error(">> Static_Image_Button : No Image Detected!");} Lock();
    }
    void Print()
    {
        CONSOLE_SCREEN_BUFFER_INFO sbi; GetConsoleScreenBufferInfo(Console::Out(),&sbi);
        SHORT TextLen = Text.size(), i=1, WS = Size.X - TextLen; std::string LineSpc, SuppSpc;
        CHAR_INFO RawImg[Image[0].size()*Image.size()];
        for(size_t i=0;i<Image.size();i++) for(size_t j=0;j<Image[0].size();j++) RawImg[(Image[0].size()*i)+j] = Image[i][j];
        SHORT SX = ImageOffset.X + (Size.X-Image[0].size())/2 + Pos.X + 1;
        if(SX<=0||SX>=(Pos.X+Size.X)) {SX = Pos.X + 1 + ImageOffset.X;}  while(SX>=Pos.X+Size.X) SX--;
        SHORT SY = ImageOffset.Y + (Size.Y-Image.size())/2 + Pos.Y + (Text.empty()?1:0);
        if(SY<=0||SY>=(Pos.Y+Size.Y)) {SX = Pos.Y + 1 + ImageOffset.Y;}  while(SY>=Pos.Y+Size.Y) SY--;
        Console::Color(Border_FGCol,Border_BGCol); GOTO(Pos.X,Pos.Y);
        std::cout<<ULB; for(int i=0;i<Size.X;i++) std::cout<<HT; std::cout<<URB;
        for(int i=0;i<(WS/2);i++) {SuppSpc+=" ";} for(int i=0;i<TextLen;i++) LineSpc+=" ";
        std::string TempText = SuppSpc+Text+SuppSpc; LineSpc+=(SuppSpc+SuppSpc);
        for(;i<=((Size.Y-1)/2);i++)
        {
            GOTO(Pos.X,Pos.Y+i); Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
            Console::Color(FGCol,BGCol); std::cout<<LineSpc;
            Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
        }
        GOTO(Pos.X,Pos.Y+i); Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
        Console::Color(FGCol,BGCol); std::cout<<LineSpc; i++;
        Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
        for(;i<=Size.Y;i++)
        {
            GOTO(Pos.X,Pos.Y+i); Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
            Console::Color(FGCol,BGCol); std::cout<<LineSpc;
            Console::Color(Border_FGCol,Border_BGCol); std::cout<<VT;
        }
        if(Text.size()>0) {GOTO(Pos.X+1,Pos.Y+(Size.Y/2)+TextOffset); Console::Color(FGCol, BGCol); std::cout<<TempText; }
        COORD Buf = {SHORT(Image[0].size()),SHORT(Image.size())}, CP = {0,0};
        SMALL_RECT Ar = {SX,SY,SHORT(SX+Image[0].size()),SHORT(SY+Image.size())};
        WriteConsoleOutput(Console::Out(),RawImg,Buf,CP,&Ar); Console::Color(Border_FGCol,Border_BGCol);
        GOTO(Pos.X,Pos.Y+Size.Y+1); std::cout<<DLB; for(int i=0;i<Size.X;i++) std::cout<<HT; std::cout<<DRB;
        SetConsoleTextAttribute(Console::Out(),sbi.wAttributes);
    }
};

/// Box Class : For Creating Cells for use as Input Boxes, Table Elements and Form Inputs.
class Box
{
    SHORT X, Y, Size, RdCur; std::string cache; size_t IDX; bool lckd;
    /// Reserves space for the Box on the Console Window.
    void Lock() { IDX = Console::Reserve(X,X+Size+2,Y,Y+2); lckd = true;}
    /// Release space for the Box on the Console Window.
    void Unlock() { Console::Release(IDX); lckd = false; }
public:
    enum class Row    : SHORT { Nil=0, Start, Mid, End };
    enum class Column : SHORT { Nil=0, Start, Mid, End };
    enum class Type   : SHORT { Normal=0, Password, Restricted, Invisible };
    enum class Limit  : SHORT { Nil=0, Alphabet, Number, AlphaNumeric, Custom };
    /// Checks if Space for UI Element is reserved on the Console Window.
    bool Locked() { return lckd;}
    /// Generates a Box for use in Input Field in Forms, Cells in Tables, etc.
    Box(SHORT dx=0, SHORT dy=0, SHORT sz=1, Row RT=Row::Nil, Column CT=Column::Nil) :
        X(dx), Y(dy), Size(sz), RdCur(X+2)
    {
        char LUC,RUC,LLC,RLC;
        if      ( RT==Row::Start && CT==Column::Start ) { LUC=ULB; LLC=LC ; RUC=UC ; RLC=NC ; }
        else if ( RT==Row::Start && CT==Column::Mid   ) { LUC=LC ; LLC=LC ; RUC=NC ; RLC=NC ; }
        else if ( RT==Row::Start && CT==Column::End   ) { LUC=LC ; LLC=DLB; RUC=NC ; RLC=DC ; }
        else if ( RT==Row::Start && CT==Column::Nil   ) { LUC=ULB; LLC=DLB; RUC=UC ; RLC=DC ; }
        else if ( RT==Row::Mid   && CT==Column::Start ) { LUC=UC ; LLC=NC ; RUC=UC ; RLC=NC ; }
        else if ( RT==Row::Mid   && CT==Column::Mid   ) { LUC=NC ; LLC=NC ; RUC=NC ; RLC=NC ; }
        else if ( RT==Row::Mid   && CT==Column::End   ) { LUC=NC ; LLC=DC ; RUC=NC ; RLC=DC ; }
        else if ( RT==Row::Mid   && CT==Column::Nil   ) { LUC=UC ; LLC=DC ; RUC=UC ; RLC=DC ; }
        else if ( RT==Row::End   && CT==Column::Start ) { LUC=UC ; LLC=NC ; RUC=URB; RLC=RC ; }
        else if ( RT==Row::End   && CT==Column::Mid   ) { LUC=NC ; LLC=NC ; RUC=RC ; RLC=RC ; }
        else if ( RT==Row::End   && CT==Column::End   ) { LUC=NC ; LLC=DC ; RUC=RC ; RLC=DRB; }
        else if ( RT==Row::End   && CT==Column::Nil   ) { LUC=UC ; LLC=DC ; RUC=URB; RLC=DRB; }
        else if ( RT==Row::Nil   && CT==Column::Start ) { LUC=ULB; LLC=LC ; RUC=URB; RLC=RC ; }
        else if ( RT==Row::Nil   && CT==Column::Mid   ) { LUC=LC ; LLC=LC ; RUC=RC ; RLC=RC ; }
        else if ( RT==Row::Nil   && CT==Column::End   ) { LUC=LC ; LLC=DLB; RUC=RC ; RLC=DRB; }
        else                                            { LUC=ULB; LLC=DLB; RUC=URB; RLC=DRB; }
        GOTO(X,Y); std::cout<<LUC; for(int i=0;i<Size;i++) std::cout<<HT; std::cout<<RUC; GOTO(X,Y+1); std::cout<<VT;
        for(int i=0;i<Size;i++) {std::cout<<" ";} std::cout<<VT; GOTO(X,Y+2); std::cout<<LLC; for(int i=0;i<Size;i++)
        {std::cout<<HT;} std::cout<<RLC; Lock();
    }
    /// Copy Constructor for the Box Type
    Box(const Box& B2) { X=B2.X; Y=B2.Y; Size=B2.Size; RdCur=B2.RdCur; cache=B2.cache; IDX=B2.IDX; lckd=B2.lckd; }
    /// Reads user submitted data to the Box, and allows it to be saved later.
    COORD Read(Type InputType=Type::Normal, Limit InputCond=Limit::Nil, std::regex Fmt = std::regex(""))
    {
        GOTO(RdCur,Y+1); COORD DR; INPUT_RECORD rec; DWORD ev; Console::Cursor(1); CONSOLE_SCREEN_BUFFER_INFO ci;
        bool (*is_per_cond)(char, const std::locale&);
        if     (InputCond == Limit::Alphabet) is_per_cond = std::isalpha;
        else if(InputCond == Limit::Number) is_per_cond = std::isdigit;
        else if(InputCond == Limit::AlphaNumeric) is_per_cond = std::isalnum;
        while(true)
        {
            ReadConsoleInput(Console::In(),&rec,1,&ev); GetConsoleScreenBufferInfo(Console::Out(),&ci);
            if(rec.EventType==KEY_EVENT)
            {
                if(rec.Event.KeyEvent.bKeyDown)
                {
                    if(rec.Event.KeyEvent.wVirtualKeyCode==VK_BACK && (RdCur)>X+1)
                    {
                        if(cache.size()>1) { cache.resize(cache.size()-1); GOTO(RdCur-1,Y+1);
                        std::cout<<" "; GOTO(RdCur-1,Y+1); RdCur--; } else {cache=""; Fill("");}
                    }
                    else if(rec.Event.KeyEvent.wVirtualKeyCode==VK_RETURN)
                    {
                        DR = {-1,-1};
                        if(InputType == Type::Restricted && InputCond == Limit::Custom)
                        {
                            if(regex_match(cache,Fmt)) return DR;
                            else {cache=""; RdCur = X+2; Fill("");}
                        }
                        else return DR;
                    }
                    else if(rec.Event.KeyEvent.uChar.AsciiChar!=0)
                    {
                        if(RdCur<(X+Size))
                        {
                            if(InputType == Type::Normal && std::isprint(rec.Event.KeyEvent.uChar.AsciiChar))
                            {
                                std::cout<<rec.Event.KeyEvent.uChar.AsciiChar;
                                cache += rec.Event.KeyEvent.uChar.AsciiChar; RdCur++;
                            }
                            else if(InputType == Type::Password && std::isprint(rec.Event.KeyEvent.uChar.AsciiChar))
                            { std::cout<<'*'; cache += rec.Event.KeyEvent.uChar.AsciiChar; RdCur++; }
                            else if(InputType == Type::Restricted)
                            {
                                if(InputCond != Limit::Custom)
                                {
                                    if(is_per_cond(rec.Event.KeyEvent.uChar.AsciiChar,std::cout.getloc()))
                                    {
                                        std::cout<<rec.Event.KeyEvent.uChar.AsciiChar;
                                        cache += rec.Event.KeyEvent.uChar.AsciiChar; RdCur++;
                                    }
                                }
                                else
                                {
                                    std::cout<<rec.Event.KeyEvent.uChar.AsciiChar;
                                    cache += rec.Event.KeyEvent.uChar.AsciiChar; RdCur++;
                                }
                            }
                            else { std::cout<<' '; cache += rec.Event.KeyEvent.uChar.AsciiChar; RdCur++; }
                        }
                    }
                }
            }
            else if(rec.EventType==MOUSE_EVENT)
            {
                DR = rec.Event.MouseEvent.dwMousePosition;
                if(rec.Event.MouseEvent.dwButtonState==FROM_LEFT_1ST_BUTTON_PRESSED && (!IsClicked(DR))
                   && rec.Event.MouseEvent.dwEventFlags==0)
                {
                    if(InputType == Type::Restricted && InputCond == Limit::Custom)
                    {
                        if(regex_match(cache,Fmt)) return DR;
                        else {cache=""; RdCur = X+2; Fill("");}
                    }
                    else return DR;
                }
            }
        }
    }
    /// Clears the box, and deletes all the saved memory.
    void Clear() noexcept { GOTO(X+1,Y+1); for(int i=0;i<Size;i++) std::cout<<" "; GOTO(X+2,Y+1); cache = ""; RdCur = X+2; }
    /// Fills the box with a string, which may be added to lated during read.
    void Fill(std::string_view str)
    {
        Clear(); if(str.size()>unsigned(Size-2)) throw std::out_of_range(">> Box : Overflow!");
        GOTO(X+2,Y+1); std::cout<<str; cache = str; RdCur = X+2+cache.size();
    }
    /// Checks if the Box was clicked, which may be used to select the Box to read data to.
    bool IsClicked(COORD C) const noexcept { return (C.X>=(X+1)&&C.X<=(X+Size))&&(C.Y==(Y+1)); }
    /// Returns the current data in the box.
    std::string Retrieve() const noexcept { return cache; }
};

#endif // UI_ELEMENT_HPP
