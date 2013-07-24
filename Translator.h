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
        
         Translator          ();
    void SetTranslator       (Tokeniser* newTokeniser, bool newDump);
    
    bool CheckCurrentToken   (int type, int subtype);
    bool CheckName           (Token *token, const char* only, std::vector <std::string>* list);
    bool ParsingError        (const char* expected);
    void SkipSpaces          (FILE* output, int deep);
    
    bool Translate           (FILE* output);
    bool TranslateMainBlocks (FILE* output, int deep);
    
    bool ReadList            (const char* only, const char* multiple, std::vector <std::string>* list);
    
    bool States              ();
    bool Inputs              ();
    bool Outputs             ();
    bool Switch              (FILE* output, int deep);
    bool State               (FILE* output, int deep);
    bool IfBlock             (FILE* output, int deep);
};