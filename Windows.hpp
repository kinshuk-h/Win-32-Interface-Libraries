#ifndef WINDOWS_HPP_INCLUDED
#define WINDOWS_HPP_INCLUDED

#include <windows.h>
#include <stdexcept>
#include <shlobj.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>

#define GOTO Console::CursorAt

/// Basic Colors - FOREGROUND

#define RED            Color::Foreground::Red
#define BROWN          Color::Foreground::Brown
#define GREEN          Color::Foreground::Green
#define DARK_GREEN     Color::Foreground::Dark_Green
#define BLUE           Color::Foreground::Blue
#define DARK_BLUE      Color::Foreground::Dark_Blue
#define YELLOW         Color::Foreground::Yellow
#define TURQUOISE      Color::Foreground::Turquoise
#define DARK_TURQUOISE Color::Foreground::Dark_Turquoise
#define MAGENTA        Color::Foreground::Magenta
#define PURPLE         Color::Foreground::Purple
#define OLIVE          Color::Foreground::Olive
#define WHITE          Color::Foreground::White
#define GRAY           Color::Foreground::Gray
#define DARK_GRAY      Color::Foreground::Dark_Gray
#define BLACK          Color::Foreground::Black

/// Basic Colors - BACKGROUND

#define BG_RED            Color::Background::Red
#define BG_GREEN          Color::Background::Green
#define BG_DARK_GREEN     Color::Background::Dark_Green
#define BG_BLUE           Color::Background::Blue
#define BG_DARK_BLUE      Color::Background::Dark_Blue
#define BG_YELLOW         Color::Background::Yellow
#define BG_TURQUOISE      Color::Background::Turquoise
#define BG_DARK_TURQUOISE Color::Background::Dark_Turquoise
#define BG_MAGENTA        Color::Background::Magenta
#define BG_PURPLE         Color::Background::Purple
#define BG_OLIVE          Color::Background::Olive
#define BG_WHITE          Color::Background::White
#define BG_GRAY           Color::Background::Gray
#define BG_DARK_GRAY      Color::Background::Dark_Gray
#define BG_BLACK          Color::Background::Black

/// Text Attribute Color Enumerations
namespace Color
{
  enum class Foreground : WORD { Black=0, Dark_Blue, Dark_Green, Dark_Turquoise, Brown, Purple, Olive,
                                 Gray, Dark_Gray, Blue, Green, Turquoise, Red, Magenta, Yellow, White };
  enum class Background : WORD { Black=0, Dark_Blue=16, Dark_Green=32, Dark_Turquoise=48, Brown=64, Purple=80, Olive=96, Gray=112,
                                 Dark_Gray=128, Blue=144, Green=160, Turquoise=176, Red=192, Magenta=208, Yellow=224, White=240 };
};

/// Enumeration for Executable Path Type ( Direct Path / Path from Environment Variable )
enum class EXEType : bool { Direct = false, Environment = true };

///Border Items
const unsigned char DC = 0xCA, UC = 0xCB, ULB = 201, DLB = 200, URB = 187, DRB = 188, HT = 205,
                    VT = 186, LC = 0xCC, RC = 0xB9, NC = 0xCE;

class Console
{
    friend class BUTTON_TYPE;
    friend class Box;
    static HANDLE in, out; static HWND CsWnd;
    static CONSOLE_CURSOR_INFO cursorInfo;
    static CONSOLE_SCREEN_BUFFER_INFOEX csbix;
    static RECT Pos; static bool ictr; static DWORD state;
    static std::map<size_t, SMALL_RECT> Cur_UI_Elm;
    Console(){}
    /// Allows UI Element Classes to reserve space for objects on the Console Window.
    static size_t Reserve(SHORT X1, SHORT Y1, SHORT X2, SHORT Y2)
    { SMALL_RECT R = {X1,Y1,X2,Y2}; Cur_UI_Elm.insert(std::make_pair(Cur_UI_Elm.size(),R)); return Cur_UI_Elm.size()-1; }
    /// Releases released space for use on the console.
    static void Release(size_t IDX) { Cur_UI_Elm.erase(IDX); }
public:
    /// Returns the Input Handle
    static HANDLE In()  { return in;  }
    /// Returns the Output Handle
    static HANDLE Out() { return out; }
    /// Return the current areas which are reserved on the console.
    static std::map<size_t,SMALL_RECT> Reserved() { return Cur_UI_Elm; }
    /// Initializes I/O Handles and records initial attributes.
    static void Init()
    {
        if(ictr) {throw std::logic_error(">> Console : Already Initialized!");} csbix.cbSize = sizeof(csbix);
        in  = GetStdHandle(STD_INPUT_HANDLE ); out = GetStdHandle(STD_OUTPUT_HANDLE);
        if(in==INVALID_HANDLE_VALUE||out==INVALID_HANDLE_VALUE)
            throw std::logic_error(">> Console : Unable to retrieve I/O Handles");
        CsWnd = GetConsoleWindow(); if(CsWnd==0) throw std::logic_error(">> Console : Could not retrieve Window Handle!");
        if(!GetConsoleScreenBufferInfoEx(out,&csbix)) throw std::logic_error(">> Console : Could not Initiate CSBI EX!");
        if(!GetConsoleCursorInfo(out,&cursorInfo)) throw std::logic_error(">> Console : Could not Initiate CursorInfo!");
        if(!GetWindowRect(CsWnd, &Pos)) throw std::logic_error(">> Console : Could not Initiate Window Size!");
        if(!GetConsoleMode(in, &state)) {throw std::logic_error(">> Console : Could not Initiate ConsoleMode!");} ictr=1;
        csbix.ColorTable[0] = RGB(0,0,0); csbix.ColorTable[15] = RGB(255,255,255);
        SetConsoleTextAttribute(out,WORD(WHITE)); SetConsoleCursorPosition(out,{0,0});
        SMALL_RECT s = {10,10,89,34}; SetConsoleWindowInfo(out,TRUE,&s); csbix.srWindow = s; csbix.wAttributes = WORD(WHITE);
        MoveWindow(CsWnd,10,10,80,25,FALSE); SetConsoleScreenBufferSize(out,{80,100}); csbix.dwCursorPosition = {0,0};
        csbix.dwSize = {80,25}; SetConsoleScreenBufferInfoEx(out,&csbix);
    }
    /// Returns the Current Console Window Size
    static COORD CSize()
    {
        CONSOLE_SCREEN_BUFFER_INFOEX cbi; cbi.cbSize = sizeof(cbi); GetConsoleScreenBufferInfoEx(out,&cbi);
        SHORT Width = cbi.srWindow.Right-cbi.srWindow.Left+1, Height = cbi.srWindow.Bottom-cbi.srWindow.Top+1;
        COORD R = {Width,Height}; return R;
    }
    /// Saves Current Settings as Original Settings.
    static void Save()
    {
        if(!ictr) {throw std::logic_error(">> Console : Uninitialized!");} csbix.cbSize = sizeof(csbix);
        if(!GetConsoleScreenBufferInfoEx(out,&csbix)) throw std::logic_error(">> Console : Could not Save CSBI EX!");
        if(!GetConsoleCursorInfo(out,&cursorInfo)) throw std::logic_error(">> Console : Could not Save CursorInfo!");
        if(!GetWindowRect(CsWnd, &Pos)) throw std::logic_error(">> Console : Could not Save Window Size!");
        if(!GetConsoleMode(in,&state)) throw std::logic_error(">> Console : Could not Save ConsoleMode!");
    }
    /// Resets all attributes to original configuration.
    static void To_Original()
    {
        if(!ictr) throw std::logic_error(">> Console : Uninitialized");
        SetConsoleTextAttribute(out,csbix.wAttributes); SetConsoleCursorPosition(out,csbix.dwCursorPosition);
        MoveWindow(CsWnd,Pos.left,Pos.top,csbix.srWindow.Right-csbix.srWindow.Left,csbix.srWindow.Bottom-csbix.srWindow.Top,FALSE);
        SetConsoleScreenBufferSize(out,csbix.dwSize); SetConsoleWindowInfo(out,TRUE,&csbix.srWindow);
        SetConsoleScreenBufferInfoEx(out,&csbix);
    }
    /// Sets the color (foreground and background) for text attributes of the console.
    static void Color(Color::Foreground Fore, Color::Background Back=Color::Background::Black)
    { if(!ictr) throw std::logic_error(">> Console : Uninitialized"); SetConsoleTextAttribute(out,WORD(Fore)|WORD(Back)); }
    /// Sets the color (foreground and background) of the console in general. All colors on the screen revert to these values.
    static void OverallColor(COLORREF Forg, COLORREF Bacg)
    {
        if(!ictr) throw std::logic_error(">> Console : Uninitialized");
        CONSOLE_SCREEN_BUFFER_INFOEX cb; cb.cbSize = sizeof(cb); GetConsoleScreenBufferInfoEx(out,&cb);
        cb.ColorTable[15] = Forg; cb.ColorTable[0] = Bacg; SetConsoleScreenBufferInfoEx(out,&cb);
    }
    /// Sets the size of the console window. ( In character units or no. of characters )
    static void Size(SHORT W, SHORT H)
    {
        if(!ictr) throw std::logic_error(">> Console : Uninitialized");
        COORD BufferCoord = {200,200}; SetConsoleScreenBufferSize(out,BufferCoord);
        SMALL_RECT SIZE = {0,0,SHORT(W-1),SHORT(H-1)}; SetConsoleWindowInfo(out,TRUE,&SIZE);
        BufferCoord = {W,SHORT(2*H)}; SetConsoleScreenBufferSize(out,BufferCoord);
    }
    /// Sets the position of the console window. ( In pixel units relative to the screen )
    static void Position(LONG LeftPos, LONG TopPos)
    {
        if(!ictr) throw std::logic_error(">> Console : Uninitialized");
        CONSOLE_SCREEN_BUFFER_INFOEX c; c.cbSize = sizeof(c); GetConsoleScreenBufferInfoEx(out,&c);
        MoveWindow(CsWnd,LeftPos,TopPos,c.srWindow.Right-c.srWindow.Left,c.srWindow.Bottom-c.srWindow.Top,FALSE);
        Size(c.srWindow.Right-c.srWindow.Left+1,c.srWindow.Bottom-c.srWindow.Top+1);
    }
    /// Sets the size of the buffer, or the extra space available which requires scrolling.
    static void BufferSize(SHORT w, SHORT h)
    {
        if(!ictr) throw std::logic_error(">> Console : Uninitialized");
        COORD buf = {w,h}; SetConsoleScreenBufferSize(out,buf);
    }
    /// Sets the position of the console window to the center of the screen. ( In pixel units relative to the screen )
    static void Center()
    {
        if(!ictr) throw std::logic_error(">> Console : Uninitialized");
        CONSOLE_SCREEN_BUFFER_INFOEX c; c.cbSize = sizeof(c); GetConsoleScreenBufferInfoEx(out,&c);
        SHORT Width = c.srWindow.Right-c.srWindow.Left+1, Height = c.srWindow.Bottom-c.srWindow.Top+1;
        LONG X = 630 - (7*Width)/2, Y = 310 - (26*Height)/5;
        LONG X2 = (2240/3) - (40*Width)/9, Y2 = 456 - 8*Height;
        MoveWindow(CsWnd,(Width>130?X2:X),(Height>50?Y2:Y),Width,Height,FALSE); Size(Width,Height);
    }
    /// Moves Cursor on the console to an arbitrary location.
    static void CursorAt(SHORT X, SHORT Y)
    {
        if(!ictr) throw std::logic_error(">> Console : Uninitialized");
        COORD CPos = {X,Y}; SetConsoleCursorPosition(out,CPos);
    }
    /// Toggles support for Mouse Input on console.
    static void MouseInput(bool flag = true)
    {
        DWORD st, fmt = ENABLE_EXTENDED_FLAGS|ENABLE_WINDOW_INPUT|ENABLE_MOUSE_INPUT; GetConsoleMode(in,&st);
        bool enabled = ((st&fmt)==fmt); if(!enabled&&flag) SetConsoleMode(in,st|fmt);
        else if(enabled&&(!flag)) { SetConsoleMode(in,st&(~fmt)); }
    }
    /// Checks if current console configuration supports Mouse Input or not.
    static bool IsMouseInputEnabled()
    {
        DWORD fmt = ENABLE_EXTENDED_FLAGS|ENABLE_WINDOW_INPUT|ENABLE_MOUSE_INPUT;
        DWORD st; GetConsoleMode(in,&st); return (st&fmt)==fmt;
    }
    /// Executes a Particular executable with supplied parameters. You can also mention Environment Variables.
    static bool Execute(EXEType type, std::string executable, std::string_view parameters="",
                        std::string_view exe_directory = "", bool show=false)
    {
        SHELLEXECUTEINFO rSEI ={0}; rSEI.cbSize = sizeof( rSEI ); rSEI.lpVerb = "open";
        if(type == EXEType::Environment)
        {
            char temp[260]; GetEnvironmentVariable(executable.data(),temp,MAX_PATH);
            executable.assign(temp);
        }
        rSEI.lpFile = (!executable.empty()?executable.data():NULL);
        rSEI.lpParameters = (!parameters.empty()?parameters.data():NULL);
        rSEI.lpDirectory = (!exe_directory.empty()?exe_directory.data():NULL);
        rSEI.nShow = (show?SW_SHOW:SW_HIDE); rSEI.fMask = SEE_MASK_NOCLOSEPROCESS;
        ShellExecuteEx( &rSEI ); WaitForSingleObject(rSEI.hProcess,INFINITE); return CloseHandle(rSEI.hProcess);
    }
    /// Toggles Cursor Visbility ON or OFF on the console.
    static void Cursor(bool flag = true)
    {
        CONSOLE_CURSOR_INFO cci;
        GetConsoleCursorInfo(out, &cci);
        if(cci.bVisible!=flag)
        {
            cci.bVisible = flag;
            SetConsoleCursorInfo(out, &cci);
        }
    }
    /// Retrieves the complete path of the console application.
    static std::string Path()
    {
        HMODULE hModule = GetModuleHandleA(nullptr);
        TCHAR path[MAX_PATH]; GetModuleFileNameA(hModule, path, MAX_PATH);
        std::string a; a.assign(path); return a;
    }
    /// Retrieves the drive of the console application.
    static std::string Drive()
    {
        HMODULE hModule = GetModuleHandleA(nullptr);
        TCHAR path[MAX_PATH]; GetModuleFileNameA(hModule, path, MAX_PATH);
        std::string s; s+=path[0]; return s;
    }
    /// Draws a Secondary Border on the Console Window, to make it attractive.
    static void Border()
    {
        CONSOLE_SCREEN_BUFFER_INFOEX cs; cs.cbSize = sizeof(cs); GetConsoleScreenBufferInfoEx(out,&cs);
        SHORT W = cs.srWindow.Right-cs.srWindow.Left+1, H = cs.srWindow.Bottom-cs.srWindow.Top+1;
        GOTO(0,0); std::cout<<ULB; for(int i=1;i<(W-1);i++) std::cout<<HT; std::cout<<URB;
        GOTO(0,H-1); std::cout<<DLB; for(int i=1;i<(W-1);i++) std::cout<<HT; std::cout<<DRB;
        for(int i=1;i<H-1;i++) {GOTO(0,i);std::cout<<VT;} for(int i=1;i<H-1;i++) {GOTO(W-1,i);std::cout<<VT;} GOTO(1,1);
    }
    virtual ~Console(){}
};

HANDLE Console::in, Console::out; CONSOLE_CURSOR_INFO Console::cursorInfo; HWND Console::CsWnd; RECT Console::Pos;
bool Console::ictr=0; DWORD Console::state; std::map<size_t,SMALL_RECT> Console::Cur_UI_Elm;
CONSOLE_SCREEN_BUFFER_INFOEX Console::csbix;

/// Prints a std::string with a border around it, as a header.
void Head(std::ostream& obj, std::string_view name, char sym1='-',char sym2='|')
{
    for(SHORT i=1;i<=(Console::CSize().X);i++) obj<<sym1;
    if(name.size()<static_cast<unsigned>(Console::CSize().X))
    {
        obj<<sym2; for(size_t i=1;i<=((Console::CSize().X)-2-name.size())/2;i++) obj<<" "; if(name.size()%2==0) obj<<name;
        else {obj<<name<<" ";} for(size_t i=1;i<=((Console::CSize().X)-2-name.length())/2;i++) obj<<" "; obj<<sym2;
    }
    else { obj<<name; } for(SHORT i=1;i<=(Console::CSize().X);i++) obj<<sym1;
}

/// A special version of Head(), Prints a std::string with a Decorative border around it, as a header.
void SplHead(std::ostream& obj, std::string_view name)
{
    obj<<ULB; for(SHORT i=0;i<(Console::CSize().X)-2;i++) obj<<HT; obj<<URB;
    if(name.length()<static_cast<unsigned>((Console::CSize().X)-2))
    {
        obj<<VT; for(size_t i=1;i<=((Console::CSize().X)-2-name.length())/2;i++) obj<<" ";
        if(name.size()%2!=unsigned(Console::CSize().X%2)) std::cout<<name<<" "; else std::cout<<name;
        for(size_t i=1;i<=((Console::CSize().X)-2-name.length())/2;i++) {obj<<" ";} obj<<char(186);
    }
    else if(name.length()==static_cast<unsigned>((Console::CSize().X)-2)) { obj<<VT<<name<<VT; } else { obj<<name; }
    obj<<DLB; for(size_t i=0;i<static_cast<unsigned>((Console::CSize().X)-2);i++) obj<<HT; obj<<DRB;
}

/// Generates an Information Dialog Box, used to indicate Status/Completion/Requirement/etc.
template<typename fx> void InfoBox(std::string_view content, std::string_view title, fx f)
{
    UINT buttons = MB_OK; int res = MessageBox(nullptr,content.data(),title.data(),MB_ICONINFORMATION|buttons);
    if(res==IDOK) {f();} return;
}

/// Generates an Error Dialog Box, used to indicate Failure/Incompletion/Requirement/etc.
template<typename fx> void ErrorBox(std::string_view content, std::string_view title, fx f)
{
    UINT buttons = MB_OK; int res = MessageBox(nullptr,content.data(),title.data(),MB_ICONERROR|buttons);
    if(res==IDOK) {f();} return;
}

/// Generates an Question Dialog Box, used to confirm Choice/Decision/Change/Option/etc.
template<typename fx1, typename fx2> void QuestionBox(std::string_view content, std::string_view title, fx1 f1, fx2 f2)
{
    UINT buttons = MB_YESNO; int res = MessageBox(nullptr,content.data(),title.data(),MB_ICONQUESTION|buttons);
    if(res==IDYES) {f1();} else if(res==IDNO) {f2();} return;
}

/// Draws a Boundary local to the console, to mark a particular area.
void Boundary(size_t x, size_t y, size_t w, size_t h)
{
    GOTO(x-2,y-2); std::cout<<ULB; for(size_t i=x-1;i<=w;i++) std::cout<<HT; std::cout<<URB;
    GOTO(x-2,h+1); std::cout<<DLB; for(size_t i=x-1;i<=w;i++) std::cout<<HT; std::cout<<DRB;
    for(size_t i=y-1;i<=h;i++) {GOTO(x-2,i);std::cout<<VT;}
    for(size_t i=y-1;i<=h;i++) {GOTO(w-x+6,i);std::cout<<VT;}
}

/// Checks if a particular file or directory exists on the current system. Requires complete path.
bool Exist(std::string_view file)
{
    WIN32_FIND_DATA FData;
    HANDLE h = FindFirstFile(file.data(), &FData) ;
    bool found = h != INVALID_HANDLE_VALUE;
    if(found) { FindClose(h); } return found;
}

/**Opens a Dialog Box to allow the user to select a file. <br><br>
 The user can also alter the Dialog Box's Title and the Type of Files to select. <br>
 To select specific file(s) pass a std::string to 'type_filter' in the following form : <br> <br>
 @syntax: <pre>[File Type 1]NUL[*.extension 1]NUL...NUL[All Files]NUL[*.*]NUL</pre>*/
std::string GetFile(std::string_view title="Open", LPCSTR type_filter="All Files\0*.*\0\0")
{
    char filename[MAX_PATH]; OPENFILENAME ofn;
    ZeroMemory(&filename, sizeof(filename)); ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn); ofn.hwndOwner  = nullptr;  ofn.lpstrFilter = type_filter;
    ofn.lpstrFile   = filename;    ofn.nMaxFile   = MAX_PATH; ofn.lpstrTitle  = title.data();
    ofn.Flags       = OFN_ENABLESIZING | OFN_NOVALIDATE;
    if (GetOpenFileNameA(&ofn)) { return std::string(filename); }
    else
    {
       switch (CommDlgExtendedError())
       {case CDERR_DIALOGFAILURE   : throw std::logic_error(">> Open Dialog : Cannot create Dialog Box.        (DIALOGFAILURE  )");
        case CDERR_FINDRESFAILURE  : throw std::logic_error(">> Open Dialog : Cannot find Specified Resource.  (FINDRESFAILURE )");
        case CDERR_INITIALIZATION  : throw std::logic_error(">> Open Dialog : Cannot Initialize Properly.      (INITIALIZATION )");
        case CDERR_LOADRESFAILURE  : throw std::logic_error(">> Open Dialog : Cannot load Specified Resource.  (LOADRESFAILURE )");
        case CDERR_LOADSTRFAILURE  : throw std::logic_error(">> Open Dialog : Cannot load Specified string.    (LOADSTRFAILURE )");
        case CDERR_LOCKRESFAILURE  : throw std::logic_error(">> Open Dialog : Cannot lock Specified Resource.  (LOCKRESFAILURE )");
        case CDERR_MEMALLOCFAILURE : throw std::logic_error(">> Open Dialog : Cannot allocate Required Memory. (MEMALLOCFAILURE)");
        case CDERR_MEMLOCKFAILURE  : throw std::logic_error(">> Open Dialog : Cannot lock Handle Memory.       (MEMLOCKFAILURE )");
        case CDERR_NOHINSTANCE     : throw std::logic_error(">> Open Dialog : No HINSTANCE Provided when reqd. (NOHINSTANCE    )");
        case CDERR_NOHOOK          : throw std::logic_error(">> Open Dialog : No Hook Provided when required.  (NOHOOK         )");
        case CDERR_NOTEMPLATE      : throw std::logic_error(">> Open Dialog : No Template Provided when reqd.  (NOTEMPLATE     )");
        case CDERR_STRUCTSIZE      : throw std::logic_error(">> Open Dialog : The Structure Size is invalid.   (STRUCTSIZE     )");
        case FNERR_BUFFERTOOSMALL  : throw std::logic_error(">> Open Dialog : The File Buffer is very Small.   (BUFFERTOOSMALL )");
        case FNERR_INVALIDFILENAME : throw std::logic_error(">> Open Dialog : The specified file is invalid.   (INVALIDFILENAME)");
        case FNERR_SUBCLASSFAILURE : throw std::logic_error(">> Open Dialog : Subclassing a list box Failed.   (SUBCLASSFAILURE)");
        default                    : return ""; }
    }
}

/// The Callback Procedure for Directory Selection.
static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    if(uMsg == BFFM_INITIALIZED)
    { SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData); }
    return 0;
}

/** Opens a Dialog Box to allow the user to select a Directory. <br><br>
The user can change the caption inside the Dialog Box. <br> */
std::string GetFolder(std::string_view caption)
{
    char path[MAX_PATH]; BROWSEINFO bi = { 0 }; bi.lpszTitle  = caption.data();
    bi.ulFlags    = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpfn       = BrowseCallbackProc; bi.lParam     = 0;
    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
    if ( pidl != 0 )
    {
        SHGetPathFromIDList ( pidl, path ); IMalloc* imalloc = 0;
        if(SUCCEEDED(SHGetMalloc(&imalloc))) { imalloc->Free(pidl); imalloc->Release( ); } return path;
    }
    return "";
}

/** Wrapper for creating RTF Code for .doc/.rtf files.<br>
    <ul>
       <li>Functions : Font(), Init(), Print(), Close(), Color(), Data(), Save(), Path()  </li>
       <li> How to use : <ol>
           <li> First set the path of the Wrapper 'Word' to the .doc file, using Path().</li>
           <li> Then call Print() to add data line by line to the file.</li>
           <li> Finally, call the Save() function to save the file & write data to it.</li>
           <li> To load new file, call Clear(), respecify Path(), & repeat Steps 2 & 3.</li>
       </ol> </li>
       <li> The user can alter Color, Font, Alignment, Paragraph Setting, Bold <br>
            Setting, Underline Setting, Italic Setting & Size of the data to be printed.</li>
       <li> The Paragraph Setting is required when you want a group of lines to have <br>
            the same attributes and alignment.</li>
       <li> If you forgot to enter a file path, you can do so later, using Path().</li>
       <li> To clear the printed data from the file, use Clear().</li>
       <li> Note that unless you call Save(), the file remains empty.</li>
    </ul>*/
class Word
{
    static int fctr, cctr; static std::string ct, ft, content, fname;
    Word(){}
public:
    /// Creates the printed data from the file.
    static void Clear() { content = ""; std::fstream fout; fout.open(fname,std::ios::trunc|std::ios::out); fout.close();}
    /// Redirects the Object to a new .doc/.rtf file.
    static void Path(std::string_view path) { fname = path; }
    /// Creates a new font for use in the document. Returns an index in the fonttable.
    static int Font(std::string F="Consolas")
    {
        std::string res = "{\\f"+std::to_string(fctr)+" "+F+";}";
        ft.resize(ft.length()-1); ft+=(res+"}");
        fctr++; return fctr-1;
    }
    /// Creates a new color for use in the document. Returns an index in the colortable.
    static int Color(int r=0, int g=0, int b=0)
    {
        ct.resize(ct.length()-1);
        std::string res = "\\red"+std::to_string(r)+"\\green"+std::to_string(g)+"\\blue"+std::to_string(b)+";";
        cctr++; ct+=(res+"}"); return cctr-1;
    }
    /// Prints specified data with different attributes into the document.
    static void Print(std::string text, int fore_col, int back_col, int font, int size=14,
                      char align='l', bool bold=0, bool italic=0, bool underline=0, bool p=true)
    {
        std::string res;
        if(p)
        {
            res = "{\\pard"+(std::string("\\q")+align)+"\\f"+std::to_string(font)+"\\fs"+std::to_string(size*2)+
                  "\\chshdng0\\chcbpat"+std::to_string(back_col)+"\\cb"+std::to_string(back_col)+"\\cf"+std::to_string(fore_col)+
                  (bold?std::string("\\b1"):std::string("\\b0"))+(italic?std::string("\\i1"):std::string("\\i0"))+
                  (underline?std::string("\\ul1"):std::string("\\ul0"))+" "+text+"\\par}";
        }
        else
        {
            res = "{"+(std::string("\\q")+align)+"\\f"+std::to_string(font)+"\\fs"+std::to_string(size*2)+"\\chshdng0\\chcbpat"+
                   std::to_string(back_col)+"\\cb"+std::to_string(back_col)+"\\cf"+std::to_string(fore_col)+(bold?std::string
                   ("\\b1"):std::string("\\b0"))+(italic?std::string("\\i1"):std::string("\\i0"))+(underline?std::string("\\ul1"):
                   std::string("\\ul0"))+" "+text+"}";
        }
        content += res;
    }
    /// Returns the RTF Code of the file so far.
    static std::string Data()
    {
        std::string fins = std::string("{\\rtf1\\ansi\\margl720\\margr720\\margt720\\margb720") + ft + ct + content +
        std::string("}"); return fins;
    }
    /// Saves the RTF code to your document file.
    static void Save()
    {
        if(fname.empty()) throw std::logic_error(">> RTF : No File to Save To! Call Path()!");
        std::fstream fout; fout.open(fname,std::ios::trunc|std::ios::out); fout<<Data(); fout.close();
    }
};

std::string Word::ct = "{\\colortbl }", Word::ft="{\\fonttbl }", Word::fname="", Word::content = ""; int Word::fctr=0, Word::cctr=0;

const int Red       = Word::Color(255,  0,  0); const int Green     = Word::Color(  0,255,  0);
const int Blue      = Word::Color(  0,  0,255); const int Black     = Word::Color(  0,  0,  0);
const int White     = Word::Color(255,255,255); const int Yellow    = Word::Color(255,255,  0);
const int Orange    = Word::Color(255,140,  0); const int Pink      = Word::Color(255, 20,147);
const int Gold      = Word::Color(255,215,  0); const int Lime      = Word::Color(153,204, 50);
const int Skin      = Word::Color(255,231,186); const int Purple    = Word::Color(135, 31,120);
const int Violet    = Word::Color( 79, 47, 79); const int Maroon    = Word::Color(128,  0,  0);
const int DarkGreen = Word::Color(  0,128,  0); const int DarkBlue  = Word::Color(  0,  0,139);
const int Lilac     = Word::Color(154, 50,205); const int Brown     = Word::Color(139, 37,  0);
const int Chocolate = Word::Color(139, 69, 19); const int RoseWood  = Word::Color(166, 42, 42);
const int SkyBlue   = Word::Color( 56,176,222); const int NavyBlue  = Word::Color( 35, 35,142);
const int Turquoise = Word::Color(  0,229,238); const int DarkTrqse = Word::Color(  0,197,205);
const int BlueBlack = Word::Color( 25, 25,112);

const int Consolas        = Word::Font("Consolas");
const int Berlin_Sans_FB  = Word::Font("Berlin Sans FB");
const int Calibri         = Word::Font("Calibri");
const int Times_New_Roman = Word::Font("Times New Roman");
const int Arial           = Word::Font("Arial");
const int Aharoni         = Word::Font("Aharoni");
const int Harlow          = Word::Font("Harlow Solid Italic");

#endif // WINDOWS_HPP_INCLUDED
