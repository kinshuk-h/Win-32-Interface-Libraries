#include "H:\All C++ Files\Custom Headers\KV\UI_Element.hpp"
using namespace std;

int main()
{
    try
    {
        Console::Init(); Console::Size(60,14); Console::Center(); Console::BufferSize(60,14);
        GOTO(1,2); cout<<"> Name          : ";
        GOTO(1,4); cout<<"> Date of Birth : ";
        GOTO(1,6); cout<<"> Subject       : ";
        Box B1(20,1,16,Box::Row::Nil,Box::Column::Start),
            B2(20,3,16,Box::Row::Nil,Box::Column::Mid),
            B3(20,5,16,Box::Row::Nil,Box::Column::End);
        Box* BP = &B1; string s1,s2,s3; Button B("SUBMIT",45,3,0,0,RED,WHITE); B.Print();
        while(true)
        {
            COORD C = BP->Read();
            if((C.X==-1)&&(C.Y==-1))
            {
                if(BP==&B1) {BP=&B2;}
                else if(BP==&B2) {BP=&B3;}
                else if(BP==&B3) {s1=B1.Retrieve();s2=B2.Retrieve();s3=B3.Retrieve(); goto down;}
            }
            else
            {
                if(B1.IsClicked(C)) {BP=&B1;} else if(B2.IsClicked(C)) {BP=&B2;} else if(B3.IsClicked(C)) {BP=&B3;}
                else if(B.IsClicked(C)) {s1=B1.Retrieve();s2=B2.Retrieve();s3=B3.Retrieve(); goto down;}
            }
        }
        down: GOTO(0,9); cout<<"Details = "<<s1<<", "<<s2<<", "<<s3<<"\n";
        system("pause");
    }
    catch(exception& e) { InfoBox(e.what(),"",[](){}); }
}
