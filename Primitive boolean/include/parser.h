#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>  // prototype for strtok
#include <map>
#include <algorithm>
#include <iterator>
#include <sstream>  // for split by space


using namespace std;

class Parser {
public:
    int inputSize, outputSize, matrixSize, p;
    bool isTautology = true, printComplement = false, complementTautology = true, round1 = true;
    vector<string> lines, functionVec1, functionVec2, inputVec, outputVec;
    vector<unsigned char> matrixVec, matrixVec2, complementVec;
    string complementStr;
    Parser(string, string);
    void toMatrix();
    bool Tautology(vector<unsigned char>);
    bool checkTautology(vector<unsigned char>, int, bool);
    void showMatrix(vector<unsigned char>, int);
    bool Containment(vector<unsigned char>);
    void Complement();
};

void Parser::showMatrix(vector<unsigned char> matrix, int key) {
    int k = 0;
    for(int i = 0 ; i < matrix.size()/(inputSize-key) ; i++)
    {
        for(int j = 0; j < inputSize-key ; j++)
        {
            //cout << static_cast<unsigned int>(matrixVec[k]) << " ";
            if(matrix[k] == 0) {
                cout << "00 ";
            } else if(matrix[k] == 1) {
                cout << "01 ";
            } else if(matrix[k] == 2) {
                cout << "10 ";
            } else if(matrix[k] == 3) {
                cout << "11 ";
            }
            k++;
        }
        cout << endl;
    }
}
Parser::Parser(string PLA_FILE1, string PLA_FILE2) {
    ifstream readFile1, readFile2;
    readFile1.open(PLA_FILE1);
    string line;
    while(getline(readFile1, line))
    {    
        vector<string> tokenVec;
        lines.push_back(line);
        // case: 1.i 2.o 
        if( line == "" || line[0] == '#' )
            continue;
        // split line with space
        istringstream iss(line);
        string token;
        while ( iss >> token )
        {
            // split data's process
            tokenVec.push_back( token );
            //cout << token << " ";
        }
        //cout << endl;
        if(tokenVec[0] == ".i") {
            inputSize = atoi(tokenVec[1].c_str());
            //cout << inputSize << endl;
        }else if(tokenVec[0] == ".o") {
            outputSize = atoi(tokenVec[1].c_str());
        }else if(tokenVec[0] == ".ilb") {
            int i = inputSize;
            line = line.substr(4);
            istringstream iss(line);
            string token;
            while(iss >> token) {
                inputVec.push_back(token);
            }
        }else if(tokenVec[0] == ".p") {
            int i = atoi(tokenVec[1].c_str());
            p = i;
            while(i--) {
                getline(readFile1, line);
                istringstream iss(line);
                string token;
                iss >> token;
                functionVec1.push_back(token);
                //cout << token << endl;
            }
        }else if(tokenVec[0] == ".e") {
            matrixSize = functionVec1.size();
            // end parser
            break;
        }
    }      
    
    if(!inputVec.size()) {
        for(int i = 1 ; i <= inputSize ; i++) {
            inputVec.push_back(to_string(i));
        }
    }
    
    line = "";
    readFile2.open(PLA_FILE2);
    while(getline(readFile2, line))
    {
        vector<string> tokenVec;
        //lines.push_back(line);
        // case: 1.i 2.o 
        if( line == "" || line[0] == '#' )
            continue;
        // split line with space
        istringstream iss(line);
        string token;
        while ( iss >> token )
        {
            // split data's process
            tokenVec.push_back( token );
            //cout << token << " ";
        }
        //cout << endl;
        if(tokenVec[0] == ".i") {
            //inputSize = atoi(tokenVec[1].c_str());
        }else if(tokenVec[0] == ".o") {
            //outputSize = atoi(tokenVec[1].c_str());
        }else if(tokenVec[0] == ".p") {
            getline(readFile2, line);
            //cout << line << endl;
            istringstream iss(line);
            string token;
            iss >> token;
            //cout << token << endl;
            functionVec2.push_back(token);
        }else if(tokenVec[0] == ".e") {
            // end parser
            break;
        }
    }      
    /*
    for(auto itVec = functionVec1.begin() ; itVec != functionVec1.end() ; itVec++) {
        cout << *itVec << endl;
    }
    for(auto itVec = functionVec2.begin() ; itVec != functionVec2.end() ; itVec++) {
        cout << *itVec << endl;
    }
    */
}

void Parser::toMatrix() {
    for(auto itVec = functionVec1.begin() ; itVec != functionVec1.end() ; itVec++)
    {
        for(int i = 0 ; i < inputSize ; i++)
        {  
            string str = *itVec;
            //cout << str[i] << " ";
            if(str[i] == '0') {
                matrixVec.push_back(2);
                //cout << matrixVec.size() << endl;
            }else if(str[i] == '1') {
                matrixVec.push_back(1);
               // cout << matrixVec.size() << endl;
            }else if(str[i] == '-') {
                matrixVec.push_back(3);
               // cout << matrixVec.size() << endl;
            }
        }
        //cout << endl;
    }
    for(auto itVec = functionVec2.begin() ; itVec != functionVec2.end() ; itVec++)
    {
        for(int i = 0 ; i < inputSize ; i++)
        {  
            string str = *itVec;
            //cout << str[i] << " ";
            if(str[i] == '0') {
                matrixVec2.push_back(2);
                //cout << matrixVec.size() << endl;
            }else if(str[i] == '1') {
                matrixVec2.push_back(1);
               // cout << matrixVec.size() << endl;
            }else if(str[i] == '-') {
                matrixVec2.push_back(3);
               // cout << matrixVec.size() << endl;
            }
        }
    }
    //cout << matrixVec.size() << endl;
    /*
    int k = 0;
    for(int i = 0 ; i < p ; i++)
    {
        for(int j = 0; j < matrixSize ; j++)
        {
            cout << matrixVec[k++] << " ";
        }
        cout << endl;
    }
    */
}

bool Parser::Tautology(vector<unsigned char> matrix) {
    bool isTautology1, isTautology2;
    isTautology1 = checkTautology(matrix, 0, 0);
    if(!isTautology1) {
        return false;
    } else {
        isTautology2 = checkTautology(matrix, 0, 1);
        if(isTautology1 && isTautology2) {
            return true;
        } else {
            return false;
        }
    }    
    return true;
}

bool Parser::checkTautology(vector<unsigned char> matrix, int key, bool prime) {
    if(!isTautology)
      return false;
    int k = 0, l = 0; 
    
    k = 0;
    /*
    cout << "Select variable " << inputVec[key];
    if(prime) {
        cout << "'" << endl;
    } else {
        cout << endl;
    }
    */
    if(round1) {
        if(prime) {
            complementVec.push_back(0);
        } else {
            complementVec.push_back(1);
        }
    }
    /*
    cout << complementVec.size() << endl;
    for(int i = 0 ; i < complementVec.size() ; i++) {
        cout << +complementVec[i];
    }
    cout << endl;
    
    for(int i = 0 ; i < matrix.size()/(inputSize-key); i++)
    {
        for(int j = 0; j < inputSize-key ; j++)
        {
            //cout << static_cast<unsigned int>(matrixVec[k]) << " ";
            if(matrix[k] == 0) {
                cout << "00 ";
            } else if(matrix[k] == 1) {
                cout << "01 ";
            } else if(matrix[k] == 2) {
                cout << "10 ";
            } else if(matrix[k] == 3) {
                cout << "11 ";
            }
            k++;
        }
        cout << endl;
    }
    */
    vector<unsigned char> cofactorVec;
    for(int i = 0 ; i < inputSize-key ; i++)
    {  
        if(!i) {
            if(prime) {
                cofactorVec.push_back(2);
            } else {
                
                cofactorVec.push_back(1);
            }
        } else {
            cofactorVec.push_back(3);
        }
    }
    /*
    cout << "Cofactor w.r.t " << inputVec[key];
    if(prime) {
        cout << "' -> " << endl;
    } else {
        cout << " -> " << endl;
    }
    */
    /*
    cout << endl;
    for(int i = 0 ; i < inputSize-key ; i++)
    {
        if(cofactorVec[i] == 0) {
            cout << "00 ";
        } else if(cofactorVec[i] == 1) {
            cout << "01 ";
        } else if(cofactorVec[i] == 2) {
            cout << "10 ";
        } else if(cofactorVec[i] == 3) {
            cout << "11 ";
        }
    }
    cout << "AND" << endl;
    for(int i = 0 ; i < inputSize-key ; i++)
    {
        cout << "----";
    }
    cout << endl;
    */
    //cout << "AND:" << endl;
    
    k = 0;
    for(int i = 0 ; i < matrix.size()/(inputSize-key) ; i++)
    {    
        for(int j = 0 ; j < inputSize-key ; j++)
        {  
            matrix[k] = matrix[k] & cofactorVec[j];
            k++;
        }
    }  
    //showMatrix(matrix, key);
    //cout << "delete void" << endl;
    
    // if void
    k = 0;
    l = 0;
    vector<unsigned char> tmpMatrix;
    for(int i = 0 ; i < matrix.size()/(inputSize-key) ; i++)
    {    
        if(matrix[i*(inputSize-key)] != 0) {
            for(int j = 0 ; j < inputSize-key ; j++)
            {  
                tmpMatrix.push_back(matrix[k]);
                k++;
            }
        }else {
            k += inputSize-key;
        }
    } 
    matrix.clear();
    for(int i = 0 ; i < tmpMatrix.size() ; i++)
    {
        matrix.push_back(tmpMatrix[i]);
    }
    
    //showMatrix(matrix, key);
    
    if(!matrix.size()) {
        //cout << "case 0: matrix size is zero" << endl;
        return true;
    }
    /*
    k = 0;
    for(int i = 0 ; i < matrix.size()/(inputSize-key); i++)
    {
        for(int j = 0; j < inputSize-key ; j++)
        {
            //cout << static_cast<unsigned int>(matrixVec[k]) << " ";
            if(matrix[k] == 0) {
                cout << "00 ";
            } else if(matrix[k] == 1) {
                cout << "01 ";
            } else if(matrix[k] == 2) {
                cout << "10 ";
            } else if(matrix[k] == 3) {
                cout << "11 ";
            }
            k++;
        }
        cout << endl;
    }
    */
    for(int i = 0 ; i < inputSize-key ; i++)
    {  
        if(cofactorVec[i] == 0) {
            cofactorVec[i] = 3;
        } else if(cofactorVec[i] == 1) {
            cofactorVec[i] = 2;
        } else if(cofactorVec[i] == 2) {
            cofactorVec[i] = 1;
        } else if(cofactorVec[i] == 3) {
            cofactorVec[i] = 0;
        }
    }
    /*
    cout << endl;
    for(int i = 0 ; i < inputSize-key ; i++)
    {
        if(cofactorVec[i] == 0) {
            cout << "00 ";
        } else if(cofactorVec[i] == 1) {
            cout << "01 ";
        } else if(cofactorVec[i] == 2) {
            cout << "10 ";
        } else if(cofactorVec[i] == 3) {
            cout << "11 ";
        }
    }
    cout << "OR" << endl;
    for(int i = 0 ; i < inputSize-key ; i++)
    {
        cout << "----";
    }
    cout << endl;
    */
    // OR
    k = 0;
    for(int i = 0 ; i < matrix.size()/(inputSize-key) ; i++)
    {    
        for(int j = 0 ; j < inputSize-key ; j++)
        {  
            matrix[k] = matrix[k] | cofactorVec[j];
            k++;
        }
    }  
    
    // if void
    k = 0;
    l = 0;
    tmpMatrix.clear();
    for(int i = 0 ; i < matrix.size()/(inputSize-key) ; i++)
    {    
        if(matrix[i*(inputSize-key)] != 0) {
            for(int j = 0 ; j < inputSize-key ; j++)
            {  
                tmpMatrix.push_back(matrix[k]);
                k++;
            }
        }else {
            k += inputSize-key;
        }
    } 
    matrix.clear();
    for(int i = 0 ; i < tmpMatrix.size() ; i++)
    {
        matrix.push_back(tmpMatrix[i]);
    }
    
    if(!matrix.size()) {
        //cout << "case 0: matrix size is zero" << endl;
        if(round1)
          complementVec.pop_back();
        return true;
    }
    /*
    k = 0;
    for(int i = 0 ; i < matrix.size()/(inputSize-key) ; i++)
    {
        for(int j = 0; j < inputSize-key ; j++)
        {
            //cout << static_cast<unsigned int>(matrixVec[k]) << " ";

            if(matrix[k] == 0) {
                cout << "00 ";
            } else if(matrix[k] == 1) {
                cout << "01 ";
            } else if(matrix[k] == 2) {
                cout << "10 ";
            } else if(matrix[k] == 3) {
                cout << "11 ";
            }
            k++;
        }
        cout << endl;
    }
    */
    
    //  case row of all 1s, TAUTOLOGY
    //cout << "check case 1" << endl;
    bool check_row_all_1;
    k = 0;
    for(int i = 0 ; i < matrix.size()/(inputSize-key) ; i++)
    {    
        check_row_all_1 = true;
        for(int j = 0 ; j < inputSize-key ; j++)
        {  
            if(matrix[j] != 3) {
                check_row_all_1 = false;
                break;
            }
        }
        if(check_row_all_1) {
            //cout << "case 1 : TAUTOLOGY" << endl;
            if(round1)
                complementVec.pop_back();
            return true;
        }
    }
    
    
    //  case column of 0s, NO TAUTOLOGY
    //cout << "check case 2" << endl;
    bool check_column_of_0s_first, check_column_of_0s_second, column_of_0s = false;
    vector<int> tmpVec;
    for(int i = 0 ; i < inputSize-key ; i++)
    {    
        //if((i+p)%p == key) continue;
        
        if(matrix.size()/(inputSize-key) == 1) {
            if(matrix.back() != 3) {
                isTautology = false;
                tmpVec.push_back(4);
                tmpVec.push_back(1);
                break;
            }
        }
        check_column_of_0s_first = false;
        check_column_of_0s_second = false;
        int left_first = 0, left_second = 0, right_first = 0, right_second = 0;
        for(int j = 0 ; j < matrix.size()/(inputSize-key) ; j++)
        {  
            // right bit all 0 if right_first is 1 and right_second is 0 
            if(matrix[j*(inputSize-key)+i] == 0 || matrix[j*(inputSize-key)+i] == 2) {
                right_first = 1;
            }
            if(matrix[j*(inputSize-key)+i] == 1 || matrix[j*(inputSize-key)+i] == 3) {
                right_second = 1;
            }
            // left bit all 0 if left_first is 1 and left_second is 0
            if(matrix[j*(inputSize-key)+i] == 0 || matrix[j*(inputSize-key)+i] == 1) {
                left_first = 1;
            }
            if(matrix[j*(inputSize-key)+i] == 2 || matrix[j*(inputSize-key)+i] == 3) {
                left_second = 1;
            }
        }
        if(right_first && !right_second) check_column_of_0s_first = true;
        if(left_first && !left_second) check_column_of_0s_second = true;
        if(check_column_of_0s_first || check_column_of_0s_second) {
            isTautology = false;
            tmpVec.push_back(1);
            column_of_0s = true;
            //break;
        }else {
            tmpVec.push_back(4);
        }
    }
    if(!isTautology) {
        //cout << "case 2 : NOT TAUTOLOGY" << endl;
        /*
        if(matrix.back() == 1) {
            complementVec.push_back(0);
        } else if(matrix.back() == 0){
            complementVec.push_back(1);
        }
        */
        /*
        for(int i = 0 ; i < tmpVec.size() ; i++) {
            cout << tmpVec[i] <<" ";
        }
        cout << endl;
        */
        if(!printComplement) {
            complementTautology = false;
            int j = inputSize - complementVec.size();
            //cout << j << endl;
            for(int i = 1 ; i <= j ; i++)
            {    //cout << i << endl;
                if(tmpVec[i] == 4) {
                    complementVec.push_back(3);
                } else {
                    if(matrix[i] == 1) {
                        complementVec.push_back(0);
                    } else if(matrix[i] == 2){
                        complementVec.push_back(1);
                    }
                }
                //cout << i << endl;
            }/*
            for(int i = 0 ; i < complementVec.size() ; i++) {
                cout << +complementVec[i] <<" ";
            }
            cout << endl;
            */
            Complement();
            printComplement = true;
        }
        return false;
    }
    // case one variable and no column of 0s in this field
    //cout << "check case 3" << endl;
    if(key == inputSize-2) {
        //cout << "check case 3" << endl;
        //if((i+p)%p == key) continue;
        bool check_last_column_of_0s_first, check_last_column_of_0s_second;
        check_last_column_of_0s_first = true;
        check_last_column_of_0s_second = true;
        int left_first = 0, left_second = 0, right_first = 0, right_second = 0;
        int i = key+1;
        for(int j = 0 ; j < matrix.size()/(inputSize-key) ; j++)
        {  
            // right bit all 0 if right_first is 1 and right_second is 0 
            if(matrix[j*(inputSize-key)+i] == 0 || matrix[j*(inputSize-key)+i] == 2) {
                right_first = 1;
            }
            if(matrix[j*(inputSize-key)+i] == 1 || matrix[j*(inputSize-key)+i] == 3) {
                right_second = 1;
            }
            // left bit all 0 if left_first is 1 and left_second is 0
            if(matrix[j*(inputSize-key)+i] == 0 || matrix[j*(inputSize-key)+i] == 1) {
                left_first = 1;
            }
            if(matrix[j*(inputSize-key)+i] == 2 || matrix[j*(inputSize-key)+i] == 3) {
                left_second = 1;
            }
        }
        if(right_first && !right_second) check_last_column_of_0s_first = true;
        if(left_first && !left_second) check_last_column_of_0s_second = true;
        if(!(check_column_of_0s_first || check_column_of_0s_second)) {
            //cout << "case 3 : TAUTOLOGY" << endl;
            if(round1)
              complementVec.pop_back();
            return true;
        }
        if(round1)
          complementVec.pop_back();
        return false;
    }
    if(key+1 < inputSize) {
        // if this variable finish (delete 11)
        k = 0;
        tmpMatrix.clear();
        for(int i = 0 ; i < matrix.size() ; i++)
        {    
            if(i%(inputSize-key) == 0) {
                k++;
                continue;
            }else {
                tmpMatrix.push_back(matrix[k]);
                    k++;
            }
        } 
        matrix.clear();
        for(int i = 0 ; i < tmpMatrix.size() ; i++)
        {
            matrix.push_back(tmpMatrix[i]);
        }
        this->checkTautology(matrix, key+1, 0);
        this->checkTautology(matrix, key+1, 1);
        if(!isTautology)
          return false;
    }
    if(round1)
      complementVec.pop_back();
    return true;
}

bool Parser::Containment(vector<unsigned char> matrix) {
    int k = 0;
    for(int i = 0 ; i < matrix.size()/inputSize ; i++)
    {    
        for(int j = 0 ; j < matrixVec2.size() ; j++)
        {  
            matrix[k] = matrix[k] & matrixVec2[j];
            k++;
        }
    }  
    for(int i = 0 ; i < inputSize ; i++)
    {  
        if(matrixVec2[i] == 0) {
            matrixVec2[i] = 3;
        } else if(matrixVec2[i] == 1) {
            matrixVec2[i] = 2;
        } else if(matrixVec2[i] == 2) {
            matrixVec2[i] = 1;
        } else if(matrixVec2[i] == 3) {
            matrixVec2[i] = 0;
        }
    }
    k = 0;
    for(int i = 0 ; i < matrix.size()/inputSize ; i++)
    {    
        for(int j = 0 ; j < matrixVec2.size() ; j++)
        {  
            matrix[k] = matrix[k] | matrixVec2[j];
            k++;
        }
    }  
    
    //showMatrix(matrix, 0);
    bool isContainment1, isContainment2; 
    isTautology = true;
    isContainment1 = checkTautology(matrix, 0, 0);
    if(!isContainment1) {
        return false;
    } else {
        //cout << "2:" << endl;
        isContainment2 = checkTautology(matrix, 0, 1);
        if(isContainment1 && isContainment2) {
            return true;
        } else {
            return false;
        }
    }
    
    return false;
}

void Parser::Complement() {
    for(int i = 0 ; i < lines.size() ; i++) {
        if(lines[i].substr(0,3) == ".p ") {
            outputVec.push_back(".p 1\n");
            //cout << ".p 1" << endl;
            break;
        } else {
            outputVec.push_back(lines[i] + '\n');
            //cout << lines[i] << endl;
        }
    }
    //cout << complementTautology << endl;
    if(complementTautology) {
        for(int i = 0 ; i < inputSize ; i++) {
            //cout << "0" << endl;
            outputVec.push_back("0");
        }
        //cout << endl;
        outputVec.push_back(" 1\n.e\n");
    } else {
        for(int i = 0 ; i < complementVec.size() ; i++) {
            if(complementVec[i] == 0) {
                //cout << "0";
                outputVec.push_back("0");
            } else if(complementVec[i] == 1) {
                //cout << "1";
                outputVec.push_back("1");
            } else if(complementVec[i] == 3) {
                //cout << "1";
                outputVec.push_back("-");
            } 
        }
        //cout << endl;
        outputVec.push_back(" 1\n.e\n");
        //cout << ".e" << endl;
    }
    // write file : function.out
    ofstream writeFile("output.pla");
    ostream_iterator<string> writeFile_iterator(writeFile);
    copy( outputVec.begin(), outputVec.end(), writeFile_iterator );
    writeFile.close();
}