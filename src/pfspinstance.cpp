/***************************************************************************
 *   Copyright (C) 2012 by Jérémie Dubois-Lacoste   *
 *   jeremie.dl@gmail.com   *
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

#include <iostream>
#include <vector>
#include <string.h>
#include <fstream>

#include "pfspinstance.h"

using namespace std;

void PfspInstance::printSolBis(vector<int> & sol)
{
    cout << "[";
    for (int i = 1; i < sol.size(); i ++) {
        cout << sol[i];
        if (i < sol.size()-1) {
            cout << ", ";
        }
    }
    cout << "]" << endl;
}

void PfspInstance::printMatrixBis(vector<vector<int>> & matrix)
{
    cout << "[";
    for (int i = 1; i < matrix.size(); i++) {
        printSolBis(matrix[i]);
    }
    cout << "]" << endl;
}

/// Constructor
PfspInstance::PfspInstance()
{
	/// Nothing. We fill the object using the function readDataFromFile(char * filename)
}

/// Destructor
PfspInstance::~PfspInstance()
{
}

int PfspInstance::getNbJob()
{
	return nbJob;
}

int PfspInstance::getNbMac()
{
	return nbMac;
}

/// Allow the memory for the processing times matrix : */
void PfspInstance::allowMatrixMemory(int nbJ, int nbM)
{
	processingTimesMatrix.resize(nbJ+1);          /// 1st dimension

	for (int cpt = 0; cpt < nbJ+1; ++cpt) {
		processingTimesMatrix[cpt].resize(nbM+1); /// 2nd dimension
	}

	dueDates.resize(nbJ+1);
	priority.resize(nbJ+1);
}

bool PfspInstance::getTime(int job, int machine, int & result)
{
	if (job == 0) {
		return false;
    }
	else
	{
		if ((job < 1) || (job > nbJob) || (machine < 1) || (machine > nbMac)) {
			cout    << "ERROR. file:pfspInstance.cpp, method:getTime. Out of bound. job="
			<< job << ", machine=" << machine << std::endl;
			return false;
		}
		result = processingTimesMatrix[job][machine];
        return true;
	}
}

bool PfspInstance::getPriority(int job, int & result)
{
    if (job == 0) {
        return false;
    }
    else
    {
        if ((job < 1) || (job > nbJob)) {
			cout    << "ERROR: file:pfspInstance.cpp, method:getPriority, message:Out of bound. job = " << job << endl;
			return false;
		}
        result = priority[job];
        return true;
    }
}

/// Read the instance from file
bool PfspInstance::readDataFromFile(string fileName)
{
	bool everythingOK = true;
	int j, m; /// iterators
	int readValue;
	string str;
	ifstream fileIn;

	fileIn.open(fileName);

	if ( fileIn.is_open() ) {
		fileIn >> nbJob;
		fileIn >> nbMac;
		allowMatrixMemory(nbJob, nbMac);
		/// reading the processing times
		for (j = 1; j <= nbJob; ++j)
		{
			for (m = 1; m <= nbMac; ++m)
			{
				fileIn >> readValue; /// The number of each machine, not important !
				fileIn >> readValue; /// Process Time

				processingTimesMatrix[j][m] = readValue;
			}
		}
        fileIn >> str; /// this is not read ("Reldue")
		/// reading the dueDates and priority for each job
		for (j = 1; j <= nbJob; ++j)
		{
			fileIn >> readValue; /// -1
			fileIn >> readValue;
			dueDates[j] = readValue;
			fileIn >> readValue; /// -1
			fileIn >> readValue;
            priority[j] = readValue;
		}
		fileIn.close();
	}
	else {
		cout    << "ERROR: file:pfspInstance.cpp, method:readDataFromFile, "
				<< "error while opening file " << fileName << std::endl;

		everythingOK = false;
	}
	return everythingOK;
}

/**
 * Compute the weighted sum of completion time of a given partial solution
 * 
 * @param[in] nbJobs Number of jobs currently in solution (sol). IMPORTANT : nbJobs not necessarely equal to the length of sol.
 * @param[in] sol Solution we want to evaluate
 * @param[out] wct Resulting WCT
 */
bool PfspInstance::computeWCT(int nbJobs, vector< int > & sol, int & wct)
{
    int j, m;
	int jobNumber;

	/// We need end times on previous machine
	vector< int > previousMachineEndTime ( nbJobs + 1 );
	/// And the end time of the previous job, on the same machine
	int previousJobEndTime;

	/// 1st machine
	previousMachineEndTime[0] = 0;
	for ( j = 1; j <= nbJobs; ++j )
	{
		jobNumber = sol[j];
		previousMachineEndTime[j] = previousMachineEndTime[j-1] + processingTimesMatrix[jobNumber][1];
	}

	/// others machines
	for ( m = 2; m <= nbMac; ++m )
	{
		previousMachineEndTime[1] += processingTimesMatrix[sol[1]][m];
		previousJobEndTime = previousMachineEndTime[1];


		for ( j = 2; j <= nbJobs; ++j )
		{
			jobNumber = sol[j];

			if ( previousMachineEndTime[j] > previousJobEndTime )
			{
				previousMachineEndTime[j] = previousMachineEndTime[j] + processingTimesMatrix[jobNumber][m];
				previousJobEndTime = previousMachineEndTime[j];
			}
			else
			{
				previousJobEndTime += processingTimesMatrix[jobNumber][m];
				previousMachineEndTime[j] = previousJobEndTime;
			}
		}
	}

	wct = 0;
	for ( j = 1; j<= nbJobs; ++j ) {
	    wct += previousMachineEndTime[j] * priority[sol[j]];
    }

    return true;
}