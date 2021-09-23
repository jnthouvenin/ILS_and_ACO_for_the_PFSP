
#ifndef _ACO_H_
#define _ACO_H_

#include <iostream>
#include <vector>
#include <math.h>
#include <bits/stdc++.h>

#include "errors.h"
#include "parameters.h"
#include "localSearch.h"

using namespace std;

void resizeMatrix(vector<vector<double>> & pheromones, vector<vector<int>> & antSolution, vector<int> & antWCT, vector<int> & bestSolEver) {
    bestSolEver.resize(instance.getNbJob()+1);
    antSolution.resize(2);
    antWCT.resize(2);
    pheromones.resize(instance.getNbJob()+1);
    for (int i = 1; i <= 1; i++) {
        antSolution[i].resize(instance.getNbJob()+1);
    }
    for (int i = 1; i <= instance.getNbJob(); i++) {
        pheromones[i].resize(instance.getNbJob()+1);
    }
}

/**
 * Fill the matrix of pheromones with the initial amount of pheromones
 * 
 * @param[out] pheromones Matrix of pheromones
 * @param[in] bestWCTInit 1/(Amount of pheromones)
 */
void initPheromones(vector<vector<double>> & pheromones, int & bestWCTInit) {
    double phiInit = 1/ ((double)bestWCTInit);
    for (int i = 1; i <= instance.getNbJob(); i++) {
        for (int j = i; j <= instance.getNbJob(); j++) {
            pheromones[i][j] = phiInit;
            pheromones[j][i] = phiInit;
        }
    }
}

/**
 * Test if the algorithm reached its termination criterion
 * 
 * @param[in] iteration Number of the current iteration
 * @param[in] max_iterations Maximum number of iterations
 * @param[in] ms_double Number of ms spent on ACO
 * @param[in] bestWCTever Self explanatory
 * @param[in] target Quality solution that the algorithm wants to reach
 * @param[in] timeLimit in ms
 * @param[out] termination True if the algorithm should stop, false otherwise
 * @return false if one error occured, true otherwise
 */
bool updateTermination(int & iteration, int & max_iterations, chrono::duration<double, std::milli> & ms_double, int bestWCTever, int & target, double & timeLimit, bool & termination) {
    if (iteration < 0) {
        generateError("aco.h","updateTermination","iteration has a negative value","iteration",iteration);
        return false;
    }
    if (max_iterations <= 0) {
        generateError("aco.h","updateTermination","max_iterations should have a positive value","max_iterations",max_iterations);
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
 * Update pheromones
 * 
 * @param[in] bestSolEver best solution found so far
 * @param[in] antSolution solution constructed by the ant
 * @param[in] antWCT wct associated with the ant solution
 * @param[in] rho evaporation rate
 * @param[in,out] pheromones matrix of pheromones
 * @return false if one error occured, true otherwise
 */
bool updatePheromones(vector<int> & bestSolEver, vector<vector<int>> & antSolution, vector<int> & antWCT, double & rho, vector<vector<double>> & pheromones) {
    int jobAnt;
    double diffJobAnt;
    int oldPlaceJob;

    int itWhile;
    bool endWhile;

    // Evaporation
    for (int i = 1; i<= instance.getNbJob(); i++) {
        for (int j = 1; j <= instance.getNbJob(); j++) {
            pheromones[i][j] = rho*pheromones[i][j];
        }
    }

    // Reward components of the new solution
    for (int i = 1; i <= instance.getNbJob(); i++) {
        jobAnt = antSolution[1][i];
        
        itWhile = 1;
        endWhile = false;

        while (!endWhile && itWhile <= instance.getNbJob()) {
            if (bestSolEver[itWhile] == jobAnt) {
                oldPlaceJob = itWhile;
                endWhile = true;
            }
            itWhile ++;
        }
        diffJobAnt = pow(abs(i-oldPlaceJob) + 1,0.5);
        pheromones[jobAnt][i] += 1/(diffJobAnt*antWCT[1]);
    }
    return true;
}

/**
 * Construct a new solution, using the matrix of pheromones and the best solution found so far
 * The pseudo-code of this function is presented in the report
 * 
 * @param[in] pheromones matrix of pheromones
 * @param[in] bestSolEver best solution found so far
 * @param[in] ant index of the ant (here always 1)
 * @param[out] antSolution constructed solution
 * @param[out] antWCT wct associated with the constructed solution
 * @return false if one error occured, true otherwise
 */
bool constructSolution(vector<vector<double>> & pheromones, vector<int> bestSolEver, int & ant, vector<vector<int>> & antSolution, vector<int> & antWCT) {
    /// random numbers
    double randomDouble;
    double randomDouble_2;
    /// while variables
    bool endWhile;
    int itWhile;
    /// memorize the chosen job
    int chosenJob;
    int indexChosenJob;

    /// size of the set (<= 5)
    int sizeOfSet;
    /// set of the indicies of the jobs
    vector<int> itInSet (6);
    /// set of the values T of the jobs
    vector<double> T (6);
    /// variables on T
    double Tmax = 0.;
    double sumT;
    double sum;
    
    for (int indexJob = 1; indexJob <= instance.getNbJob(); indexJob++) {
        randomDouble = (double) rand() / (RAND_MAX);
        if (randomDouble < 0 || randomDouble > 1) {
            generateError("aco.h","constructSolution","randomDouble out of range","randomDouble",randomDouble);
            return false;
        }
        if (randomDouble <= 0.4) {
            // parcourir bestSolEver, des qu'on a pas un zero (job not used) on l'ajoute à notre solution.
            endWhile = false;
            itWhile = 1;
            while(!endWhile && itWhile <= instance.getNbJob()) {
                if (bestSolEver[itWhile] > 0) {
                    chosenJob = bestSolEver[itWhile];
                    bestSolEver[itWhile] = 0;
                    endWhile = true;
                }
                if (!endWhile) {itWhile++;}
            }
            if (itWhile > instance.getNbJob()) {
                generateError("aco.h","constructSolution","itWhile out of bound, bestSolEver full of zeros");
                return false;
            }
        } else {
            /// creating the set of unscheduled jobs, of size sizeOfSet (<= 5)
            endWhile = false;
            itWhile = 1;

            sizeOfSet = 0;
            while(!endWhile && itWhile <= instance.getNbJob()) {
                if (bestSolEver[itWhile] > 0) {
                    sizeOfSet ++;
                    itInSet[sizeOfSet] = itWhile;
                    if (sizeOfSet == 5) {
                        endWhile = true;
                    }
                }
                itWhile++;
            } /// itInSet[1:sizeOfSet] are the indicies of the first sizeOfSet jobs unscheduled in bestSolEver.

            sumT = 0.;
            Tmax = 0.;
            /// computing for each job in the set its T value, and computing the sum of T and computing the greatest value of T
            for (int i = 1; i <= sizeOfSet; i++) {
                T[i] = 0.;
                for (int j = 1; j <= indexJob; j++) {
                    T[i] += pheromones[bestSolEver[itInSet[i]]][j];
                }
                sumT += T[i];
                if (T[i] > Tmax) {
                    Tmax = T[i];
                    indexChosenJob = itInSet[i];
                    chosenJob = bestSolEver[indexChosenJob];
                }
            } /// T[1:sizeOfSet] are the T of the first sizeOfSet jobs unscheduled in bestSolEver.

            if (randomDouble <= 0.8) {
                /// we already chose the job, we just have to signal that it is scheduled now
                bestSolEver[indexChosenJob] = 0;
            } else {
                randomDouble_2 = (double) rand() / (RAND_MAX);

                if (randomDouble < 0 || randomDouble > 1) {
                    generateError("aco.h","constructSolution","randomDouble_2 out of range","randomDouble_2",randomDouble_2);
                    return false;
                }

                itWhile = 1;
                endWhile = false;

                sum = 0.;
                while (!endWhile && itWhile <= sizeOfSet) {
                    sum += T[itWhile];
                    if (sum/sumT >= randomDouble_2) {
                        chosenJob = bestSolEver[itInSet[itWhile]];
                        bestSolEver[itInSet[itWhile]] = 0;
                        endWhile = true;
                    }
                    itWhile ++;
                }
            }
        }
        antSolution[ant][indexJob] = chosenJob;
    }
    instance.computeWCT(instance.getNbJob(), antSolution[ant], antWCT[ant]);
    return true;
}

/**
 * Disturb the matrix of pheromones randomly
 * 
 * @param[in,out] pheromones the matrix of pheromones
 */
void disturbPheromones(vector<vector<double>> & pheromones) {
    double randomDouble;
    double maxPhi = 0;
    double alpha = 0.2;
    double beta = 0.8;

    // compute the maximum level of pheromones
    for (int i = 1; i <= instance.getNbJob(); i++) {
        for (int j= 1; j <= instance.getNbJob(); j++) {
            if (maxPhi < pheromones[i][j]){
                maxPhi = pheromones[i][j];
            }
            pheromones[i][j] += 1;
        }
    }

    maxPhi ++; // update pheromones maximum

    // randomize the matrix of pheromones
    for (int i = 1; i <= instance.getNbJob(); i++) {
        for (int j= 1; j <= instance.getNbJob(); j++) {
            randomDouble = (double) rand() / (RAND_MAX);
            if (randomDouble <= 1) {
                randomDouble = ((double) rand() / (RAND_MAX))*maxPhi;
                pheromones[i][j] = randomDouble*alpha+pheromones[i][j]*beta;
            }
        }
    }
}

/**
 * modify global solution according to the ACO heuristic
 * 
 * @return false if one error occured, true otherwise
 */
bool ACO_heuristic(bool & readParams, int & target, double & timeLimit) {
    auto start = chrono::high_resolution_clock::now();
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> ms_double;
    double timeComp = 0.0;

    vector<vector<double>> pheromones; /// pheromone matrix

    vector<vector<int>> antSolution; /// solution for each ant
    vector<int> antWCT; /// wct for each ant

    vector<int> bestSolEver;
    int bestWCTEver;

    double rho;
    int max_iterations;

    if (readParams) {
        if (!readACOArguments(rho,max_iterations)) { return false; }
    } else {
        setDefaultParameters(rho,max_iterations);
    }

    resizeMatrix(pheromones,antSolution,antWCT,bestSolEver);

    // INITIAL SOLUTION
    simplifiedRZ(bestSolEver,bestWCTEver);
    embeddedVND(bestSolEver,bestWCTEver);

    // INIT PHEROMONES
    initPheromones(pheromones,bestWCTEver);

    bool termination = false;
    int iteration = 1;
    int lastWCT = 0;
    int stagnate = 0;

    while (!termination) {

        for (int ant = 1; ant <= 1; ant ++) {

            // construct solution
            if (!constructSolution(pheromones,bestSolEver,ant,antSolution,antWCT)) { return false; }
            // operate local search
            embeddedVND(antSolution[ant], antWCT[ant]);

            // Check if the algorithm is in stagnation
            if (lastWCT == antWCT[ant]) {
                stagnate ++;
                if (stagnate == 8) {
                    disturbPheromones(pheromones);
                }
            } else {
                stagnate = 0;
            }
            
            lastWCT = antWCT[ant];

            // update best sol ever
            if (antWCT[ant] < bestWCTEver) {
                bestWCTEver = antWCT[ant];
                replaceSolByNewSol(bestSolEver,antSolution[ant]);
                end = chrono::high_resolution_clock::now();
                ms_double = end - start;
                cout << "new best : WCT -> "<< bestWCTEver << ", time : " << ms_double.count()/1000 << "s, it : " << iteration << endl;
            }

            // Update pheromones
            updatePheromones(bestSolEver,antSolution,antWCT,rho,pheromones);
        }

        iteration ++;

        end = chrono::high_resolution_clock::now();
        ms_double = end - start;

        // Check if the algorithm needs to stop
        if (!updateTermination(iteration, max_iterations, ms_double, bestWCTEver, target, timeLimit, termination)) { return false; }
    }

    return true;
}

#endif