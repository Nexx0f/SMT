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
    
    return TranslateMainBlocks (output);
}

bool Translator::TranslateMainBlocks(FILE* output)
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
            if (!Switch (output)) return false;
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

bool Translator::Switch(FILE* output)
{
    currentToken++;
    if (dump) printf ("Begin to read switch\n");
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::start)) currentToken++;
    else return ParsingError ("{");
    
    /*There is a start of translating main swith
      to verilog's module. It contains prototype
      of module, <state> variable initialisation, and begin
      of "always" block*/
    
    fprintf (output, "module state_machine (");
    
    if (dump) printf ("Translating inputs...  (");
        
    int space = strlen ("module state_machine (");
    for (int i = 0; i < inputs.size(); i++)
    {
        fprintf (output, "input  %s,\n", inputs [i].c_str());
        for (int i = 0; i < space; i++) fprintf (output, " "); 
            
        if (dump) printf  ("%s", inputs [i].c_str());
        if (i != inputs.size() - 1) 
            if (dump) printf (", ");
    }
    
    if (dump) printf (")\nTranslating outputs... (");
    
    for (int i = 0; i < outputs.size(); i++)
    {
        fprintf (output, "output %s", outputs [i].c_str());
        if (i != outputs.size() - 1)
        {
            fprintf (output, ",\n");
            for (int i = 0; i < space; i++) fprintf (output, " ");
        }
        
        if (dump) printf  ("%s", outputs [i].c_str());
        if (i != outputs.size() - 1) 
            if (dump) printf (", ");
    }
    
    if (dump) printf (")\n");
    fprintf (output, ");\n\n");
    
    /*Prototype of module was generated.
      Now initialisation of <state> variable*/
    
    fprintf (output, "reg [%d:0] state = 1;\n\n", states.size() - 1);
    if (dump) printf ("State wariable was initialised. "
                      "State machine has %d states. "
                      "It means that state variable will have %d bits.\n", 
                      states.size(), states.size());
    
    /*<state> varianle was initialised.
      Now begin of always block.*/
    
    fprintf (output, "always @("); 
    
    int secondSpace = strlen ("always @(");
    for (int i = 0; i < inputs.size(); i++)
    {
        fprintf (output, "%s", inputs [i].c_str());
        if (i != inputs.size() - 1)
        {
            fprintf (output, " or\n");
            for (int i = 0; i < secondSpace; i++)
                 fprintf (output, " ");
        }
    }
    fprintf (output, ")\nbegin\n");
    
    if (dump) printf ("Always block was inialised.\n"
                      "Recursive call of blocks translate function...\n");
    
    /*Begin of always block was written.
      Now recursive all of function <TranslateMainBlocks>.
      It have to read and translate all
      that switch contains*/
    
    if (!TranslateMainBlocks(output)) return false;
    
    /*There is end of generating file.
      It contains end of always block and
      <endmodule> keyword*/
    
    if (CheckCurrentToken (TokenType::divider, TokenSubtype::end)) currentToken++;
    else return ParsingError ("}");
    
    fprintf (output, "end\n\nendmodule");
}
