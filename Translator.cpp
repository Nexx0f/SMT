#include "Translator.h"

#define SS SkipSpaces(deep);

Translator::Translator ()
{
    dump = false;
    currentParsingState = 0;
    currentToken        = 0;
}

void Translator::SetTranslator(Tokeniser* newTokeniser, bool newDump)
{
    tokeniser = newTokeniser;
    dump      = newDump;
}

bool Translator::SortConditionalOutputs()
{

/*      Sorting conditions. Index of the transitional
 * conditional action (<do> block) have to be lower than others.
 * Or in some cases generated code will be incorrect */    

    for (int state = 0; state < conditionalOutputs.size(); state++)
         for (int first = 0; first < conditionalOutputs[state].size()-1; first++)
              for (int i = 0; i < conditionalOutputs[state].size()-1; i++)
              {
                  if (conditionalOutputs[state][i+1].condition.onState &&
                      !conditionalOutputs[state][i].condition.onState)
                  {
                      ConditionalAction temp         = conditionalOutputs[state][i];
                      conditionalOutputs[state][i]   = conditionalOutputs[state][i+1];
                      conditionalOutputs[state][i+1] = temp;
                  }
              }
    
}

bool Translator::Translate()
{
    currentToken = 0;
    currentParsingState = 0;
    int deep = 0;
    
    if (dump) printf ("First <TranslateMainBlock> called\n");
    int result = TranslateMainBlocks (Condition {}, 0, 0);
    if (dump) printf ("Exit from first <TranslateMainBlock>\n");
    
    
    if (dump) printf ("\n"
                      "Sorting conditional output actions...  ");
    SortConditionalOutputs ();
    if (dump) printf ("Completed.");
        
    deep = 4;
    
    if (dump) printf ("\n\nPrinting all conditional actions:\n");
    for (int i = 0; i < conditionalTransits.size(); i++)
         for (int t = 0; t < conditionalTransits[i].size(); t++)
         {
             if (dump) 
             {
                 SS printf ("Transition from state %d. Conditional inputs are", i);
                conditionalTransits[i][t].DumpConditionalAction (&states);
             }
         }
         
    if (dump) printf ("\n");
    for (int i = 0; i < conditionalOutputs.size(); i++)
         for (int t = 0; t < conditionalOutputs[i].size(); t++)
         {
             if (dump) 
             {
                 SS printf ("Output action from state %d. Conditional inputs are", i);
                 conditionalOutputs[i][t].DumpConditionalAction (&states);
             }
         }
         
    return result;
}

bool Translator::TranslateMainBlocks(Condition condition, int stateNumber, int deep)
{
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
        else
        if (CheckCurrentToken (TokenType::keyword, TokenSubtype::switchType))
        {
            if (!Switch ()) return false;
        }
        else
        if (CheckCurrentToken (TokenType::keyword, TokenSubtype::state))
        {
            if (dump) {SS printf ("Opening <state> block\n");}
                if (!State (condition, deep)) return false;
            if (dump) {SS printf ("<state> block closed\n");}
        }
        else
        if (CheckCurrentToken (TokenType::keyword, TokenSubtype::ifType))
        {
            
            if (dump) {SS printf ("Opening <if> block\n");}
                if (!IfBlock (condition, stateNumber, deep)) return false;
            if (dump) {SS printf ("<if> block closed\n");}
        }
        else
        if (CheckCurrentToken (TokenType::keyword, TokenSubtype::emitSignal))
        {
            if (dump) {SS printf ("Reading <emitSignal>\n");}
                if (!EmitSignal (condition, stateNumber, deep)) return false;
            if (dump) {SS printf ("<emitSignal> read\n");}    
        }
        else
        if (CheckCurrentToken (TokenType::keyword, TokenSubtype::stopSignal))
        {
            if (dump) {SS printf ("Reading <stopSignal>\n");}
                if (!StopSignal (condition, stateNumber, deep)) return false;
            if (dump) {SS printf ("<stopSignal> read\n");}
        }
        else
        if (CheckCurrentToken (TokenType::keyword, TokenSubtype::stopSignals))
        {
            if (dump) {SS printf ("Reading <stopSignals>\n");}
                if (!StopSignals (condition, stateNumber, deep)) return false;
            if (dump) {SS printf ("<stopSignals> read\n");}
        }
        else
        if (CheckCurrentToken (TokenType::keyword, TokenSubtype::transition))
        {
            if (dump) {SS printf ("Reading <transition>\n");}
                if (!Transition (condition, stateNumber, deep)) return false;
            if (dump) {SS printf ("<transition> read\n");}
        }
        else
        if (CheckCurrentToken (TokenType::divider, TokenSubtype::end))
        {
            return true;
        }
        else
        {
            return ParsingError ("Any defined keyword");
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
        printf ("\nParsing error: Expected \"%s\", found \"%s\"\n\n", 
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
    bool success =  ReadList ("state", "states", &states);
    conditionalTransits.resize (states.size());
    conditionalOutputs.resize (states.size());
    return success;
}

bool Translator::Inputs()
{
    return ReadList ("input", "inputs", &inputs);
}

bool Translator::Outputs()
{
    return ReadList ("output", "outputs", &outputs);
}

void Translator::SkipSpaces (int deep)
{
    for (int i = 0; i < deep; i++) printf (" ");
}

bool Translator::Switch ()
{
    currentToken++;
    if (dump) printf ("Begin to read switch\n");
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::start)) currentToken++;
    else return ParsingError ("{");
    
    Condition condition;

    
    if (!TranslateMainBlocks(condition, 0, 0)) return false;
    
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::end)) currentToken++;
    else return ParsingError ("}");

    
    return true;
}

bool Translator::CheckName (Token* token, const char* only, std::vector <std::string>* list)
{
    for (int i = 0; i < list -> size(); i++)
    {
        if (token -> name == (*list) [i])
            return true;
    }
    if (dump) printf ("\nError: Undeclared %s <%s>", only, token -> name.c_str());
    return false;
}

bool Translator::State (Condition condition, int deep)
{
    currentToken++;
    
    if (!CheckName (tokeniser -> tokensBuffer [currentToken], "state", &states))
        return false;
    
    int stateNumber = -1;
    for (int i = 0; i < states.size(); i++)
         if (tokeniser -> tokensBuffer [currentToken] -> name == states [i])
             stateNumber = i;
    currentParsingState = stateNumber;
    
    currentToken++;
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::start)) currentToken++;
    else return ParsingError ("{");
    
   
    if (!TranslateMainBlocks(condition, stateNumber, deep+4)) return false;
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::end)) currentToken++;
    else return ParsingError ("}");
    
    return true;
}

bool Translator::ReadConditionsList(Condition* condition, int stateNumber, int deep)
{
    while (true)
    {
        if (!CheckName (tokeniser -> tokensBuffer [currentToken], "input", &inputs))
            return false;
    
        int inputNumber = -1;
        for (int i = 0; i < inputs.size(); i++)
             if (tokeniser -> tokensBuffer [currentToken] -> name == inputs [i])
             {
                 inputNumber = i;
                 break;
             }
             
        condition -> PushSubCondition (inputNumber);
        currentToken++;
        
        if (CheckCurrentToken (TokenType::divider, TokenSubtype::comma)) currentToken++;
        else
        if (CheckCurrentToken (TokenType::divider, TokenSubtype::rightBracket)) break;
        else return ParsingError ("',', ')'");
    }
}

bool Translator::IfBlock(Condition condition, int stateNumber, int deep)
{
    currentToken++;
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::leftBracket)) currentToken++;
    else return ParsingError ("(");
    
    ReadConditionsList (&condition, stateNumber, deep+4);
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::rightBracket)) currentToken++;
    else return ParsingError (")");
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::start)) currentToken++;
    else return ParsingError ("{");
    
    if (!TranslateMainBlocks(condition, stateNumber, deep+4)) return false;
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::end)) currentToken++;
    else return ParsingError ("}");
    
    return true;
}

bool Translator::EmitSignal (Condition condition, int stateNumber, int deep)
{
    currentToken++;
        
    if (!CheckName (tokeniser -> tokensBuffer [currentToken], "output", &outputs))
        return false;
    
    int outputNumber = 0;
    for (int i = 0; i < outputs.size(); i++)
         if (tokeniser -> tokensBuffer [currentToken] -> name == outputs [i])
         {
             outputNumber = i;
             break;
         }
    ConditionalAction newAction (condition, outputNumber, ActionType::signalEmitting);
    if (dump) 
    {
        deep+=4;
        SS newAction.DumpConditionalAction(&states);
    }
    
    if (!condition.onState) conditionalOutputs[stateNumber].push_back(newAction);
    else
    {
            int tmp = condition.transitionState;
            newAction.condition.transitionState = stateNumber;
            conditionalOutputs[tmp].push_back(newAction);
    }
    
    currentToken++;
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::colon)) currentToken++;
    else return ParsingError (";");
    
    return true;
}

bool Translator::StopSignal (Condition condition, int stateNumber, int deep)
{
    currentToken++;
    
    if (!CheckName (tokeniser -> tokensBuffer [currentToken], "output", &outputs))
        return false;
    
    int outputNumber = 0;
    for (int i = 0; i < outputs.size(); i++)
         if (tokeniser -> tokensBuffer [currentToken] -> name == outputs [i])
         {
             outputNumber = i;
             break;
         }
         
    ConditionalAction newAction (condition, outputNumber, ActionType::signalStopping);
    if (dump) 
    {
        deep+=4;
        SS newAction.DumpConditionalAction(&states);
    }
    
    if (!condition.onState) conditionalOutputs[stateNumber].push_back(newAction);
    else
    {
            int tmp = condition.transitionState;
            newAction.condition.transitionState = stateNumber;
            conditionalOutputs[tmp].push_back(newAction);
    }
    
    currentToken++;
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::colon)) currentToken++;
    else return ParsingError (";");
    
    return true;
}

bool Translator::StopSignals (Condition condition, int stateNumber, int deep)
{
    currentToken++;
    
    ConditionalAction newAction (condition, -1, ActionType::allSignalsStopping);
    if (dump) 
    {
        deep+=4;
        SS newAction.DumpConditionalAction(&states);
    }
    
    if (!condition.onState) conditionalOutputs[stateNumber].push_back(newAction);
    else
    {
            int tmp = condition.transitionState;
            newAction.condition.transitionState = stateNumber;
            
            conditionalOutputs[tmp].push_back(newAction);
    }
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::colon)) currentToken++;
    else return ParsingError (";");
    
    return true;
}

bool Translator::Transitto (Condition condition, int stateNumber, int deep)
{
    currentToken++;
    
    if (!CheckName (tokeniser -> tokensBuffer [currentToken], "state", &states))
        return false;
    
    int newStateNumber = -1;
    for (int i = 0; i < states.size(); i++)
         if (tokeniser -> tokensBuffer [currentToken] -> name == states [i])
             newStateNumber = i;
         
    ConditionalAction newAction (condition, newStateNumber, ActionType::transition);
    if (dump) 
    {
        deep+=4;
        SS newAction.DumpConditionalAction(&states);
    }
    conditionalTransits[stateNumber].push_back(newAction);
    currentToken++;
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::colon)) currentToken++;
    else return ParsingError (";");
    
    return true;
}

bool Translator::Transition(Condition condition, int stateNumber, int deep)
{
    /* 1) Read conditions
     * 2) Read destination state 
     * 3) Generate transition event 
     * 4) Call TranslateMainBlocks with special conditions */
    
    currentToken++;
    
    if (CheckCurrentToken (TokenType::keyword, TokenSubtype::on)) currentToken++;
    else return ParsingError ("on");
    
    /* Reading condition block. On that conditions 
     * we will transit to the next state */
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::leftBracket)) currentToken++;
    else return ParsingError ("(");
    
    ReadConditionsList (&condition, stateNumber, deep+4);
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::rightBracket)) currentToken++;
    else return ParsingError (")");
    
    /* Condition block was read */
    
    /* Reading destination state */
    
    if (CheckCurrentToken (TokenType::keyword, TokenSubtype::to)) currentToken++;
    else return ParsingError ("to");
    
    int newStateNumber = -1;
    for (int i = 0; i < states.size(); i++)
         if (tokeniser -> tokensBuffer [currentToken] -> name == states [i])
             newStateNumber = i;   
         
    ConditionalAction newAction (condition, newStateNumber, ActionType::transition);
    if (dump) 
    {
        deep+=4;
        SS newAction.DumpConditionalAction(&states);
    }
    conditionalTransits[stateNumber].push_back(newAction);
    currentToken++;
    
    /* Destination state read */
    
    /* Making special condition and reading do block */
    
    if (!condition.PushStateCondition (newStateNumber)) return false;
   
    if (CheckCurrentToken (TokenType::keyword, TokenSubtype::tokenDo)) currentToken++;
    else return ParsingError ("do");
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::start)) currentToken++;
    else return ParsingError ("{");
    
    if (!TranslateMainBlocks(condition, stateNumber, deep+4)) return false;
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::end)) currentToken++;
    else return ParsingError ("}");
    
    return true;
}