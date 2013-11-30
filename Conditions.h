#include <vector>
#include <string>
#include <stdio.h>

#ifndef H_CONDITIONS
#define H_CONDITIONS

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
    bool              onState;
    int               transitionState;
    
                Condition          ();
    void        PushSubCondition   (int input);
    bool        PushStateCondition (int state);
    int         PopSubCondition    ();
    void        DumpCondition      ();
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

#endif