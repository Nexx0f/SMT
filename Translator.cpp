#include "Translator.h"

Translator::Translator ()
{
}

void Translator::SetTranslator(Tokeniser* newTokeniser, bool newDump)
{
    tokeniser = newTokeniser;
    dump      = newDump;
}
bool Translator::Translate(FILE* output)
{
    currentToken = 0;
    int tokensBufferSize = tokeniser -> tokensBuffer.size();
    
    while (currentToken < tokensBufferSize)
    {
        if (CheckCurrentToken (TokenType::keyword, TokenSubtype::states))
        {
            if (!States ()) return false;
        }
        else
        if (CheckCurrentToken (TokenType::keyword, TokenSubtype::inputs))
        {
            if (!Inputs ()) return false;
        }
        else
        if (CheckCurrentToken (TokenType::keyword, TokenSubtype::outputs))
        {
            if (!Outputs ()) return false;
        }
    }
}

bool Translator::CheckCurrentToken (int type, int subtype)
{
    if (tokeniser -> tokensBuffer [currentToken] -> type    == type &&
        tokeniser -> tokensBuffer [currentToken] -> subtype == subtype) return true;
    else                                                                return false;
}

bool Translator::ParsingError (const char* expected)
{
    if (dump)
    {
        printf ("Parsing error: Expected \"%s\", found \"%s\"\n", 
                expected, tokeniser -> tokensBuffer [currentToken] -> name.c_str());
    }
    return false;
} 

bool Translator::ReadList(const char* only, const char* multiple, std::vector <std::string>* list)
{
    currentToken++;
    
    if (dump) printf ("Begin to read list of %s (Token \"%s\" was found)\n", multiple, multiple);
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::start)) currentToken++;
    else return ParsingError ("{");
    
    while (!CheckCurrentToken(TokenType::divider, TokenSubtype::end))
    {
        if (CheckCurrentToken(TokenType::name, TokenSubtype::name))
        {
            list -> push_back (tokeniser -> tokensBuffer [currentToken] -> name);
            if (dump) printf ("New %s \"%s\" was read\n", only, tokeniser -> tokensBuffer [currentToken] -> name.c_str());
            currentToken++;
        }
        else if (CheckCurrentToken(TokenType::divider, TokenSubtype::comma))
        {
            currentToken++;
        }
        else 
        {
            char errorString [256] = "";
            sprintf (errorString, "<Name of %s> or ,", only);
            return ParsingError (errorString);
        }
    }
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::end)) currentToken++;
    else return ParsingError ("}");
    
    if (dump) printf ("End of reading list of states\n\n");
    
    return true;
}

bool Translator::States()
{
    return ReadList ("state", "states", &states);
}

bool Translator::Inputs()
{
    return ReadList ("input", "inputs", &inputs);
}

bool Translator::Outputs()
{
    return ReadList ("output", "outputs", &outputs);
}