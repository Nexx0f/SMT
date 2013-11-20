#include <vector>
#include <string>
#include <stdio.h>

namespace ActionType
{
    const int transition         = 0;
    const int signalEmitting     = 1;
    const int signalStopping     = 2;
    const int allSignalsStopping = 3;
};

class Condition
{
    public:
    std::vector <int> subConditions;
    
                Condition ();
    void        PushSubCondition (int input);
    int         PopSubCondition  ();
    void        DumpCondition ();
};

class ConditionalAction
{
    public:
    Condition condition;
    int       action;
    int       actionType;
    
    public:
         ConditionalAction     (Condition newCondition, int newAction, int newActionType);
    void DumpConditionalAction (std::vector <std::string>* namesData);
};