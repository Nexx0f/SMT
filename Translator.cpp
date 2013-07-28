#include "Translator.h"

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
bool Translator::Translate(FILE* output)
{
    currentToken = 0;
    currentParsingState = 0;
    int deep = 0;
    
    return TranslateMainBlocks (output, deep);
}

bool Translator::TranslateMainBlocks(FILE* output, int deep)
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
            if (!Switch (output, deep)) return false;
        }
        else
        if (CheckCurrentToken (TokenType::keyword, TokenSubtype::state))
        {
            if (!State (output, deep)) return false;
        }
        else
        if (CheckCurrentToken (TokenType::keyword, TokenSubtype::ifType))
        {
            if (!IfBlock (output, deep)) return false;
        }
        else
        if (CheckCurrentToken (TokenType::keyword, TokenSubtype::emitSignal))
        {
            if (!EmitSignal (output, deep)) return false;
        }
        else
        if (CheckCurrentToken (TokenType::keyword, TokenSubtype::stopSignal))
        {
            if (!StopSignal (output, deep)) return false;
        }
        else
        if (CheckCurrentToken (TokenType::keyword, TokenSubtype::stopSignals))
        {
            if (!StopSignals (output, deep)) return false;
        }
        else
        if (CheckCurrentToken (TokenType::keyword, TokenSubtype::transitto))
        {
            if (!Transitto (output, deep)) return false;
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

void Translator::SkipSpaces (FILE* output, int deep)
{
    for (int i = 0; i < deep; i++) fprintf (output, " ");
}

bool Translator::Switch (FILE* output, int deep)
{
    currentToken++;
    if (dump) printf ("Begin to read switch\n");
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::start)) currentToken++;
    else return ParsingError ("{");
    
    /* There is a start of translating main swith
       to verilog's module. It contains prototype
       of module, <state> variable initialisation, and begin
       of "always" block */
    
    SkipSpaces (output, deep);
    fprintf (output, "module state_machine (");
    
    if (dump) printf ("Translating inputs...  (");
        
    int space = strlen ("module state_machine (");
    for (int i = 0; i < inputs.size(); i++)
    {
        fprintf (output, "input  %s,\n", inputs [i].c_str());
        SkipSpaces (output, deep + space); 
            
        if (dump) printf  ("%s", inputs [i].c_str());
        if (i != inputs.size() - 1) 
            if (dump) printf (", ");
    }
    
    if (dump) printf (")\nTranslating outputs... (");
    
    for (int i = 0; i < outputs.size(); i++)
    {
        fprintf (output, "output reg %s", outputs [i].c_str());
        if (i != outputs.size() - 1)
        {
            fprintf (output, ",\n");
            SkipSpaces (output, deep + space); 
        }
        
        if (dump) printf  ("%s", outputs [i].c_str());
        if (i != outputs.size() - 1) 
            if (dump) printf (", ");
    }
    
    if (dump) printf (")\n");
    fprintf (output, ");\n\n");
    
    /* Prototype of module was generated.
       Now initialisation of <state> variable */
    
    SkipSpaces (output, deep);
    fprintf (output, "reg [%d:0] state = 1;\n\n", states.size() - 1);
    if (dump) printf ("State variable was initialised. "
                      "State machine has %d states. "
                      "It means that state variable will have %d bits.\n", 
                      states.size(), states.size());
    
    /* <state> varianle was initialised.
       Now begin of always block. */
    
    SkipSpaces (output, deep);
    fprintf (output, "always @("); 
    
    int secondSpace = strlen ("always @(");
    for (int i = 0; i < inputs.size(); i++)
    {
        fprintf (output, "%s", inputs [i].c_str());
        if (i != inputs.size() - 1)
        {
            fprintf (output, " or\n");
            SkipSpaces (output, deep + secondSpace);
        }
    }
    fprintf (output, ")\n");
    SkipSpaces (output, deep);
    fprintf (output, "begin\n");
    
    if (dump) printf ("Always block was inialised.\n"
                      "Recursive call of blocks translate function...\n");
    
    /* Begin of always block was written.
       Now recursive all of function <TranslateMainBlocks>.
       It have to read and translate all
       that switch contains */
    
    if (!TranslateMainBlocks(output, deep + 4)) return false;
    
    /* There is end of generating file.
       It contains end of always block and
       <endmodule> keyword */
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::end)) currentToken++;
    else return ParsingError ("}");
    
    SkipSpaces (output, deep);
    fprintf (output, "end\n\n");
    SkipSpaces (output, deep);
    fprintf (output, "endmodule");
    if (dump) printf ("Blocks translate function ended it's work\n"
                      "Writing end of always block and <endmodule> keyword to output file...\n\n\n"
                      "Switch DONE\n\n");
    
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

bool Translator::State (FILE* output, int deep)
{
    currentToken++;
    
    /* There is a conditional block which
       executes when state machine is on defined
       state. First we have to read name of state,
       find it's number and then translate it
       to the <if> block in verilog code */
    
    if (dump) printf ("Begin to read state block\n");
    
    if (!CheckName (tokeniser -> tokensBuffer [currentToken], "state", &states))
        return false;
    
    int stateNumber = -1;
    for (int i = 0; i < states.size(); i++)
         if (tokeniser -> tokensBuffer [currentToken] -> name == states [i])
             stateNumber = i;
    currentParsingState = stateNumber;
         
    SkipSpaces (output, deep);     
    fprintf (output, "if (state [%d] == 1)\n", stateNumber);     
    if (dump) printf ("<If> block (<state> block in smt language) was initialised\n"
                      "Number of bit that equals <%s> state is %d\n",
                      tokeniser -> tokensBuffer [currentToken] -> name.c_str(), stateNumber);
    
    currentToken++;
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::start)) currentToken++;
    else return ParsingError ("{");
    
    SkipSpaces (output, deep);
    fprintf (output, "begin\n");
    
    if (dump) printf ("Recursive call of blocks translate function...\n");
    
    /* Begin of if block was written.
       Now recursive all of function <TranslateMainBlocks>.
       It have to read and translate all
       that state block contains */
    
    if (!TranslateMainBlocks(output, deep + 4)) return false;
    
    /* There is end of if block.
       It contains end keyword */
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::end)) currentToken++;
    else return ParsingError ("}");
    
    if (CheckCurrentToken (TokenType::keyword, TokenSubtype::state)) 
    {
        SkipSpaces (output, deep);
        fprintf (output, "end\n");
        SkipSpaces (output, deep);
        fprintf (output, "else\n");
    }
    else 
    {
        SkipSpaces (output, deep);
        fprintf (output, "end\n");
    }
        
    if (dump) printf ("Blocks translate function ended it's work\n"
                      "Writing end of if block to output file...\n\n\n");
    
    return true;
}

bool Translator::IfBlock(FILE *output, int deep)
{
    currentToken++;
    
    /* This is a conditional block which 
       executes when defined input
       has high level. This block translates
       to the <if> block in verilog */
    
    if (dump) printf ("Begin to read if block\n");
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::leftBracket)) currentToken++;
    else return ParsingError ("(");
    
    if (!CheckName (tokeniser -> tokensBuffer [currentToken], "input", &inputs))
        return false;
    
    SkipSpaces (output, deep);
    fprintf    (output, "if (%s", tokeniser -> tokensBuffer [currentToken] -> name.c_str());
    currentToken++;

    if (CheckCurrentToken (TokenType::divider, TokenSubtype::rightBracket)) currentToken++;
    else return ParsingError (")");
    fprintf (output, ")\n");
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::start)) currentToken++;
    else return ParsingError ("{");
    SkipSpaces (output, deep);
    fprintf    (output, "begin\n");
    
    /* Begin of if block was written.
       Now recursive all of function <TranslateMainBlocks>.
       It have to read and translate all
       that state block contains */
    
    if (!TranslateMainBlocks(output, deep + 4)) return false;
    
    /* There is end of if block.
       It contains end keyword */
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::end)) currentToken++;
    else return ParsingError ("}");
    SkipSpaces (output, deep);
    fprintf    (output, "end\n");
    
    return true;
}

bool Translator::EmitSignal (FILE* output, int deep)
{
    currentToken++;
    
    /* This is a <emitsignal> command which 
       assigns defined output to high level.
       It translates to assignment in verilog */
    
    if (dump) printf ("Begin to read emitsignal\n");
    
    if (!CheckName (tokeniser -> tokensBuffer [currentToken], "output", &outputs))
        return false;
    
    SkipSpaces (output, deep);
    fprintf    (output, "%s = 1;\n", tokeniser -> tokensBuffer [currentToken] -> name.c_str());
    currentToken++;
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::colon)) currentToken++;
    else return ParsingError (";");
    
    return true;
}

bool Translator::StopSignal (FILE* output, int deep)
{
    currentToken++;
    
    /* This is a <stopsignal> command which 
       assigns defined output to low level.
       It translates to assignment in verilog */
    
    if (dump) printf ("Begin to read stopsignal\n");
    
    if (!CheckName (tokeniser -> tokensBuffer [currentToken], "output", &outputs))
        return false;
    
    SkipSpaces (output, deep);
    fprintf    (output, "%s = 0;\n", tokeniser -> tokensBuffer [currentToken] -> name.c_str());
    currentToken++;
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::colon)) currentToken++;
    else return ParsingError (";");
    
    return true;
}

bool Translator::StopSignals (FILE* output, int deep)
{
    currentToken++;
    
    /* This is a <signal> command which 
       assigns defined all outputs to low level.
       It translates to assignment in verilog */
    
    if (dump) printf ("Begin to read stopsignals\n");
    
    for (int i = 0; i < outputs.size(); i++)
    {
        SkipSpaces (output, deep);
        fprintf    (output, "%s = 0;\n", outputs [i].c_str());
    }
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::colon)) currentToken++;
    else return ParsingError (";");
    
    return true;
}

bool Translator::Transitto (FILE* output, int deep)
{
    currentToken++;
    
    /* This is a <transitto> command which 
       transits state machine to the defined.
       state. It translates to some assignments
       in verilog */
    
    if (dump) printf ("Begin to read transitto\n");
    
    if (!CheckName (tokeniser -> tokensBuffer [currentToken], "state", &states))
        return false;
    
    /* Left for history... */
    /* Before transition all output signals 
       have to be in low level. It may be changed
       in future. */ 
    
    /* for (int i = 0; i < outputs.size(); i++)
       {
           SkipSpaces (output, deep);
           fprintf    (output, "%s = 0;\n", outputs [i].c_str());
       } */
    
    
    
    /* Then last state's bit have to be assigned
       to 0 and new state's bit have to be assigned 
       to 1. */
    
    SkipSpaces (output, deep);
    fprintf    (output, "state [%d] = 0;\n", currentParsingState);
    
    int stateNumber = -1;
    for (int i = 0; i < states.size(); i++)
         if (tokeniser -> tokensBuffer [currentToken] -> name == states [i])
             stateNumber = i;
    
    SkipSpaces (output, deep);
    fprintf    (output, "state [%d] = 1;\n", stateNumber);
    
    currentToken++;
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::colon)) currentToken++;
    else return ParsingError (";");
    
    return true;
}