#include <stdio.h>
#include <vector>
#include <string.h>

#ifndef H_TOKENISER
#define H_TOKENISER

#include "Tokens.h"

const int MAX_TOKEN_LENGTH = 256;
class Tokeniser
{
    std::vector <Token*> tokensBuffer;
    KeysData             keysData;
    FILE*                inputFile;
    bool                 dump;
    
public:
                   Tokeniser    ();
    bool           SetTokeniser (const char* fileName, bool newDump);
    
    bool           Tokenise     ();
};

#endif