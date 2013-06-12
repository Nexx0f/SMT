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
    const int states = 0;
    
    const int start  = 100;
    const int end    = 101;
    const int comma  = 102;
    
    const int name   = 200;
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