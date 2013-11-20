#include <iostream>
#include "Tokeniser.h"
#include "Translator.h"

int main(int argc, char **argv) 
{
    Tokeniser t;
    t.SetTokeniser ("lighter_test.txt", false);
    t.Tokenise ();
    
    Translator translator;
    translator.SetTranslator (&t, true);
    
    translator.Translate ();
}