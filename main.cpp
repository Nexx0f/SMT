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
    
    Generator gen (translator.states.size(),
                   translator.inputs.size(),
                   translator.outputs.size(),
                   true,
                   translator.conditionalTransits,
                   translator.conditionalOutputs);
    
    FILE* output = fopen ("output.v", "w");
    gen.Generate (output);
    fclose (output);
}