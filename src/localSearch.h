/***************************************************************************
 *   Copyright (C) 2012 by Jérémie Dubois-Lacoste                          *
 *   jeremie.dl@gmail.com                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _PROJECT_1_H_
#define _PROJECT_1_H_

#include <iostream>      /// rand()
#include <vector>
#include <float.h>       /// max value for floats and doubles
#include <chrono>        /// measuring times
#include <algorithm>

#include "pfspinstance.h" /// pfsp instance used to represent the problem
#include "globalVariables.h" /// global variables used in the whole program (solution, instance, etc.)
#include "errors.h" /// functions to print errors, and help
#include "parameters.h" /// functions to manage parameters parsing, setting and printing
#include "files.h" /// functions to read, write, manage files

using namespace std;

/*************************************************************************************
 *                                    EXERCICE 1.1                                   *
 *************************************************************************************/

/**
 * Fill the global solution according to the randomPermutation initialisation
 * 
 * @return false if one error occured, true otherwise
 */
bool randomPermutation()
{
    int nbJobs = instance.getNbJob();

    if (solution.size() != nbJobs + 1) {
        generateError("flowshop.cpp","randomPermutation","initial_solution parameter not recognized", "solution.size()", solution.size());
        return false;
    }

    vector<bool> alreadyTaken(nbJobs+1, false); // nbJobs elements with value false
    vector<int > choosenNumber(nbJobs+1, 0);

    int nbj;
    int rnd, i, j, nbFalse;

    nbj = 0;
    for (i = nbJobs; i >= 1; --i) {
        rnd = rand() % i + 1;
        nbFalse = 0;

        /// find the rndth cell with value = false
        for (j = 1; nbFalse < rnd; ++j){
            if ( ! alreadyTaken[j] ) {
                ++nbFalse;
            }
        }
        --j;
        solution[j] = i;

        ++nbj;
        choosenNumber[nbj] = j;

        alreadyTaken[j] = true;
    }

    /// update global WCT according to the new global solution found
    if (!instance.computeWCT(nbJobs,solution,WCT)) {
        return false;
    }

    return true;
}

/**
 * Replace values of the given ancient solution by the values of the given new solution
 * 
 * The solutions keep their adress in memory, we modify values one by one.
 * 
 * @param[out] ancientSol The sol to replace
 * @param[in] newSol The sol replacing
 */
bool replaceSolByNewSol(vector<int> & solToReplace, vector<int> & newSol)
{
    if (solToReplace.size() != newSol.size()) {
        generateError("localSearch.h","replaceSolByNewSol","The two solutions don't have the same size");
        return false;
    }
    for (int i = 1; i < solToReplace.size(); i++) {
        solToReplace[i] = newSol[i];
    }
    return true;
}

/**
 * Modify the given solution by adding a new job, in a way that is minimizing the resulting WCT
 * 
 * Exemple : minSol = {1,4,2}, nbJobs = 3, newJob = 3
 *      we construct the following solutions : {3,1,4,2},{1,3,4,2},{1,4,3,2},{1,4,2,3}.
 *      we evaluate WCT on each one and the one with the lowest WCT replace the given solution (minSol)
 * 
 * @param[in] nbJobs The initial number of jobs in minSol
 * @param[in] newJob The id of the new job, that we'll add to minSol
 * @param[in,out] minSol The solution to which we'll add the new job, and become the best new solution.
 * @return false if one error occured, true otherwise
 */
bool constructSolMinWCT(int nbJobs, int newJob, vector<int> & minSol)
{
    /// initialization : we add the new job at the end of the permutation sequence and compute the new WCT
    //vector< int > sol (minSol); /// original solution sotred, to keep track of it
    minSol[nbJobs+1] = newJob; /// we add the new job at the end
    vector<int> solBuffer (minSol);
    
    /// computing the value of WCT for minSol
    int minWCT = 0;
    if (!instance.computeWCT(nbJobs+1,minSol,minWCT)) {
        return false;
    }

    //vector<int> solBuffer (sol.size()); /// storing the solution the algorithm is working with
    int wctBuffer = 0;
    /// beginning minimization
    //for (int i = 1; i <= nbJobs; i++) {
    for (int i = nbJobs; i >= 1; i --) {
        /// we copy the first part of the sol unchanged into solBuffer
        /*for (int j = 1; j < i; j++) {
            solBuffer[j] = sol[j];
        }
        /// we insert the new job at index i
        solBuffer[i] = newJob;
        /// we copy the remaining jobs of sol into solBuffer
        for (int j = i + 1; j <= nbJobs+1; j++) {
            solBuffer[j] = sol[j-1];
        }*/
        solBuffer[i+1] = solBuffer[i];
        solBuffer[i] = newJob;
        /// compute WCT of the buffer solution
        if (!instance.computeWCT(nbJobs+1,solBuffer,wctBuffer)) {
            return false;
        }
        /// if solBuffer is better than or current min sol, we update minSol
        if (wctBuffer < minWCT) {
            minWCT = wctBuffer;
            replaceSolByNewSol(minSol,solBuffer);
        }
    }
    return true;
}

/**
 * Fill the global solution (solution) according to the simplifiedRZ heuristic
 * 
 * @return false if one error occured, true otherwise
 */
bool simplifiedRZ(vector<int> & sol, int & wct)
{
    if (sol.size() != instance.getNbJob()+1) {
        generateError("localSearch.h","simplifiedRZ","The given sol doesn't have the right size","sol.size()",sol.size());
        return false;
    }
    vector< double > ratios (instance.getNbJob() + 1);
    vector<int> permRatios (instance.getNbJob()+1);
    //vector< int > sol (instance.getNbJob() + 1);
    vector<int>::iterator it = permRatios.begin();
    advance(it,1);

    for (int i = 1; i<=instance.getNbJob(); i++){
        permRatios[i] = i;
    }

    /// computing the ratios of the jobs
    int nbTabuElements = 0;
    int timeBuffer = 0;
    int priorityBuffer = 0;
    for (int job = 1; job <= instance.getNbJob(); ++job) {
        ratios[job] = 0.;
        for (int machine = 1; machine <= instance.getNbMac(); ++machine) {
            if (!instance.getTime(job,machine,timeBuffer)) {
                return false;
            }
            ratios[job] += timeBuffer;
        }
        if (!instance.getPriority(job,priorityBuffer)) {
            return false;
        }
        ratios[job] = ratios[job] / priorityBuffer;
    }

    sort(it,permRatios.end(),[&](const int& a, const int& b){return (ratios[a] < ratios[b]);});

    /// adding the jobs one by one to sol, starting by the best ones
    int indexMin = 0; /// index of smallest ratio
    for (int i = 0; i < instance.getNbJob(); i++) {
        indexMin = permRatios[i+1];
        /// add the best job (smallest ratio) into the current solution (in a way that minimise the new wct)
        constructSolMinWCT(i, indexMin, sol);
    }

    /// updating the solution (global variable)
    //replaceSolByNewSol(solution,sol);
    /// update global WCT
    if (!instance.computeWCT(instance.getNbJob(),sol,wct)) {
        return false;
    }

    return true;
}

/**
 * Resize and fill the global solution (solution) according to the initialisation parameter (initial_solution)
 * 
 * @return false if one error occured, true otherwise
 */
bool generateInitialSolution()
{
    solution.resize(instance.getNbJob()+1); /// allow memory
    if (initial_solution == Random_permutation) {
        return randomPermutation();
    }
    else if (initial_solution == Simplified_RZ) {
        return simplifiedRZ(solution,WCT);
    }
    else {
        generateError("flowshop.cpp","generateInitialeSolution","value of initial_solution not recognized","initial_solution",initial_solution);
        return false;
    }
}

/**
 * Operate a transpose neighborhood on the given current solution, according to the pivoting parameter (pivoting_rule)
 * 
 * IMPORTANT PRECONDITIONS : solution = currentSol = bestSol (values wise), WCT = currentWCT = bestWCT
 * We make sure that this is also the case at the end of the function
 * 
 * @param[in,out] currentSol Solution before the transpose, will be modified by the transpose selected. For best_improvement, is used as buffer solution (operating transpose, computing WCT on it, update bestSol, then undoing the transpose)
 * @param[in,out] currentWCT WCT associated with currentSol, will change as currentSol evolve
 * @param[in,out] bestSol For first_improvement, is used as a buffer solution. For best_improvement, stores best solution
 * @param[in,out] bestWCT WCT associated with bestSol, will change as bestSol evolve
 * @param[out] improving true if we found an improving transpose, false otherwise
 * @return false if one error occured, true otherwise
 */
bool transpose(vector<int> & currentSol, int & currentWCT, vector<int> & bestSol, int & bestWCT, bool & improving)
{
    if (pivoting_rule == First_improvement) {
        //printVect(solution);
        for (int i = 1; i < instance.getNbJob(); i++) {
            bestSol[i] = solution[i+1];
            bestSol[i+1] = solution[i];
            //cout << "modif done" << endl;
            if (!instance.computeWCT(instance.getNbJob(),bestSol,bestWCT)) { /// compute new wct of the transposed solution
                return false;
            }
            //cout << "compute WCT done" << endl;
            if (bestWCT < WCT) { /// if the new wct is better than the old one, we end the computation
                improving = true;
                if (!replaceSolByNewSol(solution,bestSol)) { return false; }
                if (!replaceSolByNewSol(currentSol,bestSol)) { return false; }
                WCT = bestWCT;
                currentWCT = bestWCT;
                return true; // END
            }
            else { /// we undo the transposition
                bestSol[i] = solution[i];
                bestSol[i+1] = solution[i+1];
                bestWCT = WCT;
            }
        }
    } else { /// in best_improvement, bestSol isn't equal to solution and currentSol because it has to remember current best transposition
        for (int i = 1; i < instance.getNbJob(); i++) {
            currentSol[i] = solution[i+1];
            currentSol[i+1] = solution[i];
            if (!instance.computeWCT(instance.getNbJob(),currentSol,currentWCT)) { /// compute new wct of the transposed solution
                return false;
            }
            if (currentWCT < bestWCT) { /// if the new wct is better than the current best one, we update best one
                if (!replaceSolByNewSol(bestSol,currentSol)) { return false; }
                bestWCT = currentWCT;
            }
            /// we undo the transposition
            currentSol[i] = solution[i];
            currentSol[i+1] = solution[i+1];
            currentWCT = WCT;
        }
        /// checking if we have got better
        if (bestWCT < WCT) {
            improving = true;
            if (!replaceSolByNewSol(solution,bestSol)) { return false; }
            if (!replaceSolByNewSol(currentSol,bestSol)) { return false; }
            WCT = bestWCT;
            currentWCT = bestWCT;
            return true;
        }
    }
    /// we didn't find any transposition, we don't need to replace any thing, solution = currentSol = bestSol
    improving = false;
    return true;
}

/**
 * Operate a exchange neighborhood on the given current solution, according to the pivoting parameter (pivoting_rule)
 * 
 * @param[in,out] currentSol Solution before the exchange, will be modified by the exchange selected. For best_improvement, is used as buffer solution (operating exchange, computing WCT on it, update bestSol, then undoing the exchange)
 * @param[in,out] currentWCT WCT associated with currentSol, will change as currentSol evolve
 * @param[in,out] bestSol For first_improvement, is used as a buffer solution. For best_improvement, stores best solution
 * @param[in,out] bestWCT WCT associated with bestSol, will change as bestSol evolve
 * @param[out] improving true if we found an improving exchange, false otherwise
 * @return false if one error occured, true otherwise
 */
bool exchange(vector<int> & currentSol, int & currentWCT, vector<int> & bestSol, int & bestWCT, bool & improving)
{
    if (pivoting_rule == First_improvement) {
        for (int i = 1; i < instance.getNbJob(); i++) {
            for (int j = i+1; j <= instance.getNbJob(); j++) {
                bestSol[i] = solution[j];
                bestSol[j] = solution[i];
                if (!instance.computeWCT(instance.getNbJob(),bestSol,bestWCT)) {
                    return false;
                }
                if (bestWCT < WCT) {
                    improving = true;
                    if (!replaceSolByNewSol(solution,bestSol)) { return false; }
                    if (!replaceSolByNewSol(currentSol,bestSol)) { return false; }
                    WCT = bestWCT;
                    currentWCT = bestWCT;
                    return true;
                }
                else { /// we undo the transposition
                    bestSol[i] = solution[i];
                    bestSol[j] = solution[j];
                    bestWCT = WCT;
                }
            }
        }
    } else {
        for (int i = 1; i < instance.getNbJob(); i++) {
            for (int j = i+1; j <= instance.getNbJob(); j++) {
                currentSol[i] = solution[j];
                currentSol[j] = solution[i];
                if (!instance.computeWCT(instance.getNbJob(),currentSol,currentWCT)) {
                    return false;
                }
                if (currentWCT < bestWCT) {
                    if (!replaceSolByNewSol(bestSol,currentSol)) { return false; }
                    bestWCT = currentWCT;
                }
                /// we undo the transposition
                currentSol[i] = solution[i];
                currentSol[j] = solution[j];
                currentWCT = WCT;
            }
        }
        /// checking if we have got better
        if (bestWCT < WCT) {
            improving = true;
            if (!replaceSolByNewSol(solution,bestSol)) { return false; }
            if (!replaceSolByNewSol(currentSol,bestSol)) { return false; }
            WCT = bestWCT;
            currentWCT = bestWCT;
            return true;
        }
    }
    improving = false;
    return true;
}

bool operateInsert(int & i, int & j, vector<int> oldSolution, vector<int> & newSolution, bool verbose) {
    if (i < 1 || i > instance.getNbJob()) {
        generateError("localSearch.h","operateInsert","i out of bound","i",i);
        return false;
    }
    if (j < 1) {
        generateError("localSearch.h","operateInsert","j out of bound","j",j);
        return false;
    }
    if (i != j) {
        if (verbose) {
            cout << "operateInsert -> (" << i << "," << j <<") : " << oldSolution[i] << "," << oldSolution[j] << endl;
        }
        if (i < j) {
            for (int x = i; x < j; x++) {
                newSolution[x] = oldSolution[x+1];
            }
            newSolution[j] = oldSolution[i];
        } else {
            for (int x = j+1; x <= i; x++) {
                newSolution[x] = oldSolution[x-1];
            }
            newSolution[j] = oldSolution[i];
        }
    }
    return true;
}

/**
 * Operate a insert neighborhood on the given current solution, according to the pivoting parameter (pivoting_rule)
 * 
 * @param[in,out] currentSol Solution before the insert, will be modified by the insert selected. For best_improvement, is used as buffer solution (operating insert, computing WCT on it, update bestSol, then undoing the insert)
 * @param[in,out] currentWCT WCT associated with currentSol, will change as currentSol evolve
 * @param[in,out] bestSol For first_improvement, is used as a buffer solution. For best_improvement, stores best solution
 * @param[in,out] bestWCT WCT associated with bestSol, will change as bestSol evolve
 * @param[out] improving true if we found an improving insert, false otherwise
 * @return false if one error occured, true otherwise
 */
bool insert(vector<int> & currentSol, int & currentWCT, vector<int> & bestSol, int & bestWCT, bool & improving)
{
    if (pivoting_rule == First_improvement) {
        for (int i = 1; i <= instance.getNbJob(); i++) {
            for (int j = 1; j <= instance.getNbJob(); j++) {
                if (i != j) {
                    if (i < j) {
                        for (int x = i; x < j; x++) {
                            bestSol[x] = solution[x+1];
                        }
                        bestSol[j] = solution[i];
                    } else {
                        for (int x = j+1; x <= i; x++) {
                            bestSol[x] = solution[x-1];
                        }
                        bestSol[j] = solution[i];
                    }
                    if (!instance.computeWCT(instance.getNbJob(),bestSol,bestWCT)) {
                        return false;
                    }
                    if (bestWCT < WCT) {
                        improving = true;
                        if (!replaceSolByNewSol(solution,bestSol)) { return false; }
                        if (!replaceSolByNewSol(currentSol,bestSol)) { return false; }
                        WCT = bestWCT;
                        currentWCT = bestWCT;
                        return true;
                    }
                    else { /// we undo the 
                        if (i < j) {
                            for (int x = i; x <= j; x++) {
                                bestSol[x] = solution[x];
                            }
                            
                        } else {
                            for (int x = j; x <= i; x++) {
                                bestSol[x] = solution[x];
                            }
                        }
                        bestWCT = WCT;
                    }
                }
            }
        }
    } else {
        for (int i = 1; i <= instance.getNbJob(); i++) {
            for (int j = 1; j <= instance.getNbJob(); j++) {
                if (i != j) {
                    if (i < j) {
                        for (int x = i; x < j; x++) {
                            currentSol[x] = solution[x+1];
                        }
                        currentSol[j] = solution[i];
                    } else {
                        for (int x = j+1; x <= i; x++) {
                            currentSol[x] = solution[x-1];
                        }
                        currentSol[j] = solution[i];
                    }
                    if (!instance.computeWCT(instance.getNbJob(),currentSol,currentWCT)) {
                        return false;
                    }
                    if (currentWCT < bestWCT) {
                        if (!replaceSolByNewSol(bestSol,currentSol)) { return false; }
                        bestWCT = currentWCT;
                    }
                    /// we undo the transposition
                    if (i < j) {
                        for (int x = i; x <= j; x++) {
                            currentSol[x] = solution[x];
                        }
                    } else {
                        for (int x = j; x <= i; x++) {
                            currentSol[x] = solution[x];
                        }
                    }
                    currentWCT = WCT;
                }
            }
        }
        if (bestWCT < WCT) {
            improving = true;
            if (!replaceSolByNewSol(solution,bestSol)) { return false; }
            if (!replaceSolByNewSol(currentSol,bestSol)) { return false; }
            WCT = bestWCT;
            currentWCT = bestWCT;
            return true;
        }
    }
    improving = false;
    return true;
}

/**
 * Operate an iterative improvement on the global solution.
 * 
 * Initialise global solution, then operate a descent on it.
 * 
 * @return false if one error occured, true otherwise
 */
bool iterativeImprovement()
{
    if (!generateInitialSolution()) {
        return false;
    }

    if (!experiment) {
        cout << "WCT : " << WCT;
    }

    vector<int> currentSol (solution);
    vector<int> bestSol (solution);
    int currentWCT = WCT;
    int bestWCT = WCT;

    bool improving = false;
    if (neighborhood == Transpose) {
        if (!transpose(currentSol,currentWCT, bestSol, bestWCT, improving)) {
            return false;
        }
        while(improving) {
            if (!transpose(currentSol,currentWCT, bestSol, bestWCT, improving)) {
                return false;
            }
        }
    } else if (neighborhood == Exchange) {
        if (!exchange(currentSol,currentWCT, bestSol, bestWCT, improving)) {
            return false;
        }
        while(improving) {
            if (!exchange(currentSol,currentWCT, bestSol, bestWCT, improving)) {
                return false;
            }
        }
    } else {
        if (!insert(currentSol,currentWCT, bestSol, bestWCT, improving)) {
            return false;
        }
        while(improving) {
            if (!insert(currentSol,currentWCT, bestSol, bestWCT, improving)) {
                return false;
            }
        }
    }

    /// free memory
    currentSol = vector<int>();
    bestSol = vector<int>();

    return true;
}

/*************************************************************************************
 *                                    EXERCICE 1.2                                   *
 *************************************************************************************/

/**
 * Modify the global pivoting parameter (pivoting_rule) according the given index and the global neighborhood order parameter (neighborhood_order)
 * 
 * if (neighborhood_order == tr_ex_in) {
 *     1 = Transpose; 2 = Exchange; 3 = Insert;
 * }
 * 
 * @param[in] i The index of the neighborhood in the neighborhood order
 * @return false if one error occured, true otherwise
 */
bool setPivotingRule(int i)
{
    if (i == 0 || i > 3) {
        generateError("flowshop.cpp","setPivotingRule","index of pivoting rule out of range","index",i);
        return false;
    } else {
        if (i == 1) {
            neighborhood = Transpose;
        } else if (i == 2) {
            if (neighborhood_order == tr_ex_in) { neighborhood = Exchange; }
            else { neighborhood = Insert; }
        } else {
            if (neighborhood_order == tr_ex_in) { neighborhood = Insert; }
            else { neighborhood = Exchange; }
        }
        return true;
    }
}

/**
 * Operate a variable neighborhood descent on the global solution
 * 
 * Initialise solution and operate variable neighborhood descent on it
 * 
 * @return false if one error occured, true otherwise
 */
bool vnd(bool generateInitial, bool verbose)
{
    if (generateInitial) {
        if (!generateInitialSolution()) {
            return false;
        }
    }

    if (!experiment && verbose) {
        cout << "WCT : " << WCT;
    }

    vector<int> currentSol (solution);
    vector<int> bestSol (solution);
    int currentWCT = WCT;
    int bestWCT = WCT;

    int k = 3;
    int i = 1;

    bool improving = false;

    while (i <= k) {
        improving = false;
        if (!setPivotingRule(i)) {
            return false;
        }
        if (neighborhood == Transpose) {
            if (!transpose(currentSol,currentWCT,bestSol,bestWCT,improving)) {
                return false;
            }
        } else if (neighborhood == Exchange) {
            if (!exchange(currentSol,currentWCT,bestSol,bestWCT,improving)) {
                return false;
            }
        } else {
            if (!insert(currentSol,currentWCT,bestSol,bestWCT,improving)) {
                return false;
            }
        }
        if (!improving) {
            i ++;
        } else {
            i = 1;
        }
    }

    /// free memory
    currentSol = vector<int>();
    bestSol = vector<int>();

    return true;
}

/**
 * Operate a VND local search on the given solution
 * 
 * @param[in,out] sol The solution to improve
 * @param[in,out] wct The associated WCT
 * @return false if an error occured, true otherwise
 */
bool embeddedVND(vector<int> & sol, int & wct) {
    if (sol.size() != instance.getNbJob()+1) {
        generateError("localSearch.h","embeddedVND","sol doesn't have the right size","sol.size()",sol.size());
        return false;
    }
    WCT = wct;
    solution.resize(sol.size());
    neighborhood_order = tr_in_ex;
    pivoting_rule = First_improvement;
    initial_solution = Simplified_RZ;
    algorithm = VND;
    experiment = false;
    if (!replaceSolByNewSol(solution,sol)) { return false; }
    if (!vnd(false,false)) { return false; }
    if (!replaceSolByNewSol(sol,solution)) { return false; }
    wct = WCT;
    return true;
}

#endif