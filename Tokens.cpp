#include "Tokens.h"

Token::Token()
{
    type = -1;
    subtype = 0;
    name = "";
}

Token::Token(int newType, int newSubtype, std::string newName)
{
    type = newType;
    subtype = newSubtype;
    name = newName;
}

bool KeysData::pushKey(Key newKey)
{
    data [newKey.str] = newKey;
}

Key KeysData::FindKey(std::string str)
{
    std::map <std::string, Key>::const_iterator foundKey = data.find (str);
    if (foundKey != data.end())
    {
        return foundKey -> second;
    }
    else
    {
        Key key;
        key.subtype = TokenSubtype::name;
        key.type = TokenType::name;
        return key;
    }
}

KeysData::KeysData()
{
}

bool KeysData::isDivider(char firstLetter)
{
    char string [2] = "";
    string [0] = firstLetter;
    string [1] = 0;
    std::string stdStr (string);
    
    Key key = FindKey (stdStr);
    if (key.type == TokenType::divider) return true;
    else                                return false;
}
