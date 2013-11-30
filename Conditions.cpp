#include "Conditions.h"

Condition::Condition ()
{
    onState = false;
    transitionState = -1;
}

void Condition::PushSubCondition (int input)
{
    subConditions.push_back (input);
}

bool Condition::PushStateCondition(int state)
{
    if (transitionState != -1)
    {
        printf ("\n========> Error: Multiple transition enclosure <========\n");
        return false;
    }
    
    onState = true;
    transitionState = state;
    return true;
}

int Condition::PopSubCondition()
{
    if (!subConditions.empty())
    {
        int lastSubCondition = subConditions [subConditions.size()-1];
        subConditions.pop_back ();
        return lastSubCondition;
    }
}

void Condition::DumpCondition()
{
    for (int i = 0; i < subConditions.size(); i++)
    {
        if (i != subConditions.size()-1) printf ("%d, ", subConditions[i]);
        else                             printf ("%d", subConditions[i]);
    }
}

ConditionalAction::ConditionalAction(Condition newCondition, int newAction, int newActionType):
    condition  (newCondition),
    action     (newAction),
    actionType (newActionType)
{
}

void ConditionalAction::DumpConditionalAction(std::vector <std::string>* namesData)
{
    std::string typeName;
    if (actionType == ActionType::transition) typeName = "transition to";
    else
    if (actionType == ActionType::signalEmitting) typeName = "Signal emitting";
    else
    if (actionType == ActionType::signalStopping) typeName = "Signal stopping";
    else
    if (actionType == ActionType::allSignalsStopping) typeName = "stop every signal";
    
        
    printf (" {");
    condition.DumpCondition();
    if (actionType != ActionType::allSignalsStopping) 
        printf ("} for %s <%d>\n", typeName.c_str(), action);
    else printf ("} for %s\n", typeName.c_str());
}