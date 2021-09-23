
/***********************************************************
 * Functions for handeling error messages and help message *
 ***********************************************************/

#ifndef _ERRORS_H_
#define _ERRORS_H_

#include <iostream>

using namespace std;

/**
 * Print the error message associated with the given parameters
 * 
 * @tparam T the type of the value of the variable that caused the error
 * @param[in] file String denoting the file where the error occured
 * @param[in] method String denoting the method where the error occured
 * @param[in] message String denoting the message associated with the error
 * @param[in] variable String denoting the name of the variable wich value caused the error
 * @param[in] variableValue Value, of type T, of the variable that caused the error.
 */
template<typename T>
void generateError(string file, string method, string message, string variable, T variableValue)
{
    cerr << "\nERROR: file:" << file << ", method:" << method << ", message:" << message << ", " << variable << " = " << variableValue << endl << endl;
}

void generateError(string file, string method, string message)
{
    cerr << "\nERROR: file:" << file << ", method:" << method << ", message:" << message << "." << endl << endl;
}

/**
 * Print a quick overview of how to run the program
 */
void help()
{
    cout << "Usage: ./main --SLS <ILS or ACO> --instance_file <e.g ../instances/50_20_01> --max_iterations <int>" << endl << endl;

    cout << "Exemple : ./main --SLS ACO --instance_file ../instances/50_20_01" << endl;
    cout << "Exemple : ./main --SLS ILS --instance_file ../instances/100_20_02 --max_iterations 60" << endl << endl;
}

template<typename T1>
void printVect(vector<T1> & sol)
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

template<typename T3>
void printVect(vector<T3> & sol, int & nbItems)
{
    cout << "[";
    for (int i = 1; i < nbItems+1; i ++) {
        cout << sol[i];
        if (i < nbItems) {
            cout << ", ";
        }
    }
    cout << "]" << endl;
}

template<typename T2>
void printMatrix(vector<vector<T2>> & matrix)
{
    cout << "[";
    for (int i = 1; i < matrix.size(); i++) {
        printVect(matrix[i]);
    }
    cout << "]" << endl;
}

#endif