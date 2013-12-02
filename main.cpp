#include <iostream>
#include "Tokeniser.h"
#include "Translator.h"
#include <string.h>

int main(int argc, char* argv[]) 
{
    Tokeniser t;
    bool dump = false;
    if (argc >= 4)
        if (!strcmp (argv[3], "init_dump")) dump = true;
    
    if (argc >= 2) t.SetTokeniser (argv[1], dump);
    else           t.SetTokeniser ("input.txt", dump);
    t.Tokenise ();
    
    Translator translator;
    translator.SetTranslator (&t, dump);
    
    translator.Translate ();
    
    Generator gen (translator.states.size(),
                   translator.inputs.size(),
                   translator.outputs.size(),
                   dump,
                   translator.conditionalTransits,
                   translator.conditionalOutputs);
    
    FILE* output;
    if (argc >= 3) output = fopen (argv[2], "w");
    else           output = fopen ("output.v", "w");
    gen.Generate (output);
    fclose (output);
}