
/**************************************************
 * Declaration of global parameters and variables *
 **************************************************/

#ifndef _GLOBALVARIABLES_H_
#define _GLOBALVARIABLES_H_

#include <iostream>
#include <vector>

#include "pfspinstance.h"

using namespace std;

/// enums that will be used for global parameters
enum Pivoting_rule      { First_improvement, Best_improvement };
enum Neighborhood       { Transpose, Exchange, Insert };
enum Initial_solution   { Random_permutation, Simplified_RZ };
enum Algorithm          { II, VND };
enum Neighborhood_order { tr_ex_in, tr_in_ex };

enum SLS                { ACO, ILS };

/***********************************
 * Global parameters and variables *
 ***********************************/

Pivoting_rule pivoting_rule;
Neighborhood neighborhood;
Initial_solution initial_solution;
Algorithm algorithm;
Neighborhood_order neighborhood_order;
char * instance_file = NULL;
bool experiment = false;

SLS sls_global;
PfspInstance instance;
vector<int> solution;
int WCT;

int argc_global;
vector<string> argv_global;

/*****************************************************************
 * Overwriting the console output when printing the enums values *
 *****************************************************************/

std::ostream& operator<<(std::ostream& out, const Pivoting_rule value){
    const char* s = 0;
    #define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value){
        PROCESS_VAL(First_improvement);     
        PROCESS_VAL(Best_improvement);
    }
    #undef PROCESS_VAL
    return out << s;
}

std::ostream& operator<<(std::ostream& out, const Neighborhood value){
    const char* s = 0;
    #define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value){
        PROCESS_VAL(Transpose);     
        PROCESS_VAL(Exchange);
        PROCESS_VAL(Insert);
    }
    #undef PROCESS_VAL
    return out << s;
}

std::ostream& operator<<(std::ostream& out, const Initial_solution value){
    const char* s = 0;
    #define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value){
        PROCESS_VAL(Random_permutation);     
        PROCESS_VAL(Simplified_RZ);
    }
    #undef PROCESS_VAL
    return out << s;
}

std::ostream& operator<<(std::ostream& out, const Algorithm value){
    const char* s = 0;
    #define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value){
        PROCESS_VAL(II);     
        PROCESS_VAL(VND);
    }
    #undef PROCESS_VAL
    return out << s;
}

std::ostream& operator<<(std::ostream& out, const Neighborhood_order value){
    const char* s = 0;
    #define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value){
        PROCESS_VAL(tr_ex_in);     
        PROCESS_VAL(tr_in_ex);
    }
    #undef PROCESS_VAL
    return out << s;
}

std::ostream& operator<<(std::ostream& out, const SLS value){
    const char* s = 0;
    #define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value){
        PROCESS_VAL(ACO);     
        PROCESS_VAL(ILS);
    }
    #undef PROCESS_VAL
    return out << s;
}

#endif