#include "Tokeniser.h"
#include "Conditions.h"

class Translator 
{
    Tokeniser* tokeniser;
    bool       dump;
    
    int        currentToken;
    int        currentParsingState;
    
    std::vector <std::string> states;
    std::vector <std::string> inputs;
    std::vector <std::string> outputs;
    
    std::vector <std::vector <ConditionalAction> > conditionalTransits;
    std::vector <std::vector <ConditionalAction> > conditionalOutputs;
    
    public:
        
         Translator          ();
    void SetTranslator       (Tokeniser* newTokeniser, bool newDump);
    
    bool CheckCurrentToken   (int type, int subtype);
    bool CheckName           (Token *token, const char* only, std::vector <std::string>* list);
    bool ParsingError        (const char* expected);
    void SkipSpaces          (int deep);
    
    bool Translate           ();
    bool TranslateMainBlocks (Condition condition, int stateNumber, int deep);
    
    bool ReadList            (const char* only, const char* multiple, std::vector <std::string>* list);
    
    bool States              ();
    bool Inputs              ();
    bool Outputs             ();
    bool Switch              ();
    bool State               (Condition condition, int deep);
    bool IfBlock             (Condition condition, int stateNumber, int deep);
    bool EmitSignal          (Condition condition, int stateNumber, int deep);
    bool StopSignal          (Condition condition, int stateNumber, int deep);
    bool StopSignals         (Condition condition, int stateNumber, int deep);
    bool Transitto           (Condition condition, int stateNumber, int deep);
};              