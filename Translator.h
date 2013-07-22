#include "Tokeniser.h"

class Translator 
{
    Tokeniser* tokeniser;
    bool       dump;
    
    int        currentToken;
    
    std::vector <std::string> states;
    std::vector <std::string> inputs;
    std::vector <std::string> outputs;
    
    public:
        
         Translator        ();
    void SetTranslator     (Tokeniser* newTokeniser, bool newDump);
    
    bool CheckCurrentToken (int type, int subtype);
    bool ParsingError      (const char* expected);
    
    bool Translate         (FILE *output);
    bool ReadList          (const char* only, const char* multiple, std::vector <std::string>* list);
    bool States            ();
    bool Inputs            ();
    bool Outputs           ();
};