
/*************************************************************************************
 *                                   EXPERIMENTATION                                 *
 *************************************************************************************/

#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H_

#include <iostream>

#include <vector>
#include <string.h>
#include <string>
#include <chrono>

#include <unistd.h>

#include "globalVariables.h"
#include "files.h"
#include "aco.h"
#include "ils.h"

// RTD ANALYSIS
bool runExperimentTwo() {
    auto start = chrono::high_resolution_clock::now();
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> ms_double;

    vector<string> fileNames;
    vector<string> instanceNames;

    vector<int> TIME_vect (25);

    int nbFiles = 0;
    double nbMS100 = 0.;
    double nbMS50 = 0.;
    bool readParams = false;
    vector<int> targets (5);
    double timeLimit = 0.;

    fillFileNames(fileNames,instanceNames,nbFiles,targets,timeLimit);

    string fileName;
    char * cfileName;

    /// update the name of the file we'll write on
    getFileName(fileName);
    cfileName = &fileName[0];
    
    emptyFile(cfileName);

    cout << "\nfileName = " << fileName << endl << endl;

    string line = "";

    for (int i = 0; i < nbFiles; i++) {
        instance_file = &fileNames[i][0];
        cout << instance_file << endl;

        line = "";
        line += instanceNames[i];

        /// Read data from file
        if (!instance.readDataFromFile(instance_file)) {
            return false;
        }

        for (int run = 0; run < 25; run ++) {
            cout << "run : " << run << endl;
            /// initialize random seed
            srand ( i+60*run);

            start = chrono::high_resolution_clock::now();

            if (sls_global == ACO) {
                if (!ACO_heuristic(readParams,targets[i],timeLimit)) {
                    return false;
                }
            } else {
                if (!ILS_heuristic(readParams,targets[i],timeLimit)) {
                    return false;
                }
            }

            end = chrono::high_resolution_clock::now();
            ms_double = end - start;

            TIME_vect[run] = ms_double.count();

            line += ", " + to_string(TIME_vect[run]);
        }

        appendToFile(cfileName,line);
    }

    return true;
}

// PERCENTAGE DEVIATION ANALYSIS -> doesn't fit to the actual code

/*bool runExperimentOne() {
    auto start = chrono::high_resolution_clock::now();
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> ms_double;
    double timeComp = 0.0;

    vector<string> fileNames;
    vector<string> instanceNames;

    vector<int> WCT_vect (5);

    int nbFiles = 0;
    double nbMS100 = 0.;
    double nbMS50 = 0.;
    bool readParams = false;

    fillFileNames(fileNames,instanceNames,nbFiles,nbMS100,nbMS50);

    string fileName;
    char * cfileName;

    /// update the name of the file we'll write on
    getFileName(fileName);
    cfileName = &fileName[0];
    
    emptyFile(cfileName);
    appendToFile(cfileName,"Problem , BS1, BS2, BS3, BS4, BS5");

    cout << "\nfileName = " << fileName << endl << endl;

    for (int i = 0; i < nbFiles; i++) {
        instance_file = &fileNames[i][0];
        cout << instance_file << endl;

        /// Read data from file
        if (!instance.readDataFromFile(instance_file)) {
            return false;
        }

        for (int run = 1; run <= 5; run ++) {
            /// initialize random seed
            srand ( i+60*run);

            start = chrono::high_resolution_clock::now();

            if (sls_global == ACO) {
                if (!ACO_heuristic(readParams,nbMS100,nbMS50)) {
                    return false;
                }
            } else {
                if (!ILS_heuristic(readParams,nbMS100,nbMS50)) {
                    return false;
                }
            }

            end = chrono::high_resolution_clock::now();
            ms_double = end - start;
            timeComp = ms_double.count();

            WCT_vect[run] = WCT;

            cout << "WCT : " << WCT << endl;

            sleep(50);
        }

        appendToFile(cfileName,instanceNames[i] + " , " + to_string(WCT_vect[1]) + " , " + to_string(WCT_vect[2]) + " , " + to_string(WCT_vect[3]) + " , " + to_string(WCT_vect[4]) + " , " + to_string(WCT_vect[5]));
    }

    return true;
}*/

#endif