
#ifndef _ILS_H_
#define _ILS_H_

#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>

#include "errors.h"
#include "parameters.h"
#include "localSearch.h"

void resizeMatrix(vector<int> & bestSolEver, vector<int> & currentSol, vector<int> & perturbSol) {
    bestSolEver.resize(instance.getNbJob()+1);
    currentSol.resize(instance.getNbJob()+1);
    perturbSol.resize(instance.getNbJob()+1);
}

/**
 * Test if the algorithm reached its termination criterion
 * 
 * @param[in] iteration Number of the current iteration
 * @param[in] max_iterations Maximum number of iterations
 * @param[out] result True if the algorithm should stop, false otherwise
 * @return false if one error occured, true otherwise
 */
bool updateILSTermination(int & iteration, int & max_iterations, chrono::duration<double, std::milli> & ms_double, int bestWCTever, int & target, double & timeLimit, bool & termination) {
    if (iteration < 0) {
        generateError("ils.h","updateILSTermination","iteration has a negative value","iteration",iteration);
        return false;
    }
    if (max_iterations <= 0) {
        generateError("ils.h","updateILSTermination","max_iterations should have a positive value","max_iterations",max_iterations);
        return false;
    }
    termination = false;
    if (iteration > max_iterations) {
        termination = true;
    }
    if (bestWCTever <= target) {
        termination = true;
    }
    if (ms_double.count() > timeLimit){
        termination = true;
    }
    return true;
}

/**
 * Operate a certain number of perturbation steps on a the currentSol (copy)
 * 
 * @param[in] nbPerturbationSteps Number of perturbation steps
 * @param[in] currentSol Solution to perturb
 * @param[out] perturbSol Resulting perturbed solution
 * @return false if one error occured, true otherwise
 */
bool perturbSolution(int & nbPerturbationSteps, vector<int> & currentSol, vector<int> & perturbSol) {
    replaceSolByNewSol(perturbSol,currentSol);
    int insertBegin;
    int insertEnd;
    for (int i = 1; i <= nbPerturbationSteps; i++) {
        insertBegin = round(rand()%(instance.getNbJob())+1);
        insertEnd = round(rand()%(instance.getNbJob())+1);
        if (!operateInsert(insertBegin,insertEnd,perturbSol,perturbSol,false)) {return false;}
    }
    return true;
}

/**
 * Operate an ILS on the global parameter solution
 * 
 * @param[in] readParams Wether or not the function has to parse the command parameters (false for the experiments)
 * @param[in] target Target solution quality, termination criteria
 * @param[in] timeLimit Termination criteria, ms
 * @return false if one error occured, true otherwise
 */
bool ILS_heuristic(bool & readParams, int & target, double & timeLimit) {

    auto start = chrono::high_resolution_clock::now();
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> ms_double;
    double timeComp = 0.0;

    int nbPerturbationSteps = 2;
    int max_iterations = 50;
    double lambda = 4.;

    vector<int> bestSolEver;
    vector<int> currentSol;
    vector<int> perturbSol;

    int bestWCTEver;
    int currentWCT;
    int perturbWCT;

    if (readParams) {
        if (!readILSArguments(nbPerturbationSteps,max_iterations,lambda)) { return false; }
    } else {
        setDefaultParameters(nbPerturbationSteps,max_iterations,lambda);
    }
    
    int sumTimes = 0;
    int timeBuffer;
    double randomDouble;

    for (int job = 1; job <= instance.getNbJob(); job ++) {
        for (int mac = 1; mac <= instance.getNbMac(); mac ++) {
            instance.getTime(job,mac,timeBuffer);
            sumTimes += timeBuffer;
        }
    }

    double temperature = lambda*((sumTimes)/(10*instance.getNbJob()*instance.getNbMac()));

    resizeMatrix(bestSolEver,currentSol,perturbSol);

    // INITIAL SOLUTION
    simplifiedRZ(bestSolEver,bestWCTEver);
    embeddedVND(bestSolEver,bestWCTEver);

    replaceSolByNewSol(currentSol,bestSolEver);

    currentWCT = bestWCTEver;

    int iteration = 1;
    int stagnate = 0;
    bool termination = false;

    while (!termination) {
        // Adaptative number of perturbation steps
        if (stagnate <= 5000/(float)instance.getNbJob()){
            nbPerturbationSteps = 2;
        }
        else if (stagnate > 5000/(float)instance.getNbJob() && stagnate <= 10000/(float)instance.getNbJob()){
            nbPerturbationSteps = 3;
            if (instance.getNbJob() == 100) {
                nbPerturbationSteps = 3;
            }            
        } else {
            nbPerturbationSteps = 4;
            if (instance.getNbJob() == 100) {
                nbPerturbationSteps = 4;
            }
        }

        // Perturb solution
        if (!perturbSolution(nbPerturbationSteps,currentSol,perturbSol)) { return false; }
        instance.computeWCT(instance.getNbJob(),perturbSol,perturbWCT);

        // Local search (VND)
        embeddedVND(perturbSol,perturbWCT);

        // Router
        if (perturbWCT < currentWCT) {
            if (!replaceSolByNewSol(currentSol,perturbSol)) {return false;}
            currentWCT = perturbWCT;
            if (perturbWCT < bestWCTEver) {
                if (!replaceSolByNewSol(bestSolEver,perturbSol)) {return false;}
                bestWCTEver = perturbWCT;
                
                end = chrono::high_resolution_clock::now();
                ms_double = end - start;

                cout << "new best : " << perturbWCT << ", it : " << iteration << ", stagnate = "<<stagnate<<", time = " << ms_double.count()/1000. << "s" << endl;
                
                stagnate = 0;
            }
        } else { // Non improving solution
            stagnate ++;

            randomDouble = (double) rand() / (RAND_MAX);
            if (randomDouble <= exp((currentWCT - perturbWCT)/temperature)) {
                if (!replaceSolByNewSol(currentSol,perturbSol)) {return false;}
                currentWCT = perturbWCT;
            }
        }

        iteration ++;
        end = chrono::high_resolution_clock::now();
        ms_double = end - start;

        // Test if the algorithm should stop
        updateILSTermination(iteration,max_iterations,ms_double,bestWCTEver,target,timeLimit,termination);
    }

    return true;
}

#endif