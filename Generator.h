#include <vector>
#include "Conditions.h"

class Generator
{
    public:
    int statesQuantity;
    int inputsQuantity;
    int outputsQuantity;
    bool dump;
    
    std::vector <std::vector <ConditionalAction> > conditionalTransits;
    std::vector <std::vector <ConditionalAction> > conditionalOutputs;

         Generator (int initStatesQuantity,
                    int initInputsQuantity,
                    int initOutputsQuantity,
                    bool initDump, 
                    std::vector <std::vector <ConditionalAction> > initConditionalTransits,
                    std::vector <std::vector <ConditionalAction> > initConditionalOutputs);
    bool Generate                        (FILE* output);
    bool GenerateDeclaration             (FILE* output);
    bool GenerateVariablesInitialisation (FILE* output);
    bool GenerateTransitionBlock         (FILE* output);
    bool GenerateOutputAlwaysBlock       (FILE* output);
};