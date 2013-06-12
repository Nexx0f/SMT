#include <iostream>
#include "Tokeniser.h"

int main(int argc, char **argv) 
{
    Tokeniser t;
    t.SetTokeniser ("test.txt", true);
    t.Tokenise ();
}
