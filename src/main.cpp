
#include "localSearch.h"
#include "experiment.h"
#include "aco.h"
#include "ils.h"

int main(int argc, char * argv[]) {

    if (!readSLS(argc,argv)) {
        help();
        return false;
    }

    bool readParams = true;
    int target = 0;
    double timeLimit = 250*1000.;

    if (sls_global == ACO) {
        if (!ACO_heuristic(readParams,target,timeLimit)) {
            return false;
        }
    } else {
        if (!ILS_heuristic(readParams,target,timeLimit)) {
            return false;
        }
    }

    return true;
}