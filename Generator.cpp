#include "Generator.h"
#include <string.h>
#include <math.h>

#define SS(a) for(int i = 0; i < a; i++) fprintf (output, " ");

/* TODO (1): sort conditions. Index of the transitional
 *           conditional action (<do> block) have to be lower than others.
 *           Or in some cases generated code will be incorrect */

/* TODO (2): if's cases have do be check3ed with all burned inputs 
 *           and posedge of any burned input or negedge of any input */

/* TODO (3): Parse multiple conditions */

Generator::Generator(int initStatesQuantity,
                     int initInputsQuantity,
                     int initOutputsQuantity,
                     bool initDump,
                     std::vector< std::vector <ConditionalAction> > initConditionalTransits,
                     std::vector< std::vector <ConditionalAction> > initConditionalOutputs):
    statesQuantity      (initStatesQuantity),
    inputsQuantity      (initInputsQuantity),
    outputsQuantity     (initOutputsQuantity),
    dump                (initDump),
    conditionalOutputs  (initConditionalOutputs),
    conditionalTransits (initConditionalTransits)
{
}

bool Generator::Generate (FILE* output)
{
    if (dump) printf ("\n\nGenerating module declaration...\n");
    GenerateDeclaration (output);
    
    if (dump) printf ("Generating variables initialisation...\n");
    GenerateVariablesInitialisation (output);
    
    if (dump) printf ("Generating transition always block...\n");
    GenerateTransitionBlock (output);
    
    if (dump) printf ("Generating output always block... \n");
    GenerateOutputAlwaysBlock (output);
    
    fprintf (output, "\n"
                     "endmodule");
}

bool Generator::GenerateDeclaration (FILE* output)
{
    int space = strlen ("module state_machine (");
    fprintf (output, "module state_machine (input wire clk,\n");
    
    SS (space);
    if (dump) printf ("    Generating input bus declaration. Bus size - %d\n", inputsQuantity);
    fprintf (output, "input wire inputs  [%d:0],\n", inputsQuantity-1);
    
    SS (space);
    if (dump) printf ("    Generating output bus declaration. Bus size - %d\n", outputsQuantity);
    fprintf (output, "output reg outputs [%d:0]);", outputsQuantity-1);
}

bool Generator::GenerateVariablesInitialisation (FILE* output)
{
    /* Generating <state> and <last_state> integers and size
     * of it's bus (next power of 2 after states quantity) */
    
    int busSize = 0;
   
    for (int i = 1; i < statesQuantity; i *= 2) 
         busSize++;
    
    if (dump) printf ("    <state> bus size is %d for %d states\n", busSize, statesQuantity);
    fprintf (output, "\n\n");
    fprintf (output, "reg [%d:0] state;\n", busSize-1);
    
    if (dump) printf ("    <last_state> bus size is %d for %d states\n", busSize, statesQuantity);
    fprintf (output, "\n\n");
    fprintf (output, "reg [%d:0] last_state;\n", busSize-1);
    
    /* | Every bit of this bus has positive edge on next
     * | clk's positive edge after positive edge 
     * v of corresponding bit of <inputs> */
    
    if (dump) printf ("    <posedge_inputs> bus size is %d\n", inputsQuantity);
    fprintf (output, "wire [%d:0] posedge_inputs;\n", inputsQuantity-1);
    
    /* | Every bit of this bus has positive edge on next
     * | clk's positive edge after negative edge 
     * v of corresponding bit of <inputs> */
    
    if (dump) printf ("    <negedge_inputs> bus size is %d\n", inputsQuantity);
    fprintf (output, "wire [%d:0] negedge_inputs;\n", inputsQuantity-1);
    
    /* |  This is bus that saves value of 
     * |  <inputs> on last clk positive edge. 
     * v  I need this to detect positive edge of <inputs> */
    
    if (dump) printf ("    <previous_inputs> bus size if %d\n", inputsQuantity);
    fprintf (output, "reg [%d:0] previous_inputs;", inputsQuantity-1);
    
    /* | This not synthesiable construction,
     * v but it's helpful for simulation */
    
    if (dump) printf ("    Initialising <state> and <previous_inputs> to 0\n");
    fprintf (output, "\n\n");
    fprintf (output, "initial\n"
                     "begin\n"
                     "        state           <= 0;\n"
                     "        last_state      <= 0;\n"
                     "        previous_inputs <= 0;\n"
                     "end\n\n");
    
    /* | Now all <posedge_inputs> wires have to be assigned
     * v to some function of <previous_inputs> and <inputs> */
    
    for (int i = 0; i < inputsQuantity; i++)
    {
        if (dump) printf ("    Assigning <posedge_inputs[%d]> = ~<previous_inputs[%d] && inputs[%d]\n", i, i, i);
        fprintf (output, "assign posedge_inputs[%d] = ~previous_inputs[%d] && inputs[%d];\n", i, i, i);
    }
    
    /* | Now all <negedge_inputs> wires have to be assigned
     * v to some function of <previous_inputs> and <inputs> */
    
    if (dump) printf ("\n");
    fprintf (output, "\n");
    for (int i = 0; i < inputsQuantity; i++)
    {
        if (dump) printf ("    Assigning <negedge_inputs[%d]> = <previous_inputs[%d] && ~inputs[%d]\n", i, i, i);
        fprintf (output, "assign negedge_inputs[%d] = previous_inputs[%d] && ~inputs[%d];\n", i, i, i);
    }
}

bool Generator::GenerateTransitionBlock(FILE* output)
{
    if (dump) printf ("    Initialising always block for <posedge clk>\n");
    fprintf (output,  "\n");
    fprintf (output,  "always @(posedge clk)\n"
                      "begin\n");
    
    /* | First we have to refresh values
     * v of <previous_inputs> bus */
    
    for (int i = 0; i < inputsQuantity; i++)
    {
        if (dump) printf ("    <previous_inputs[%d]> will always be refreshed on positive edge of clk\n", i);
        fprintf (output, "        previous_inputs[%d] <= inputs[%d];\n", i, i);
    }
    
    /* | Now we have to generate transition
     * v cases depending of state */
    
    if (dump) printf ("    Generating <state> cases\n");
    fprintf (output, "        case (state)\n");
    
    for (int state = 0; state < statesQuantity; state++)
    {
        if (dump) printf ("        Generating case for state %d\n", state);
        fprintf (output, "            %d:\n"
                         "            begin\n", state);
        
        
        for (int conditionNumber = 0; conditionNumber < conditionalTransits[state].size(); conditionNumber++)
        {
            if (dump) printf ("            Generating transition from state %d to state %d with conditions ", 
                              state, conditionalTransits[state][conditionNumber].action);
            if (conditionalTransits[state][conditionNumber].condition.subConditions.size() != 0)
            {
                fprintf (output, "                if (");
                
                /* First condition is all needed conditional inputs in burning */
                
                fprintf (output, "(");
                for (int inputNumber = 0; 
                     inputNumber < conditionalTransits[state][conditionNumber].condition.subConditions.size(); 
                     inputNumber++)
                {
                    if (inputNumber != conditionalTransits[state][conditionNumber].condition.subConditions.size() - 1) 
                    {
                        if (dump) printf ("%d, ", conditionalTransits[state][conditionNumber].condition.subConditions[inputNumber]);
                        fprintf (output, "inputs [%d] && ", 
                                conditionalTransits[state][conditionNumber].condition.subConditions[inputNumber]);
                    }
                    else 
                    {
                        if (dump) printf ("%d", conditionalTransits[state][conditionNumber].condition.subConditions[inputNumber]);
                        fprintf (output, "inputs [%d]", 
                                 conditionalTransits[state][conditionNumber].condition.subConditions[inputNumber]);
                    }
                }
                fprintf (output, ") ");
                
                /* Second block is any posedge conditional or any negedge not conditional */
            
                fprintf (output, "&&\n"
                                 "                    (");
                for (int inputNumber = 0; inputNumber < inputsQuantity; inputNumber++)
                {
                    bool conditional = false;
                    for (int i = 0; i < conditionalTransits[state][conditionNumber].condition.subConditions.size(); i++)
                          if (inputNumber == conditionalTransits[state][conditionNumber].condition.subConditions[i])
                              conditional = true;
                          
                    if (conditional) 
                         fprintf (output, "posedge_inputs [%d]", inputNumber);
                    else fprintf (output, "negedge_inputs [%d]", inputNumber);
                    
                    if (inputNumber != inputsQuantity-1)
                         fprintf (output, " || ");
                }
                fprintf (output, "))");
            }
                
            if (dump) printf (".\n");
            fprintf (output, "\n"
                             "                begin\n"
                             "                        last_state <= state;\n"
                             "                        state = %d;\n"
                             "                end\n", conditionalTransits[state][conditionNumber].action);
        }
        
        fprintf (output, "            end\n");
    }
    fprintf (output, "        endcase\n");
        
    if (dump) printf ("Closing always block for <posedge clk> (transition always block)\n");
    fprintf (output, "end\n");
    
}

bool Generator::GenerateOutputAlwaysBlock(FILE* output)
{
    if (dump) printf ("    Initialising output always block for <state> changing\n");
    fprintf (output,  "\n");
    fprintf (output,  "always @(state)\n"
                      "begin\n");
    
    /* | Now we have to generate output
    * v cases depending of state */
    
    if (dump) printf ("    Generating <state> cases\n");
    fprintf (output, "        case (state)\n");    
    
    for (int state = 0; state < conditionalOutputs.size(); state++)
    {
        if (dump) printf ("        Generating case for state %d\n", state);
        fprintf (output, "            %d:\n"
                         "            begin\n", state);
        
        for (int conditionNumber = 0; conditionNumber < conditionalOutputs[state].size(); conditionNumber++)
        {
            if (dump) printf ("            Generating output case on state %d with conditions ", state);
            
            fprintf (output, "                ");
            
            if (conditionalOutputs[state][conditionNumber].condition.subConditions.size() != 0)
            {
                fprintf (output, "if (");
                
                /* First conditional block contains checking of all need inputs 
                 * are burned and checking of last state if this is transitional conditional action */
                
                fprintf (output, "(");
                for (int inputNumber = 0; 
                    inputNumber < conditionalOutputs[state][conditionNumber].condition.subConditions.size(); 
                    inputNumber++)
                {
                    if (inputNumber != conditionalOutputs[state][conditionNumber].condition.subConditions.size() - 1) 
                    {
                        if (dump) printf ("%d, ", conditionalOutputs[state][conditionNumber].condition.subConditions[inputNumber]);
                        fprintf (output, "inputs [%d] && ", 
                                conditionalOutputs[state][conditionNumber].condition.subConditions[inputNumber]);
                    }
                    else 
                    {
                        if (dump) printf ("%d", conditionalOutputs[state][conditionNumber].condition.subConditions[inputNumber]);
                        fprintf (output, "inputs [%d]", 
                                conditionalOutputs[state][conditionNumber].condition.subConditions[inputNumber]);
                    }                
                }
                
                if (conditionalOutputs[state][conditionNumber].condition.onState)
                {
                    if (dump) printf (", last_state == %d", conditionalOutputs[state][conditionNumber].condition.transitionState);
                    fprintf (output, " && last_state == %d", conditionalOutputs[state][conditionNumber].condition.transitionState);
                }
                
                fprintf (output, ") ");
                
                /* Second block is any posedge conditional or any negedge not conditional */
            
                fprintf (output, "&&\n"
                                 "                    (");
                for (int inputNumber = 0; inputNumber < inputsQuantity; inputNumber++)
                {
                    bool conditional = false;
                    for (int i = 0; i < conditionalOutputs[state][conditionNumber].condition.subConditions.size(); i++)
                          if (inputNumber == conditionalOutputs[state][conditionNumber].condition.subConditions[i])
                              conditional = true;
                          
                    if (conditional) 
                         fprintf (output, "posedge_inputs [%d]", inputNumber);
                    else fprintf (output, "negedge_inputs [%d]", inputNumber);
                    
                    if (inputNumber != inputsQuantity-1)
                         fprintf (output, " || ");
                }
                fprintf (output, ")) ");
            }
            
            if (conditionalOutputs[state][conditionNumber].actionType == ActionType::allSignalsStopping)
            {
                fprintf (output, "\n"
                         "                begin\n");
                for (int i = 0; i < outputsQuantity; i++)
                     fprintf (output, "                    outputs [%d] <= 0;\n", i);
                fprintf (output, "                end\n");
            }
            else
            if (conditionalOutputs[state][conditionNumber].actionType == ActionType::signalStopping)
                fprintf (output, "outputs [%d] <= 0;", conditionalOutputs[state][conditionNumber].action);
            else
            if (conditionalOutputs[state][conditionNumber].actionType == ActionType::signalEmitting)
                fprintf (output, "outputs [%d] <= 1;", conditionalOutputs[state][conditionNumber].action);
            
            fprintf (output, "\n");
            if (dump) printf ("\n");
            
        }
        fprintf (output, "            end\n");
    }
    
    fprintf (output, "        endcase\n");
    
    if (dump) printf ("Closing always block for <state> changing (output always block)\n");
    fprintf (output, "end\n");
}
