#include "CppReflect.h"
using namespace std;


class Person
{
public:
    REFLECTABLE( Person,
        (CString)   name,
        (int)	    age,
        (bool)      isAdult,
        (ColorRef)  eyeColor
    )

};

class People
{
public:
    REFLECTABLE( People,
        (CString) groupName,
        (vector<Person>)  people
    )
};


void main(void)
{
    People ppl;
    ppl.groupName = "Group1";

    Person p;
    p.name = L"Roger";
    p.age = 37;
    p.isAdult = true;
    p.eyeColor = RGB(255,0,0);
    ppl.people.push_back(p);
    p.name = L"Alice";
    p.age = 27;
    p.isAdult = true;
    p.eyeColor = RGB( 0, 255, 0 );
    ppl.people.push_back( p );
    p.name = L"Cindy";
    p.age = 17;
    p.isAdult = false;
    p.eyeColor = RGB( 0, 0, 255 );
    ppl.people.push_back( p );

    CStringW xml = ToXML( &ppl );
    CStringW errors;

    People ppl2;

    FromXml( &ppl2, xml, errors );
    CStringA xml2 = ToXML( &ppl2 );
    printf( xml2 );

    FILE* out = fopen( "test.xml", "wb" );
    unsigned char utf8_hdr[] = { 0xEF, 0xBB, 0xBF };
    fwrite( utf8_hdr, 3, 1, out );
    fwrite( xml2.GetBuffer(), xml2.GetLength(), 1, out);
    fclose(out);
}


