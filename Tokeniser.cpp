#include "Tokeniser.h"
#include <stdio.h>


Tokeniser::Tokeniser()
{
    dump = false;
 
    keysData.pushKey ({TokenType::keyword, TokenSubtype::states, "States",         "states"});
    keysData.pushKey ({TokenType::divider, TokenSubtype::comma,  "Comma",          ","});
    keysData.pushKey ({TokenType::divider, TokenSubtype::start,  "Start of block", "{"});
    keysData.pushKey ({TokenType::divider, TokenSubtype::end,    "End of block",   "}"});
}

bool Tokeniser::SetTokeniser (const char* fileName, bool newDump)
{
    inputFile = fopen (fileName, "r");
    dump      = newDump;
}

bool Tokeniser::Tokenise()
{
    char currentSymbol;
    fscanf (inputFile, "%c", &currentSymbol);
    
    while (!feof(inputFile))
    {
        char newString [MAX_TOKEN_LENGTH] = "";
        
        while (currentSymbol == ' ' || currentSymbol == 10)
        {
               fscanf (inputFile, "%c", &currentSymbol);
               if (feof(inputFile)) break;
        }
        if (feof (inputFile)) break;
               
        if (keysData.isDivider (currentSymbol))
        {
            newString [0] = currentSymbol;
            newString [1] = 0;
            fscanf (inputFile, "%c", &currentSymbol);
        }
        else
        {
            for (int i = 0; 
                 !keysData.isDivider(currentSymbol) &&
                 currentSymbol != ' ' &&
                 currentSymbol != 10;
                 i++)
            {
                newString [i] = currentSymbol;
                fscanf (inputFile, "%c", &currentSymbol);
            }
        }
        
        //fscanf (inputFile, "", newString);
        std::string newStringStd (newString);
        
        Token* newToken;
        
        Key tokenKey = keysData.FindKey (newStringStd);
        
        newToken = new Token (tokenKey.type, tokenKey.subtype, newStringStd);
        
        tokensBuffer.push_back (newToken);
        
        if (dump)
        {
            char typeInString [MAX_TOKEN_LENGTH] = "";
            if (newToken -> type == TokenType::keyword) sprintf (typeInString, "Keyword");
            if (newToken -> type == TokenType::divider) sprintf (typeInString, "Divider");
            if (newToken -> type == TokenType::name)    sprintf (typeInString, "Name");
            
            printf ("Token's string = \"%s\", "
                    "token's type = %d (%s), "
                    "token's subtype = %d (%s)\n",
                    newString, newToken -> type, typeInString, newToken -> subtype, tokenKey.name.c_str());
        }
    }
}
