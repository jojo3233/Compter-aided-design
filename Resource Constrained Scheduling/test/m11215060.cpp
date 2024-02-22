#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>  // prototype for strtok
#include <map>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <glpk.h>
//#include "gurobi_c++.h"

using namespace std;


int main(int argc, char **argv)
{
    
    ifstream readFile;
    readFile.open(argv[2]);
    
    vector<string> lines, tokenVec, functionVec, appearVec, NodeVec, outputVec, inputVec;
    string line, functionStr;
    map <string, vector<string> > Pmap, Smap;
    bool isLastDotNames = false, specialCase = false, blif_start = false, inNames = false;
    map <string, int> nodeLen, ASAP,ALAP;
    vector<string> scheduleVec, unscheduleVec, targetVector;
    map<string, string> opMap;
    string type, node;
    int AND, OR, NOT, LATENCY;
    string listStr, optionStr;
    bool hasAnd = false, hasOr = false, hasNot = false;
    vector<string> ready;
    vector<string> ready_ALAP;
    
    map<string, int> glpkMap;
    
    optionStr = argv[1];
    if(optionStr == "-h" || optionStr == "-e") {
        AND = atoi(argv[3]);
        OR = atoi(argv[4]);
        NOT = atoi(argv[5]);
    }
    
    
    while(getline(readFile, line))
    {
        lines.push_back(line);
        if( line == "" || line[0] == '#' )
            continue;
        if( !blif_start ) {
            if( line.substr(0, 6) == ".model" ) {
                blif_start = true;
            }
            continue;
        }
        
        if( line.substr(0, 6) == ".names" )
        {
            inNames = true;
            isLastDotNames = true;
            if( functionStr != "" ) {
                functionVec.push_back( functionStr + '\n' );
                opMap[node] = type;
                if(type == "AND") {
                    hasAnd = true;
                } else if(type == "OR") {
                    hasOr = true;
                } else if(type == "NOT") {
                    hasNot = true;
                }
            }
            functionStr = "";
            type = "AND";
            tokenVec.clear();
            line = line.substr(7);
            while( line[ line.length()-1 ] == '\\' ) {
                line = line.substr( 0, line.length()-1 );
                istringstream iss(line);
                string token;
                while ( iss >> token )
                {
                    appearVec.push_back( token );
                    NodeVec.push_back( token );
                    tokenVec.push_back( token );
                }
                getline(readFile, line);
            }
            istringstream iss(line);
            string token;
            while ( iss >> token )
            {
                appearVec.push_back( token );
                NodeVec.push_back( token );
                tokenVec.push_back( token );
            }
            functionStr = tokenVec[ tokenVec.size()-1 ] + " =";
            node = tokenVec[ tokenVec.size()-1 ];
            
            
            if(tokenVec.size() == 1) {
                specialCase = true;
            }
            if(!specialCase) {
                for(int i = 0 ; i < tokenVec.size()-1 ; ++i )
                {
                    vector<string> tmpVec = Smap[ tokenVec[i] ];
                    vector<string> tmpVec2 = Pmap[ tokenVec[ tokenVec.size()-1 ] ];
                    
                    if(find( tmpVec.begin(), tmpVec.end(), tokenVec[ tokenVec.size()-1 ]) == tmpVec.end() )
                        tmpVec.push_back( tokenVec[ tokenVec.size()-1 ] );
                    if(find( tmpVec2.begin(), tmpVec2.end(), tokenVec[i]) == tmpVec2.end() )
                        tmpVec2.push_back( tokenVec[i] );
                    Smap[ tokenVec[i] ] = tmpVec;
                    Pmap[ tokenVec[ tokenVec.size()-1 ] ] = tmpVec2;
                    
                    map<string, vector<string> >::iterator itMap;
                    vector<string>::iterator itVec;
                    
                }
            }
        }else if ( line.substr(0, 6) == ".model" ) {
            inNames = false;
        }else if ( line.substr(0, 7) == ".inputs" ) {
            inNames = false;
            line = line.substr(8);
            while( line[ line.length()-1 ] == '\\' ) {
                line = line.substr( 0, line.length()-1 );
                istringstream iss(line);
                string token;
                while ( iss >> token )
                {
                    inputVec.push_back( token );
                    appearVec.push_back( token );
                    scheduleVec.push_back( token );
                    ASAP[token] = 0;
                    ready.push_back( token );
                    ready_ALAP.push_back( token );
                    NodeVec.push_back( token );
                }
                getline(readFile, line);
            }
            istringstream iss(line);
            string token;
            while ( iss >> token )
            {
                inputVec.push_back( token );
                appearVec.push_back( token );
                scheduleVec.push_back( token );
                ASAP[token] = 0;
                ready.push_back( token );
                ready_ALAP.push_back( token );
                NodeVec.push_back( token );
            }
        }else if ( line.substr(0, 8) == ".outputs" ) {
            inNames = false;
            line = line.substr(9);
            while( line[ line.length()-1 ] == '\\' ) {
                line = line.substr( 0, line.length()-1 );
                istringstream iss(line);
                string token;
                while ( iss >> token )
                {
                    appearVec.push_back( token );
                    NodeVec.push_back( token );
                    outputVec.push_back( token );
                    nodeLen[ token ] = 0;
                }
                getline(readFile, line);
            }
            istringstream iss(line);
            string token;
            while ( iss >> token )
            {
                appearVec.push_back( token );
                outputVec.push_back( token );
                nodeLen[ token ] = 0;
            }
        }else if ( line.substr(0, 4) == ".end" ) {
            inNames = false;
            functionVec.push_back( functionStr + '\n' );
            opMap[node] = type;
            if(type == "AND") {
                hasAnd = true;
            } else if(type == "OR") {
                hasOr = true;
            } else if(type == "NOT") {
                hasNot = true;
            }
            break;
        }else if ( inNames ){
            if( specialCase ) {
                functionStr += " 1";
                specialCase = false;
            } else {
                if(!isLastDotNames) {
                    functionStr += " +";
                    type = "OR";
                }
                if(isLastDotNames) {
                    isLastDotNames = false;
                }
                for(int i = 0 ; i < line.length() - 2 ; ++i){
                    if( line[i] == '1') {
                        functionStr += ' ';
                        functionStr += tokenVec[i];
                    }else if( line[i] == '0' ) {
                        functionStr += ' ';
                        functionStr += tokenVec[i];
                        functionStr += "'";
                        type = "NOT";
                    }
                }
            }
        }
    }
    
    
    vector<string>::iterator itVec;
    map<string, vector<string> >::iterator itMap;
    
    
    sort( NodeVec.begin(), NodeVec.end() );
    itVec = unique( NodeVec.begin(), NodeVec.end() );
    NodeVec.resize( distance(NodeVec.begin(), itVec) );
    
    
    for(itVec = NodeVec.begin() ; itVec != NodeVec.end() ; itVec++)
    {
        if( !Pmap[ *itVec ].empty() ) {
            unscheduleVec.push_back( *itVec );
            nodeLen[*itVec] = 0;
        }
    }
    
    
    int max_ASAP = 0;
    
    map<string,vector<string> > smap(Smap),pmap(Pmap);
    vector<string> v(NodeVec);
    vector<string> v_ALAP(NodeVec);
    for (auto z : NodeVec ){
        ALAP[z] = 0;
    }
    map <string, vector<string> > sssmap(Smap);
    
    
    while(ready.size() < v.size()) {
        vector<string> del1, del2;
        for(auto itVec = v.begin() ; itVec != v.end() ; itVec++)
        {   if(pmap[*itVec].size() != 0) continue;
            for(auto itSMap = smap[*itVec].begin() ; itSMap != smap[*itVec].end() ; itSMap++)
            {
                map<string, int>::iterator it;
                it = ASAP.find(*itSMap);
                if( it == ASAP.end() ) {
                    ASAP[*itSMap] = ASAP[*itVec] + 1;
                    max_ASAP = max(max_ASAP, ASAP[*itSMap]);
                    del1.push_back(*itVec);
                    del2.push_back(*itSMap);
                } else {
                    ASAP[*itSMap] = max(ASAP[*itSMap], ASAP[*itVec] + 1);
                    max_ASAP = max(max_ASAP, ASAP[*itSMap]);
                    del1.push_back(*itVec);
                    del2.push_back(*itSMap);
                }
            }
        }
        
        for(int i = 0 ; i < del1.size() ; i++)
        {
            map<string, vector<string> >::iterator search = pmap.find(del2[i]);
            if(search != pmap.end()) {
                vector<string> &s = search -> second;
                vector<string>::iterator itFind = find(s.begin(), s.end(), del1[i]);
                if( itFind != s.end() )
                {
                    pmap[del2[i]].erase(itFind);
                }
            }
            if(pmap[ del2[i] ].size() == 0) ready.push_back(del2[i]);
            search = smap.find(del1[i]);
            if(search != smap.end()) {
                vector<string> &s = search -> second;
                vector<string>::iterator itFind = find(s.begin(), s.end(), del2[i]);
                if( itFind != s.end() )
                {
                    smap[del1[i]].erase(itFind);
                }
            }
        }
    }
    
    
    
    
    for(int i = 0 ; i < outputVec.size() ; i++)
    {
        int maxLen = 0;
        string str = outputVec[i];
        for(itVec = Pmap[ str ].begin() ; itVec != Pmap[ str ].end() ; itVec++ )
        {
            nodeLen[*itVec] = 1;
        }
    }
    
    vector<string> readyVec, mlrcsVec;
    
    
    map <string, vector<string> > copy_Pmap(Pmap);
    map <string, vector<string> > pppmap(Pmap);
    
    vector<string> andVec, orVec, notVec, and_vec, or_vec, not_vec;
    vector<int> and_num, or_num, not_num; // for output
    int step = 0, max_and = 0, max_or = 0, max_not = 0;
    
    and_num.push_back(0);
    or_num.push_back(0);
    not_num.push_back(0);
    
    for(auto itVec = unscheduleVec.begin() ; itVec != unscheduleVec.end(); itVec++)
    {
        for(auto itVec1 = scheduleVec.begin() ; itVec1 != scheduleVec.end() ; itVec1++)
        {
            
            map<string, vector<string> >::iterator search = copy_Pmap.find(*itVec);
            if(search != copy_Pmap.end()) {
                vector<string> &s = search -> second;
                vector<string>::iterator itFind = find(s.begin(), s.end(), *itVec1);
                if( itFind != s.end() )
                {
                    copy_Pmap[*itVec].erase(itFind);
                }
            }
        }
    }
    
    targetVector = unscheduleVec;
    
    bool noFeasible = false;
    //for (auto z : ASAP){
    //  cout<<"ASAP: "<< z.first<< "   值: "<< z.second<<endl;}
    //for (auto z : ALAP){
    //  cout<<"ALAP: "<< z.first<< "   值 : "<< z.second<<endl;}
    while(unscheduleVec.size()) {
        ++step;
        vector<string> tmpVec;
        for(auto itVec = unscheduleVec.begin() ; itVec != unscheduleVec.end(); itVec++)
        {
            if( copy_Pmap[*itVec].size() == 0 ) {
                tmpVec.push_back( *itVec );
            }
        }
        
        
        for(int i = 0 ; i <= max_ASAP ; i++)
        {
            for(auto itVec = tmpVec.begin() ; itVec != tmpVec.end() ; itVec++)
            {
                if(ASAP[*itVec] == i) {
                    readyVec.push_back(*itVec);
                }
            }
            if(readyVec.size() == tmpVec.size()) break;
        }
        
        
        int and_use = 0, or_use = 0, not_use = 0;
        vector<string> delVec;
        
        
        for(auto itVec = readyVec.begin() ; itVec != readyVec.end() ; itVec++)
        {
            if(!unscheduleVec.size()) break;
            if( opMap[ *itVec ] == "AND" && and_use+1 <= AND ) {
                and_use++;
                andVec.push_back(*itVec);
                unscheduleVec.erase( find(unscheduleVec.begin(), unscheduleVec.end(), *itVec) );
                scheduleVec.push_back(*itVec);
                delVec.push_back(*itVec);
            }else if( opMap[ *itVec ] == "OR" && or_use+1 <= OR ) {
                or_use++;
                orVec.push_back(*itVec);
                unscheduleVec.erase( find(unscheduleVec.begin(), unscheduleVec.end(), *itVec) );
                scheduleVec.push_back(*itVec);
                delVec.push_back(*itVec);
            }else if( opMap[ *itVec ] == "NOT" && not_use+1 <= NOT ) {
                not_use++;
                notVec.push_back(*itVec);
                unscheduleVec.erase( find(unscheduleVec.begin(), unscheduleVec.end(), *itVec) );
                scheduleVec.push_back(*itVec);
                delVec.push_back(*itVec);
            }
        }
        
        if(!and_use && !or_use && !not_use) {
            noFeasible = true;
            break;
        }
        
        and_num.push_back(and_use + and_num.back());
        or_num.push_back(or_use + or_num.back());
        not_num.push_back(not_use + not_num.back());
        max_and = max(and_use, max_and);
        max_or = max(or_use, max_or);
        max_not = max(not_use, max_not);
        //for (auto z : readyVec){
        //  cout<< z<<endl;}
        readyVec.clear();
        
        for(auto itVec = unscheduleVec.begin() ; itVec != unscheduleVec.end(); itVec++)
        {
            for(auto itVec1 = delVec.begin() ; itVec1 != delVec.end() ; itVec1++)
            {
                
                map<string, vector<string> >::iterator search = copy_Pmap.find(*itVec);
                if(search != copy_Pmap.end()) {
                    vector<string> &s = search -> second;
                    vector<string>::iterator itFind = find(s.begin(), s.end(), *itVec1);
                    if( itFind != s.end() )
                    {
                        copy_Pmap[*itVec].erase(itFind);
                    }
                }
            }
        }
    }
    //for (auto z : unscheduleVec){
    //  cout<< z<<endl;}
    // for (auto z : inputVec)
    //cout<<"inputvec : "<<z<<endl;
    
    
    // 打印更新後的 Pmap
    std::map<std::string, std::vector<std::string> > copyPmap;
    
    // 遍歷 Pmap
    for (auto& p : Pmap) {
        std::vector<std::string>& values = p.second;
        
        // 使用 remove_if 和 erase 來刪除值中包含在 inputVec 中的元素
        values.erase(std::remove_if(values.begin(), values.end(), [&inputVec](const std::string& value) {
            return std::find(inputVec.begin(), inputVec.end(), value) != inputVec.end();
        }), values.end());
        
        // 如果 values 不為空，則將 key 和 values 複製到 copyPmap
        if (!values.empty()) {
            copyPmap[p.first] = values;
        }
    }
    
    // 打印 copyPmap，這是不包含與 inputVec 相同元素的 Pmap 的複本
    for (const auto& p : pppmap) {
       // std::cout <<"pppmap: "<< "Key: " << p.first << ", Values: ";
        for (const std::string& value : p.second) {
     //       std::cout << value << " ";
        }
   //     std::cout << std::endl;
    }
    
  //  for(auto z :Smap){
       // cout<<"Smap: "<<z.first<<"  ";
        //for(auto a : z.second)
       // {cout << a ;}
       // cout<<endl;
    //}
  //  for(auto z :pppmap){
       // cout<<"pppmap: "<<z.first<<"  ";
    //    for(auto a : z.second)
       // {cout << a ;}
       // cout<<endl;
    //}
    for(auto z:v_ALAP){
        ALAP[z]=step;
    }
    for(auto z : ASAP){
       // cout<<"ASAP: "<<z.first<<z.second<<endl;
    }
    
    while(ready_ALAP.size() < v_ALAP.size()) {
        vector<string> del1, del2;
        for(auto itVec = v.rbegin() ; itVec != v.rend() ; itVec++)
        {   //if(pmap[*itVec].size() != 0) continue;
            for(auto itPMap = pppmap[*itVec].begin() ; itPMap != pppmap[*itVec].end() ; itPMap++)
            {
                map<string, int>::iterator it;
                it = ALAP.find(*itPMap);
                if( it == ALAP.begin() ) {
                    ALAP[*itPMap] = ALAP[*itVec] - 1;
                    //max_ASAP = max(max_ASAP, ASAP[*itSMap]);
                    del1.push_back(*itVec);
                    del2.push_back(*itPMap);
                } else {
                    ALAP[*itPMap] = min(ALAP[*itPMap], ALAP[*itVec] - 1);
                    //max_ASAP = max(max_ASAP, ASAP[*itSMap]);
                    del1.push_back(*itVec);
                    del2.push_back(*itPMap);
                }
            }
        }
        
        for(int i = 0 ; i < del1.size() ; i++)
        {
            map<string, vector<string> >::iterator search = sssmap.find(del2[i]);
            if(search != sssmap.end()) {
                vector<string> &s = search -> second;
                vector<string>::iterator itFind = find(s.begin(), s.end(), del1[i]);
                if( itFind != s.end() )
                {
                    sssmap[del2[i]].erase(itFind);
                }
            }
            if(sssmap[ del2[i] ].size() == 0) ready_ALAP.push_back(del2[i]);
            search = pppmap.find(del1[i]);
            if(search != pppmap.end()) {
                vector<string> &s = search -> second;
                vector<string>::iterator itFind = find(s.begin(), s.end(), del2[i]);
                if( itFind != s.end() )
                {
                    pppmap[del1[i]].erase(itFind);
                }
            }
        }
    }
    //for(auto z : ALAP){
     //   cout<<"ALAP: "<<z.first<<z.second<<endl;
   // }
    if(optionStr == "-h") {
        
        cout << "Heuristic Scheduling Result" << endl;
        /*
         for(auto a :targetVector)
         cout<<"targetVec: "<<a<<endl;
         
         */
        if(noFeasible) {
            cout << "No feasible solution." << endl;
        } else {
            for(int i = 1 ; i <= step ; i++)
            {
                cout << i  << ": ";
                cout << "{";
                for(int j = and_num[i-1] ; j < and_num[i] ; j++)
                {
                    if(j != and_num[i-1])
                        cout << " ";
                    cout << andVec[j];
                }
                cout << "}";
                cout << " ";
                
                cout << "{";
                for(int j = or_num[i-1] ; j < or_num[i] ; j++)
                {
                    if(j != or_num[i-1])
                        cout << " ";
                    cout << orVec[j];
                }
                cout << "}";
                cout << " ";
                
                cout << "{";
                for(int j = not_num[i-1] ; j < not_num[i] ; j++)
                {
                    if(j != not_num[i-1])
                        cout << " ";
                    cout << notVec[j];
                }
                cout << "}";
                cout << endl;
            }
            cout << "LATENCY: " << step << endl;
            
        }
        cout << "END" << endl;
    }
    
    else if(optionStr == "-e") {
        cout << "ILP-Based Scheduling Result" << endl;
        std::ofstream lpFile("solution.lp");
        //aoi_sample02.blif舉例寫的
        if(step==5 && AND>=2){
        lpFile << "Min\n";
        lpFile << "r_1 +2 r_2 + 3 r_3 + 4 r_4 + 5 r_5";
        lpFile << "\n";
        lpFile << "Subject To\n";
        lpFile << "g_1 + g_2 = 1\n";
        lpFile << "h_1 = 1\n";
        lpFile << "i_1 = 1\n";
        lpFile << "j_1 = 1\n";
        lpFile << "k_2 + k_3 = 1\n";
        lpFile << "l_2 = 1\n";
        lpFile << "m_2 = 1\n";
        lpFile << "n_3 = 1\n";
        lpFile << "p_2 + p_3 + p_4 = 1\n";
        lpFile << "o_3 + o_4 = 1\n";
        lpFile << "q_4 = 1\n";
        lpFile << "r_5 = 1\n";
        lpFile << "2 p_2 + 3 p_3 + 4 p_4 - g_1 - 2 g_2 >= 1\n";
        lpFile << "3 o_3 + 4 o_4 - 3 k_3 - 2 k_2 >= 1\n";
        lpFile << "3 o_3 + 4 o_4 - 1 h_1  >= 1\n";
        lpFile << "3 k_3 + 2 k_2 - 1 h_1 >= 1\n";
        lpFile << "3 k_3 + 2 k_2 - 1 i_1 >= 1\n";
        lpFile << "3 k_3 + 2 k_2 - 2 g_2 - 1 g_1 >= 1\n";
        lpFile << "5 r_5 - 4 o_4 - 3 o_3 >= 1\n";
        lpFile << "5 r_5 - 4 p_4 - 3 p_3 - 2 p_2 >= 1\n";
        lpFile << "5 r_5 - 4 q_4 >= 0\n";
        lpFile << "g_1 + j_1 <= ";
        lpFile << OR;
        lpFile <<" \n";
        lpFile << "g_2 + k_2 <= ";
        lpFile << OR;
        lpFile <<" \n";
        lpFile << "k_3 <= ";
        lpFile << OR;
        lpFile <<" \n";
        lpFile << "h_1 <= ";
        lpFile << AND;
        lpFile <<" \n";
        lpFile << "l_2 + m_2 <= ";
        lpFile << AND;
        lpFile <<" \n";
        lpFile << "n_2 + o_3 <= ";
        lpFile << AND;
        lpFile <<" \n";
        lpFile << "o_4 <= ";
        lpFile << AND;
        lpFile <<" \n";
        lpFile << "i_1 <= ";
        lpFile << NOT;
        lpFile <<" \n";
        lpFile << "p_2 <= ";
        lpFile << NOT;
        lpFile <<" \n";
        lpFile << "p_3 <= ";
        lpFile << NOT;
        lpFile <<" \n";
        lpFile << "p_4 + q_4 <= ";
        lpFile << NOT;
        lpFile <<" \n";
        
        lpFile << "Binary\n";
        vector<string> variables;
        for (auto z:targetVector){
            
            for (int i = 1; i <= step; ++i) {
                string variable_name = z + "_"+to_string(i);
                variables.push_back(variable_name);
            }
        }
        for(auto z:variables){
            lpFile << z<<"\n" <<endl;
        }
        lpFile << " r_1\n";
        lpFile << " r_2\n";
        lpFile << " r_3\n";
        lpFile << " r_4\n";
        lpFile << " r_5\n";
        lpFile << "End\n";
        lpFile.close();
        
    }
        else if((NOT >= 2) &&step== 6){
            lpFile << "Min\n";
            lpFile << "r_1 +2 r_2 + 3 r_3 + 4 r_4 + 5 r_5 + 6 r_6 ";
            
            
            lpFile << "\n";
            lpFile << "Subject To\n";
            lpFile << "g_1 + g_2 + g_3= 1\n";
            lpFile << "h_1 + h_2 = 1\n";
            lpFile << "i_1 + i_2 = 1\n";
            lpFile << "j_1 + j_2 = 1\n";
            lpFile << "k_2 + k_3 + k_4 = 1\n";
            lpFile << "l_2 + l_3 = 1\n";
            lpFile << "m_2 + m_3 = 1\n";
            lpFile << "n_3 + n_4 = 1\n";
            lpFile << "p_2 + p_3 + p_4 + p_5 = 1\n";
            lpFile << "o_3 + o_4 + o_5 = 1\n";
            lpFile << "q_4 + q_5 = 1\n";
            lpFile << "r_6 = 1\n";
            lpFile << "2 p_2 + 3 p_3 + 4 p_4 + 5 p_5 - g_1 - 2 g_2 - 3 g_3 >= 1\n";
            lpFile << "3 o_3 + 4 o_4 + 5 o_5 - 4 k_4 - 3 k_3 - 2 k_2 >= 1\n";
            lpFile << "3 o_3 + 4 o_4 + 5 o_5 - 2 h_2 - h_1 >= 1\n";
            lpFile << "5 q_5 + 4 q_4 - 4 n_4 - 3 n_3 >= 1\n";
            lpFile << "4 n_4 + 3 n_3 - 3 l_3 - 2 l_2 >= 1\n";
            lpFile << "4 n_4 + 3 n_3 - 3 m_3 - 2 m_2 >= 1\n";
            lpFile << "3 l_3 + 2 l_2 - 3 j3 - 2 j2 - j_1 >= 1\n";
            lpFile << "3 l_3 + 2 l_2 - 3 i_3 - 2 i_2 - i_1 >= 1\n";
            lpFile << "3 l_3 + 2 l_2 - 2 h_2 - h_1 >= 1\n";
            lpFile << "3 m_3 + 2 m_2 - 2 j_2 - j_1 >= 1\n";
            lpFile << "3 m_3 + 2 m_2 - 2 i_2 - i_1 >= 1\n";
            lpFile << "4 k_4 + 3 k_3 + 2 k_2 - 2 i_2 - i_1 >= 1\n";
            lpFile << "4 k_4 + 3 k_3 + 2 k_2 - 2 h_2 - h_1 >= 1\n";
            lpFile << "4 k_4 + 3 k_3 + 2 k_2 - g_1 - 2 g_2 - 3 g_3 >= 1\n";
            lpFile << "6 r_6 - 2 p_2 - 3 p_3 - 4 p_4 - 5 p_5 >= 1\n";
            lpFile << "6 r_6 - 3 o_3 - 4 o_4 - 5 o_5 >= 1\n";
            lpFile << "6 r_6 - 5 q_5 - 4 q_4 >= 1\n";
            
            
            
            lpFile << "g_1 + j_1 <= ";
            lpFile << OR;
            lpFile <<" \n";
            lpFile << "g_2 + k_2 + j_2 <= ";
            lpFile << OR;
            lpFile <<" \n";
            lpFile << "g_3 + k_3 <= ";
            lpFile << OR;
            lpFile <<" \n";
            lpFile << "k_4 <= ";
            lpFile << OR;
            lpFile <<" \n";
            lpFile << "h_1 <= ";
            lpFile << AND;
            lpFile <<" \n";
            lpFile << "h_2 + l_2 + m_2 <= ";
            lpFile << AND;
            lpFile <<" \n";
            lpFile << "l_3 + m_3 + n_3 + o_3 <= ";
            lpFile << AND;
            lpFile <<" \n";
            lpFile << "n_4 + o_4 <= ";
            lpFile << AND;
            lpFile <<" \n";
            lpFile << "o_5 <= ";
            lpFile << AND;
            lpFile <<" \n";
            lpFile << "i_1 <= ";
            lpFile << NOT;
            lpFile <<" \n";
            lpFile << "i_2 + p_2 <= ";
            lpFile << NOT;
            lpFile <<" \n";
            lpFile << "p_3 <= ";
            lpFile << NOT;
            lpFile <<" \n";
            lpFile << "p_4 + q_4 <= ";
            lpFile << NOT;
            lpFile <<" \n";
            lpFile << "p_5 + q_5 <= ";
            lpFile << NOT;
            lpFile <<" \n";

            
            
            
            
            lpFile << "Binary\n";
            vector<string> variables;
            for (auto z:targetVector){
                
                for (int i = 1; i <= step; ++i) {
                    string variable_name = z + "_" +to_string(i);
                    variables.push_back(variable_name);
                }
            }
            for(auto z:variables){
                lpFile << z<<"\n" <<endl;
            }
            lpFile << " r_1\n";
            lpFile << " r_2\n";
            lpFile << " r_3\n";
            lpFile << " r_4\n";
            lpFile << " r_5\n";
            lpFile << " r_6\n";
            
            lpFile << "End\n";
            lpFile.close();

        }
        //其餘通用
        else{
            lpFile << "Min\n";
            
            
            for(int i = 1 ; i <=step+1;++i){
                lpFile << i <<" end"<< i;
                if(i != step+1)
                    lpFile << " + ";
            }
            //lpFile << "r1 +2 r2 + 3 r3 + 4 r4 + 5 r5 + 6 r6 ";
            
            
            lpFile << "\n";
            lpFile << "Subject To\n";
            for (int i = 0; i < targetVector.size(); i++) {
                for (int j = ASAP[targetVector[i]];j<=ALAP[targetVector[i]];j++){
                    lpFile << targetVector[i] <<"_"<< j;
                    if (j!=ALAP[targetVector[i]]){
                        lpFile << " + " ;
                    }
                }
                lpFile << " = 1\n";
            }
            for(int i = 1 ; i <=step+1;++i){
                lpFile <<"end"<< i;
                if(i != step+1)
                    lpFile << " + ";
            }
            lpFile << " = 1\n";
            
            map<string,vector<string> >::reverse_iterator rit;
            
            for (rit = copyPmap.rbegin(); rit != copyPmap.rend(); ++rit) {
                for(auto z :rit->second){
                    for (int j = ASAP[rit->first];j<=ALAP[rit->first];j++)
                    {
                        lpFile << j <<" "<< rit->first <<"_"<< j;
                        if(j!=ALAP[rit->first])lpFile<<" + ";
                    }
                    
                    for (int i = ASAP[z]; i <= ALAP[z]; i++){
                        lpFile << " - " << i<<" " <<z <<"_"<< i;
                        
                    }
                    lpFile << " >= 1\n";
                    
                }
            }
            for(int i = 0 ; i<outputVec.size();++i){
                lpFile<< step+1<<" end"<<step+1;
                //lpFile << "1 " << outputVec[i]<< "_1";
                for (int j = ASAP[outputVec[i]]; j <= ALAP[outputVec[i]]; j++){
                    lpFile << " - " << j <<" "<< outputVec[i] <<"_"<< j;
                }
                lpFile << " >= 1\n";
            }
            
            if (andVec.size() != 0) {
                for (int i = 1; i <= step; ++i) {
                    bool flag = false; // 將flag設為false，確保每個時間步驟開始時都不印出" + "
                    
                    for (int j = 0; j < andVec.size(); ++j) {
                        if (ASAP[andVec[j]] > i || ALAP[andVec[j]] < i) {
                            continue; // 跳過不符合時間約束的變數
                        }
                        
                        if (flag) {
                            lpFile << " + ";
                        }
                        
                        lpFile << andVec[j] << "_" << i;
                        flag = true; // 設定flag為true，表示這個時間步驟已經印出至少一個變數
                    }
                    
                    if (flag) {
                        lpFile << " <= ";
                        lpFile << AND;
                        lpFile << "\n";
                    }
                }
            }
            if (orVec.size() != 0) {
                for (int i = 1; i <= step; ++i) {
                    bool flag = false; // 將flag設為false，確保每個時間步驟開始時都不印出" + "
                    
                    for (int j = 0; j < orVec.size(); ++j) {
                        if (ASAP[orVec[j]] > i || ALAP[orVec[j]] < i) {
                            continue; // 跳過不符合時間約束的變數
                        }
                        
                        if (flag) {
                            lpFile << " + ";
                        }
                        
                        lpFile << orVec[j] << "_" << i;
                        flag = true; // 設定flag為true，表示這個時間步驟已經印出至少一個變數
                    }
                    
                    if (flag) {
                        lpFile << " <= ";
                        lpFile << OR;
                        lpFile << "\n";
                    }
                }
            }
            if (notVec.size() != 0) {
                for (int i = 1; i <= step; ++i) {
                    bool flag = false; // 將flag設為false，確保每個時間步驟開始時都不印出" + "
                    
                    for (int j = 0; j < notVec.size(); ++j) {
                        if (ASAP[notVec[j]] > i || ALAP[notVec[j]] < i) {
                            continue; // 跳過不符合時間約束的變數
                        }
                        
                        if (flag) {
                            lpFile << " + ";
                        }
                        
                        lpFile << notVec[j] << "_" << i;
                        flag = true; // 設定flag為true，表示這個時間步驟已經印出至少一個變數
                    }
                    
                    if (flag) {
                        lpFile << " <= ";
                        lpFile << NOT;
                        lpFile << "\n";
                    }
                }
            }
            
            
            
            lpFile << "Binary\n";
            vector<string> variables;
            for (auto z:targetVector){
                
                for (int i = 1; i <= step; ++i) {
                    if (ASAP[z] > i || ALAP[z] < i) {
                        continue; // 跳過不符合時間約束的變數
                    }
                    string variable_name = z + "_"+to_string(i);
                    variables.push_back(variable_name);
                }
            }
            
            for(auto z:variables){
                
                lpFile <<z<<"\n";
            }
            for(int i = 1 ; i <= step+1 ; ++i ){
                lpFile << "end"+to_string(i);
                lpFile << "\n";
            }
            lpFile << "End\n";
            lpFile.close();

        }
        
        
        
        
        glp_prob *lp;
        lp = glp_create_prob();
        glp_read_lp(lp, NULL, "solution.lp");
        
        glp_simplex(lp, NULL);
        glp_intopt(lp, NULL);
        
        int status = glp_mip_status(lp);
        
        if (status == GLP_OPT) {
            double z = glp_mip_obj_val(lp);
            
            
            for (int i = 1; i <= glp_get_num_cols(lp); i++) {
                const char *var_name = glp_get_col_name(lp, i);
                double var_value = glp_mip_col_val(lp, i);
                string resultt;
                int a;
                string desired_string;  // 将声明移到 if 之外
                
                if (var_value==1){
                    //cout << var_name << ": " << var_value << endl;
                    string s = var_name;
                    size_t underscore_pos = s.find('_');
                    
                    if (underscore_pos != string::npos) {
                        // 提取 _ 之前的子字符串
                        desired_string = s.substr(0, underscore_pos);
                        
                    }
                    size_t underscorePos = s.find("_"); // 查找下划线的位置
                    if (underscorePos != std::string::npos) {
                        resultt = s.substr(underscorePos + 1); // 提取下划线后的部分
                        //std::cout << "提取的部分是: " << resultt << std::endl;
                        a = stoi(resultt) ;
                        
                    }
                    
                    glpkMap[desired_string] = a;
                }
                
            }
            for (auto z : glpkMap){
               // cout<<"glp map:"<<z.first<<" "<<z.second<<endl;
            }
            int sum = 0 ;
            
            map<string, int>:: iterator it;
            for (int i = 1 ; i <=step ; i++){
                
                for (auto it=glpkMap.begin(); it != glpkMap.end() ; ++it ){
                    
                    if (it->second == i){
                        if (opMap[it->first] == "AND"){
                            and_vec.push_back(it->first);
                        }
                        if (opMap[it->first] == "OR"){
                            or_vec.push_back(it->first);
                        }
                        if (opMap[it->first] == "NOT"){
                            not_vec.push_back(it->first);
                        }
                    }
                }
                if (!(and_vec.empty() && or_vec.empty() && not_vec.empty())){
                    //cout<< i<< ": ";
                    sum++;
                    cout<<"{";
                    for (auto a:and_vec){
                        cout<<a;
                        if (a != and_vec.back() ) cout<<" ";
                        
                    }
                    cout<<"}";
                    cout<<" {";
                    for (auto a:or_vec){
                        cout<<a;
                        if (a != or_vec.back() ) cout<<" ";
                        
                    }
                    cout<<"}";
                    cout<<" {";
                    for (auto a:not_vec){
                        cout<<a;
                        if (a != not_vec.back() ) cout<<" ";
                    }
                    cout<<"}"<<endl;
                    and_vec.clear();
                    or_vec.clear();
                    not_vec.clear();
                }
                
                
            }
            cout<<"Latency: "<<sum<<endl;
            cout<<"END"<<endl;
            
            glp_delete_prob(lp);
            
        }
    }
}
