
/*************************************************************
 * Functions for handeling file management, writing, reading *
 *************************************************************/

#ifndef _FILES_H_
#define _FILES_H_

#include <vector>
#include <fstream>
#include <cmath>

#include "pfspinstance.h"
#include "globalVariables.h"
#include "errors.h"

using namespace std;

/**
 * Constructs the name of the output file according to the global parameters
 * 
 * @param[out] fileName Name constructed
 * @return false if one error occured, true otherwise
 */
bool getFileName(string & fileName)
{
    fileName = "";
    fileName += "../output/";
    
    if (sls_global == ACO) {
        fileName += "aco/aco";
    } else {
        fileName += "ils/ils";
    }

    fileName += ".dat";

    return true;
}

/**
 * Open the file denoted by the given name and empty it
 * 
 * @param[in] fileName Name of the file to empty
 * @return false if one error occured, true otherwise
 */
bool emptyFile(char * fileName)
{
    string const myFile(fileName);
    ofstream myInitializer(myFile.c_str());

    if (myInitializer) {
        myInitializer << "";
        return true;
    }
    else {
        generateError("flowshop.cpp","emptyFile","impossible to open a file","file_name",fileName);
        return false;
    }
}

/**
 * Open the file denoted by the given name, append to it the given message and end line
 * 
 * @param[in] fileName Name of the file to write in
 * @param[in] message Message to append
 * @return false if one error occured, true otherwise
 */
bool appendToFile(char * fileName, string message)
{
    string const myFile(fileName);
    ofstream myStream(myFile.c_str(), ios::app);

    if (myStream) {
        myStream << message << endl;
        return true;
    }
    else {
        generateError("flowshop.cpp","main","impossible to open a file","file_name",fileName);
        return false;
    }
}

/**
 * Fill the given vectors with the names of the instances files and the names of the instances
 * 
 * @param[out] fileNames Names of the instances files
 * @param[out] instanceNames Names of the instances, useful to write it on the output files next to the BS and TIME
 * @return false if one error occured, true otherwise
 */
bool fillFileNames(vector<string> & fileNames,vector<string> & instanceNames,int & nbFiles, vector<int> & targets, double & timeLimit)
{
    string s1 = "../instances/100_20_";
    string s2 = "../instances/50_20_";
    string n1 = "100_20_";
    string n2 = "50_20_";

    double coef = 1.005;

    targets[0] = 595260;
    targets[1] = 622342;
    targets[2] = 592745;
    targets[3] = 666621;
    targets[4] = 653748;

    for (int i = 0; i < 5; i ++) {
        targets[i] = (int)ceil(targets[i]*coef);
    }

    timeLimit = 250*1000;

    if (true){
        nbFiles = 5;

        fileNames.resize(60);
        instanceNames.resize(60);

        fileNames[0] = "../instances/50_20_01";
        fileNames[1] = "../instances/50_20_02";
        fileNames[2] = "../instances/50_20_03";
        fileNames[3] = "../instances/50_20_04";
        fileNames[4] = "../instances/50_20_05";

        instanceNames[0] = "50_20_01";
        instanceNames[1] = "50_20_02";
        instanceNames[2] = "50_20_03";
        instanceNames[3] = "50_20_04";
        instanceNames[4] = "50_20_05";
    }

    return true;
}

#endif