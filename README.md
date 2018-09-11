# Win-32-Interface-Libraries
Headers to implement Pre-GUI freatures in C++ Programs using Win32 Wrapper Classes.

## Windows.hpp
This header provides a wrapper class to facilitate easy interfacing with the Console, using a Static Class. 
Most of the properties like Color, Size, Position & Input Modes are alterable and one can even execute other programs in child consoles.
The header also provides users function for creating various types of message boxes, launching Dialog Boxes, and perform and 
I/O interface with RTF/DOC files.

- **Classes** : 
  - `Console` : Static Class to interface with the Console Window (the black screen).
    - Functions : Cursor, Size, Position, Center, Init, Save, To_Original, MouseInput, IsMouseInputEnabled, Border, Execute, CSize, Overall Color, CursorAt, Path, Drive
    - Enumerations : Color::Foreground (Foreground Colors), Color::Background (Background Colors), EXEType (Path type for Executables)
  - `Word` : Static Class to perform I/O with RTF/DOC Files, with proper formatting.
    - Functions : Font, Init, Print, Close, Color, Data, Save, Path
    - NOTE : Predefined Fonts and Colors Exist. New Fonts and Colors can be defined as well.
- **Functions** : 
  - `GetFile` : Gets a file's path using the 'Open' Dialog Box in Windows.
  - `GetFolder` : Gets a directory's path using 'Browse for Folder' Dialog Box in Windows.
  - `Head` : Prints a heading encased in a box.
  - `SplHead` : Prints a heading encased in a well-defined box.
  - `InfoBox` : Displays a Message Box with the Information Icon, and executes some function if its run was successful.
  - `ErrorBox` : Displays a Message Box with the Error Icon, and executes some function if its run was successful.
  - `QuestionBox` : Displays a Message Box with the Question Icon, and executes some function based on the clicked button (Yes/No).
  - `Exist` : Checks if a Particular file exists on the computer.

## UI_Element.hpp
This header, dependent on Windows.hpp, provides UI elements like buttons and form boxes. These can be used to interface with the user. 
There are 5 types of buttons : 

1. **Normal Buttons** : 
   Buttons that may direct to some function or perform some operation when clicked.
2. **Image Buttons** : 
   Buttons that may hold an `ASCII FORMATTED IMAGE` inside. 
   For more details, check [ASCII Paint](https://github.com/kinshuk-h/ASCII-Paint).
3. **Static Buttons** : Just like Normal Buttons, but with unalterable properties.
4. **Static Image Buttons** : Just like Normal Image Buttons, but with unalterable properties.
5. **Radio Buttons** : Buttons that can be checked or unchecked.
   
Apart from buttons, the header provides form boxes (as the class `Box`, that can be used to read input from user, when clicked upon. 
These can also be used as tables, by specifying connectors at construction. During read operations, these boxes can be checked to 
satisfy a particular criteria or regex. 

- **Classes** : 
  - `BUTTON_TYPE` : Base class to define a button object, that is clickable.
     - Functions :  Clear, Print, IsClicked, Recolor, operator=.
  - `Button : public BUTTON_TYPE` : A Class to define Buttons, or clickable elements that direct to some operation.
     - Functions : Activate, Move, Resize, Data.
  - `Image_Button : public BUTTON_TYPE` : A Class to define Buttons that can display `ASCII FORMATTED IMAGE`s alongside.
     - Functions : Print, Resize, Move, Reload, Data
  - `Radio_Button : public BUTTON_TYPE` : A Class to define radio Buttons that are checkable.
     - Functions : Checked, Check, Move
  - `Static_Button : public BUTTON_TYPE` : Unalterable `Button`s. (No Extra Functions).
  - `Static_Image_Button : public BUTTON_TYPE` : Unalterable `Image_Button`s. (No Extra Functions).
  - `Box` : Class to declare Form Boxes that can be clicked anytime to recieve inputs.
    - Functions : Fill, Read, Clear, IsClicked 
    - Enumerations : Limit(Limiting Condition during Read), Type(Input Type, Normal or enforced with Limits), Row(Row Positions), Column(Column Positions)
- **Functions** : 
  - `Check()` : This function checks if the data which is going to be printed does not overlap on any UI element.
  
## Usage
For usage, look at Sample Programs provided along with the headers.
