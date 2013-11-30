#include <string>
#include <map>

#ifndef H_TOKENS
#define H_TOKENS

namespace TokenType
{
    const int keyword = 0;
    const int divider = 1;
    const int name    = 2;
};

namespace TokenSubtype
{
    const int states       = 0;
    const int inputs       = 1;
    const int outputs      = 2;
    const int switchType   = 3;
    const int state        = 4;
    const int emitSignal   = 5;
    const int ifType       = 6;
    const int transitto    = 7;
    const int stopSignal   = 8;
    const int stopSignals  = 9;
    const int transition   = 10;
    const int on           = 11;
    const int to           = 12;
    const int tokenDo      = 13;
    
    const int start        = 100;
    const int end          = 101;
    const int comma        = 102;
    const int leftBracket  = 103;
    const int rightBracket = 104;
    const int colon        = 105;
    
    const int name         = 200;
};

struct Key
{
    int type;
    int subtype;
    std::string name;
    std::string str;
};

class KeysData
{
public:
    std::map <std::string, Key> data;
    /* Key - str (In this string the name of token
     * that you can find in parsing code */
    
public:
         KeysData  ();

    Key  FindKey   (std::string str);
    bool pushKey   (Key newKey);
    bool isDivider (char firstLetter);
};

class Token
{
public:
    int         type;
    int         subtype;
    std::string name;
    
public:
                Token ();
                Token (int newType, int newSubtype, std::string newName);
};

#endif