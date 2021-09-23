
/********************************************************************
 * Functions for handeling parameters parsing, setting and printing *
 ********************************************************************/

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include <iostream>
#include <string.h>

#include "globalVariables.h"
#include "pfspinstance.h"
#include "errors.h"

/**
 * Print global parameters values
 */
void printParameters()
{
    if (experiment) {
        if (instance_file != NULL) {
            cout << "PFSP parameters:\n"
            << "   experiment : true\n"
            << "   instance : "<< instance_file << "\n"
            << "   algorithm : "<< algorithm << "\n"
            << endl;

            cout << "   <experiment - all the other parameters are ignored>" << endl << endl;
        }
        cout << "PFSP parameters:\n"
            << "   experiment : true\n"
            << "   algorithm : "<< algorithm << "\n"
            << endl;

        cout << "   <experiment - all the other parameters are ignored>" << endl << endl;
    }
    else if (algorithm == II) {
        cout << "PFSP parameters:\n"
            << "   algorithm : " << algorithm << "\n"
            << "   pivoting_rule : "  << pivoting_rule << "\n"
            << "   neighborhood : " << neighborhood << "\n"
            << "   initial_solution : "  << initial_solution << "\n"
            << "   instance : " << instance_file << "\n"
            << endl;
        cout << "   <algorithm II - the neighborhood_order parameter is ignored>" << endl << endl;
    } else {
        cout << "PFSP parameters:\n"
            << "   algorithm : " << algorithm << "\n"
            << "   pivoting_rule : "  << pivoting_rule << "\n"
            << "   neighborhood_order : " << neighborhood_order << "\n"
            << "   initial_solution : "  << initial_solution << "\n"
            << "   instance : " << instance_file << "\n"
            << endl;
        cout << "   <algorithm VND - the neighborhood parameter is ignored>" << endl << endl;
    }
}

/**
 * Initialize global parameters with default values
 */
void setDefaultParameters()
{
	pivoting_rule = First_improvement;
	neighborhood = Transpose;
    neighborhood_order = tr_ex_in;
	initial_solution = Random_permutation;
    algorithm = II;
    experiment = false;
}

/**
 * Initialize ACO parameters with default values
 */
void setDefaultParameters(double & rho, int & max_iterations)
{
	rho = 0.75;
    max_iterations = 50;
}

/**
 * Initialize ILS parameters with default values
 */
void setDefaultParameters(int & nbPerturbationSteps, int & max_iterations, double & lambda)
{
	nbPerturbationSteps = 2;
    max_iterations = 50;
    lambda = 4.;
}

/**
 * Read SLS argument from execution command line and assign global parameters sls_global, argc_global and argv_global
 * 
 * @param argc Number of parameters
 * @param argv Parameters
 * @return false if one error occured, true otherwise
 */
bool readSLS(int argc, char *argv[])
{
    if (argc == 1) {
        return false;
    }

    if(strcmp(argv[1], "--SLS") == 0){
        if (strcmp(argv[2], "ACO") == 0){
            sls_global = ACO;
        }
        else if (strcmp(argv[2], "ILS") == 0) {
            sls_global = ILS;
        }
        else {
            generateError("parameters.h","readArguments","SLS parameter not recognized","sls",argv[2]);
            return false;
        }
    } else {
        generateError("parameters.h","readArguments","The first parameter should be --SLS");
        return false;
    }

    argc_global = argc;
    argv_global.resize(argc);
    
    for (int i = 1; i < argc; i++) {
        argv_global[i] = argv[i];
    }

	return true;
}

bool readACOArguments(double & rho, int & max_iterations) {

	setDefaultParameters(rho,max_iterations);

	for(int i=3; i< argc_global ; i++){
		if(argv_global[i] == "--rho") {
			rho = stod(argv_global[i+1]);
			i++;
		} else if(argv_global[i] == "--max_iterations") {
			max_iterations = stoi(argv_global[i+1]);
			i++;
        } else if(argv_global[i] == "--instance_file") {
            instance_file = new char [argv_global[i+1].size()];
            strcpy(instance_file,argv_global[i+1].c_str());
            cout << endl;
            i++;
		} else {
            generateError("parameters.h","readACOArguments","Parameter not recognised","Parameter",argv_global[i]);
            help();
			return false;
		}
	}

    if (instance_file == NULL) {
        generateError("parameters.h","readACOArguments","Instance has to be specified");
        return false;
    } else {
        if (!instance.readDataFromFile(instance_file)) {
            return false;
        }
    }

	return(true);
}

bool readILSArguments(int & nbPerturbationSteps, int & max_iterations, double & lambda) {

	setDefaultParameters(nbPerturbationSteps, max_iterations, lambda);

	for(int i=3; i< argc_global ; i++){
		if(argv_global[i] == "--nbPerturbationSteps"){
			nbPerturbationSteps = stoi(argv_global[i+1]);
			i++;
		} else if(argv_global[i] == "--max_iterations"){
			max_iterations = stoi(argv_global[i+1]);
			i++;
        } else if(argv_global[i] == "--lambda"){
            lambda = stod(argv_global[i+1]);
            i++;
		} else if(argv_global[i] == "--instance_file") {
            instance_file = new char [argv_global[i+1].size()];
            strcpy(instance_file,argv_global[i+1].c_str());
            cout << endl;
            i++;
		} else {
            generateError("parameters.h","readILSArguments","Parameter not recognised","Parameter",argv_global[i]);
            help();
			return false;
		}
	}

    if (instance_file == NULL) {
        generateError("parameters.h","readILSArguments","Instance has to be specified");
        return false;
    } else {
        if (!instance.readDataFromFile(instance_file)) {
            return false;
        }
    }

	return true;
}

#endif