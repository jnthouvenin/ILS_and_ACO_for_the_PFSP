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

#ifndef _PFSPINSTANCEWT_H_
#define _PFSPINSTANCEWT_H_

#include <vector>

using namespace std;

class PfspInstance{

  private:
    int nbJob;
    int nbMac;
    std::vector< int > dueDates;
    std::vector< int > priority;
    std::vector< std::vector <int> > processingTimesMatrix;

  public:
    void printSolBis(vector<int> & sol);
    void printMatrixBis(vector<vector<int>> & matrix);

    PfspInstance();
    ~PfspInstance();

    /* Read write privates attributs : */
    int getNbJob();
    int getNbMac();

    /* Allow the memory for the processing times matrix : */
    void allowMatrixMemory(int nbJ, int nbM);

    /**
     * Getter for the processing time of the job (job) on the machine (machine)
     * 
     * @param[in] job The job
     * @param[in] machine The machine
     * @param[out] result The result
     * @returns false if an error occured, true otherwise
     */
    bool getTime(int job, int machine, int & result);
    void setTime(int job, int machine, int processTime);

    int getDueDate(int job);
    void setDueDate(int job, int value);

    bool getPriority(int job, int & result);
    void setPriority(int job, int value);

    /* Read Data from a file : */
    bool readDataFromFile(string fileName);

    bool computeWCT(int nbJobs, vector< int > & sol, int & wct);
};

#endif
