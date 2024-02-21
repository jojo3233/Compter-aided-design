#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>  // prototype for strtok
#include <map>
#include <algorithm>
#include <iterator>
#include <sstream>  // for split by space
#include <parser.h>

using namespace std;

int main(int argc, char **argv)
{
    string writeFileName;
    //writeFileName = argv[2];
    bool isTautology, isContainment;
    Parser myParser(argv[1], argv[2]);  
    myParser.toMatrix();  
    //myParser.showMatrix(myParser.matrixVec, 0);
    isTautology = myParser.Tautology(myParser.matrixVec);
    if(!myParser.printComplement) {
        myParser.Complement();
    }
    //myParser.Complement(myParser.matrixVec); 
    isContainment = myParser.Containment(myParser.matrixVec);    
    
    if(isTautology) {
        cout << "Tautology: true" << endl;
    } else {
        cout << "Tautology: false" << endl;
    } 
    if(isContainment) {
        cout << "Containment: true" << endl;
    } else {
        cout << "Containment: false" << endl;
    }      
    /*
    unsigned char M[3];
    M[0] = 1;
    M[1] = 3;
    M[2] = 2;
    cout << (M[0]&M[2]) << endl;
    */
}