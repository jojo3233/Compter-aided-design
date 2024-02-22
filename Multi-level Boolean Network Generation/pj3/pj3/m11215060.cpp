#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <map>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <set>
#include <map>
#include <unordered_set>
using namespace std;
pair<string, string> splitEquation(const string& equation) {
    size_t equalsPos = equation.find('=');
    if (equalsPos == string::npos) {
        return {"", ""}; // 或者可以處理錯誤
    }

    string left = equation.substr(0, equalsPos);
    string right = equation.substr(equalsPos + 1);

    return {left, right};
}

// 分割函数，用于将多项式分割为项的集合
vector<string> splitPolynomial(const string& poly) {
    vector<string> terms;
    istringstream iss(poly);
    string term;
    while (getline(iss, term, '+')) {
        terms.push_back(term);
    }
    return terms;
}

vector<string> splitString(const string& str) {
    vector<string> result;
    string currentElement;
    for (size_t i = 0; i < str.length(); ++i) {
        char ch = str[i];
        currentElement += ch;
        if (i + 1 < str.length()) {
            char nextChar = str[i + 1];
            // 如果下一個字符是引號，則將其添加到當前元素
            if (nextChar == '\'') {
                currentElement += nextChar;
                ++i;
            }
            // 如果下一個字符是字母或字符串結尾，則將當前元素加入到結果中
            if (i + 1 == str.length() || isalpha(str[i + 1])) {
                result.push_back(currentElement);
                currentElement.clear();
            }
        }
    }
    // 添加最後的元素（如果有的話）
    if (!currentElement.empty()) {
        result.push_back(currentElement);
    }
    return result;
}

// 检查a是否包含b中的所有字符
bool contains(const string& a, const string& b) {
    vector<string> splitA = splitString(a);
    vector<string> splitB = splitString(b);
    set<string> setA;
    for (auto c : splitA) {
        setA.insert(c);
    }
    for (auto charB : splitB) {
        if (setA.find(charB) == setA.end())
            return false; // 找到目标元素，返回 true
    }

    return true;
}

// 从a中移除b中的所有字符
string removeChars(const string& a, const string& b) {
    unordered_set<string> charsToRemove;
    vector<string> splitB = splitString(b);

    // 将 b 中的每个元素添加到要移除的字符集合中
    for (const auto& ch : splitB) {
        charsToRemove.insert(ch);
    }

    string resultString;
    vector<string> splitA = splitString(a);

    // 遍历 a 的每个元素，只有当它不在要移除的字符集合中时才加入到结果字符串中
    for (const auto& ch : splitA) {
        if (charsToRemove.find(ch) == charsToRemove.end()) {
            resultString += ch;
        }
    }

    return resultString;
}

// 连接函数，用于将项的集合连接为一个多项式字符串
string joinPolynomial(const vector<string>& terms) {
    string poly;
    for (const auto& term : terms) {
        if (!poly.empty()) poly += "+";
        poly += term;
    }
    return poly.empty() ? "0" : poly;
}

set<string> findCommonLiterals(const vector<string>& terms) {
    vector<set<string>> splitTerms;

    // 使用 splitString 函數分割每個項
    for (const auto& term : terms) {
        set<string> chars;
        auto splitTerm = splitString(term);
        for (const auto& ch : splitTerm) {
            if (!ch.empty()) {
                chars.insert(ch);
            }
        }
        splitTerms.push_back(chars);
    }

    // 尋找所有集合的交集
    set<string> common = splitTerms.empty() ? set<string>{} : splitTerms[0];
    for (size_t i = 1; i < splitTerms.size(); ++i) {
        set<string> current = splitTerms[i];
        set<string> intersection;
        set_intersection(
            common.begin(), common.end(),
            current.begin(), current.end(),
            inserter(intersection, intersection.begin())
        );
        common = intersection;
    }

    return common;
}


vector<string> findCommonElements(const vector<vector<string>>& vectors) {
    vector<string> result;
    if (vectors.empty()) {
        return result; // 如果没有提供任何向量，则返回空结果
    }

    if (vectors.size() == 1) {
        // 如果只有一个向量，则直接返回其内容
        return vectors[0];
    }

    // 创建一个 set 以存储相同的元素
    set<string> commonElements(vectors[0].begin(), vectors[0].end());

    for (size_t i = 1; i < vectors.size(); ++i) {
        set<string> currentSet(vectors[i].begin(), vectors[i].end());
        set<string> temp;

        // 找到当前 vector 与 commonElements 的交集
        set_intersection(
            commonElements.begin(), commonElements.end(),
            currentSet.begin(), currentSet.end(),
            inserter(temp, temp.begin())
        );

        commonElements = temp;
    }

    result.assign(commonElements.begin(), commonElements.end());
    return result;
}

vector<string> parsePolynomial(const string& poly) {
    vector<string> terms;
    string term;
    for (char ch : poly) {
        if (ch == '+' || ch == '-') {
            if (!term.empty()) {
                terms.push_back(term);
                term.clear();
            }
            term += ch;  // Include the sign for the next term
        } else {
            term += ch;
        }
    }
    if (!term.empty()) {
        terms.push_back(term);
    }
    return terms;
}

string normalizeTerm(const string& term) {
    // Just remove the sign from the term if it exists
    if (term[0] == '-' || term[0] == '+') {
        return term.substr(1);
    }
    return term;
}

string addOrSubtractPolynomials(const string& poly1, const string& poly2, bool isAddition) {
    map<string, int> termCounts; // 用于计数的映射
    map<string, string> originalTerms; // 用于保持原始项顺序的映射
    stringstream result;

    auto updateTermCounts = [&](const string& poly, int sign) {
        for (auto& term : parsePolynomial(poly)) {
            string normalizedTerm = normalizeTerm(term);
            vector<string> splitElements = splitString(normalizedTerm);
            sort(splitElements.begin(), splitElements.end());
            string combinedElements;
            for (const auto& elem : splitElements) {
                combinedElements += elem;
            }

            int termSign = (term[0] == '-') ? -1 : 1;
            termCounts[combinedElements] += termSign * sign;
            originalTerms[combinedElements] = normalizedTerm; // 保留原始项
        }
    };

    updateTermCounts(poly1, 1);
    updateTermCounts(poly2, isAddition ? 1 : -1);

    for (const auto& pair : termCounts) {
        if (pair.second != 0) {
            string originalTerm = originalTerms[pair.first];
            if (result.tellp() > 0 && pair.second > 0) {
                result << '+';
            }
            if (pair.second < 0) {
                result << '-';
            }
            if (abs(pair.second) != 1) {
                result << abs(pair.second);
            }
            result << originalTerm;
        }
    }

    return result.str();
}

string multiplyPolynomials(const string& poly1, const string& poly2) {
    vector<string> terms1 = parsePolynomial(poly1);
    vector<string> terms2 = parsePolynomial(poly2);
    stringstream result;
    if (poly1 == "0" || poly2 == "0") {
        return "";
    }
    if (poly1 == "1" ) {
        return poly2;
    }
    if (poly2 == "1" ) {
        return poly1;
    }
    for (auto& term1 : terms1) {
        for (auto& term2 : terms2) {
            if (result.tellp() > 0) {
                result << '+';
            }
            // 删除项的开始的 '+' 或 '-' 符号
            string cleanTerm1 = (term1[0] == '+' || term1[0] == '-') ? term1.substr(1) : term1;
            string cleanTerm2 = (term2[0] == '+' || term2[0] == '-') ? term2.substr(1) : term2;
            // 相乘
            result << (term1[0] == '-' ? "-" : "") << cleanTerm1 << (term2[0] == '-' ? "-" : "") << cleanTerm2;
        }
    }

    return result.str();
}

pair<set<string>, set<string>> ALGEBRAIC_DIVISION(const vector<string>& A, const vector<string>& B) {
    vector<vector<string>> allD;

    for (const auto& b : B) {
        vector<string> D;
        for (const auto& a : A) {
            if (contains(a, b)) {
                string term = removeChars(a, b);
                if (term.empty()) term = "1";
                D.push_back(term);
                //R.erase(a);
            }
        }
        if (D.empty()) {
            return make_pair(set<string>(), set<string>(A.begin(), A.end()));
        }
        if (!D.empty()) {
            allD.push_back(D);
        }

    }

    if (allD.empty()) {
        return make_pair(set<string>(), set<string>(A.begin(), A.end()));
    }

    vector<string> commonElements = findCommonElements(allD);
    set<string> Q(commonElements.begin(), commonElements.end());
    vector<string> Qvec(Q.begin(),Q.end());
    string Qstring = joinPolynomial(Qvec);
    string divisorString = joinPolynomial(B);
    string Fstring = joinPolynomial(A);
    string Rstring = addOrSubtractPolynomials(Fstring, multiplyPolynomials(Qstring,divisorString), false) ;
    vector <string> Rvec = splitPolynomial(Rstring);
    set<string> R (Rvec.begin(),Rvec.end());
    if (Q.empty()) Q.insert("0");
    if (R.empty()) R.insert("0");
    return make_pair(Q, R);
}

bool isCubeFree(const string& poly) {
    auto terms = splitPolynomial(poly);
    set<string> uniqueElements;
    vector<string> temp;
    for (const auto& term : terms) {
        temp = splitString(term);
        for( auto z : temp){

            uniqueElements.insert(z);
        }
    }
    for (const auto& element : uniqueElements) {
        auto [Q, R] = ALGEBRAIC_DIVISION(terms, vector<string>{element});
        if ((R.size() == 1 && *R.begin() == "0")) {
            return false; // 如果余数为零，多项式不是 cube-free
        }
    }
    return true; // 如果没有余数为零的情况，多项式是 cube-free
}
vector<string> findIntersection(const vector<string>& input) {
    if (input.empty()) {
        return {}; // 如果输入为空，则返回空向量
    }

    // 将第一个字符串的分割结果初始化为初始交集
    vector<string> intersection = splitString(input[0]);

    // 对于每个字符串，找到与当前交集的交集
    for (size_t i = 1; i < input.size(); ++i) {
        const vector<string>& currentElements = splitString(input[i]);
        vector<string> newIntersection;

        for (const string& element : currentElements) {
            if (find(intersection.begin(), intersection.end(), element) != intersection.end()) {
                newIntersection.push_back(element);
            }
        }

        intersection = newIntersection;
    }

    return intersection;
}

vector<string> removeIntersection(const vector<string>& input, const vector<string>& intersection) {
    vector<string> result;

    for (const string& str : input) {
        vector<string> elements = splitString(str);

        for (const string& inter : intersection) {
            elements.erase(remove(elements.begin(), elements.end(), inter), elements.end());
        }

        string filteredStr;
        for (const string& element : elements) {
            filteredStr += element;
        }

        result.push_back(filteredStr);
    }

    return result;
}


// 主要的 KERNELS 函數
map<string, string> findKernels(const string& f, int startLiteralIndex, const string& currentCokernel = "") {
    map<string, string> kernelsAndCokernels;
    auto terms = splitPolynomial(f);
    string one = "1";

    if (isCubeFree(f)) {
        kernelsAndCokernels[f] = one;
    }

    set<string> literals;
   // map<string, int> literalIndices;
    //int index = 0;
    if (literals.empty()){
        for (const auto& term : terms) {
            auto splitTerm = splitString(term);
            for (const auto& lit : splitTerm) {
                literals.insert(lit);
            }
        }
    }
    vector<string> orderedLiterals(literals.begin(), literals.end());
    //for(auto a : orderedLiterals)cout<<"ordredlist: "<<a;
    for (int i = startLiteralIndex; i < orderedLiterals.size(); ++i) {
        // for(auto a : orderedLiterals)cout<<"ordredlist: "<<a;
        
        string literal = orderedLiterals[i];
        string intersecstring ;
        auto [Q, R] = ALGEBRAIC_DIVISION(terms, vector<string>{literal});
        
        if (Q.size() > 1) {
            vector<string> Qvec(Q.begin(),Q.end());
           // for(auto a : Qvec)cout<<"Qvec: "<<a;
            
            vector<string> QvecIntersection = findIntersection(Qvec);
            for(auto c : QvecIntersection){
                intersecstring +=c;
            }
            vector<string> QvecIntersecRemoved = removeIntersection(Qvec, QvecIntersection);
            string joinedQ = joinPolynomial(QvecIntersecRemoved);
            //cout<<"joinedQ: "<<joinedQ<<endl;
            if (!joinedQ.empty() && isCubeFree(joinedQ)) {
                string newCokernel =  literal + intersecstring ;
                kernelsAndCokernels[joinedQ] = newCokernel;
                
                auto subKernelsAndCokernels = findKernels(joinedQ, i + 1, newCokernel);
                kernelsAndCokernels.insert(subKernelsAndCokernels.begin(), subKernelsAndCokernels.end());
            }
        }
        
    }

    return kernelsAndCokernels;
}

vector<string> processBlifFile(ifstream& file) {
    vector<string> outputs;
    string line;
    while (getline(file, line)) {
        // Check and handle line continuation
        while (line.back() == '\\') {
            line.pop_back(); // Remove the '\' character
            string nextLine;
            if (getline(file, nextLine)) {
                line += nextLine; // Append the next line
            }
        }
        if (line.find(".names") != string::npos) {
            vector<string> inputs;
            string output;
            // Extract inputs and output variable
            stringstream ss(line);
            string temp;
            while (ss >> temp) {
                if (temp != ".names") {
                    inputs.push_back(temp);
                }
            }
            string outputVar = inputs.back(); // Last element is output
            inputs.pop_back(); // Remove output variable from inputs
            output += outputVar + "=";
            // Process the logic cubes
            while (getline(file, line)) {
                if (line.empty() || line.find(".names") != string::npos) {
                    file.seekg(-(long)(line.length() + 1), ios::cur); // Correctly rewind to process the next .names
                    break;
                }
                for (size_t i = 0; i < line.size(); ++i) {
                    if (i >= inputs.size()) {
                        break; // Prevents reading beyond the input list
                    }
                    if (line[i] == '1') {
                        output += inputs[i];
                    } else if (line[i] == '0') {
                        output += inputs[i] + "'";
                    }
                    // Skip '-' cases
                }
                // Check if it is the last line of the .names block or end of the file
                if (file.peek() == EOF || file.peek() == '\n') {
                    break; // Do not add '+' if it's the last line of the block or end of the file
                } else {
                    output += "+";
                }
            }
            // Remove last '+' if exists
            if (!output.empty() && output.back() == '+') {
                output.pop_back();
            }
            outputs.push_back(output);
        }
    }
    return outputs;
}

vector<string> splitLogicExpression(const string& str) {
    vector<string> elements;
    string current;

    for (size_t i = 0; i < str.length(); ++i) {
        char ch = str[i];

        if (ch == '+' || ch == '=') {
            if (!current.empty()) {
                elements.push_back(current);
                current.clear();
            }
            elements.emplace_back(1, ch); // Add '+' or '=' as separate elements
        } else {
            current += ch;

            // Check if the next character requires splitting
            if (i + 1 < str.length()) {
                char nextChar = str[i + 1];
                if (isalpha(nextChar) || nextChar == '+' || nextChar == '=') {
                    elements.push_back(current);
                    current.clear();
                }
                if (nextChar == '\'') {
                    current += '\'';
                    elements.push_back(current);
                    current.clear();
                    i++;
                }
            }
        }
    }

    // Add the last element if it exists
    if (!current.empty()) {
        elements.push_back(current);
    }

    return elements;
}

string convertExpressionToBlif(const vector<string>& terms) {
    string output = terms.front(); // The first term is the output variable
    set<string> allInputs; // To store all unique input variables

    // Collect all unique variables (ignoring negations and skipping "+" and "=")
    for (size_t i = 2; i < terms.size(); ++i) {
        if (terms[i] != "+" && terms[i] != "=") {
            string term = terms[i];
            if (term.back() == '\'') {
                term.pop_back();
            }
            allInputs.insert(term);
        }
    }

    // Start the .names entry
    string blif = ".names ";
    for (const auto& input : allInputs) {
        blif += input + " ";
    }
    blif += output + "\n";

    // Process each product term
    string currentLine(allInputs.size(), '-');
    for (size_t i = 2; i < terms.size(); ++i) {
        if (terms[i] == "+") {
            // End of a product term, add the line to blif and reset it
            blif += currentLine + " 1\n";
            currentLine = string(allInputs.size(), '-');
        } else if (terms[i] != "=") {
            // Process each term
            bool negated = terms[i].back() == '\'';
            string var = negated ? terms[i].substr(0, terms[i].size() - 1) : terms[i];
            size_t index = distance(allInputs.begin(), allInputs.find(var));
            currentLine[index] = negated ? '0' : '1';
        }
    }
    // Add the last line
    blif += currentLine + " 1\n";

    return blif;
}

void writeintoFile(ifstream& readFile, ofstream& outputBlif, const vector<std::string>& reducedPoly) {
    string line;
    vector<std::string> inputLines;
    vector<std::string> outputLines;
    bool copyLines = false;

    while (getline(readFile, line)) {
        while (line.back() == '\\') {
            line.pop_back(); // Remove the '\' character
            string nextLine;
            if (getline(readFile, nextLine)) {
                line += nextLine; // Append the next line
            }
        }
        if (line.find(".model") != string::npos) {
            outputBlif << ".model top" << std::endl;
        } else if (line.find(".inputs") != string::npos) {
            inputLines.push_back(line);
        } else if (line.find(".outputs") != string::npos) {
            outputLines.push_back(line);
        } else if (line.find(".end") != string::npos) {
            // Write the input and output lines collected earlier
            for (const string& inputLine : inputLines) {
                outputBlif << inputLine <<endl;
            }
            for (const string& outputLine : outputLines) {
                outputBlif << outputLine << endl;
            }

            // Process and write the reduced polynomials
            for (const string& poly : reducedPoly) {
                auto terms = splitLogicExpression(poly);
                outputBlif << convertExpressionToBlif(terms) << std::endl;
            }

            // Write the .end line
            outputBlif << line << std::endl;
            break;
        }
    }
}

int countOnesAndZerosInBlif(ifstream& file) {
    string line;
    bool inNamesBlock = false;
    int count = 0;
    int total = 0;
    while (getline(file, line)) {
        while (line.back() == '\\') {
            line.pop_back(); // Remove the '\' character
            string nextLine;
            if (getline(file, nextLine)) {
                line += nextLine; // Append the next line
            }
        }
        if (line.find(".names") != std::string::npos) {
            if (inNamesBlock) {
                // Print the count for the previous .names block before starting the next
              //  std::cout << "Count in .names block: " << count << std::endl;
                count = 0;
            }
            inNamesBlock = true;
        } else if (inNamesBlock && !line.empty()) {
            // Count '1' and '0' in the line, excluding the last two characters (" 1")
            for (size_t i = 0; i < line.length() - 2; ++i) {
                if (line[i] == '1' || line[i] == '0') {
                    ++count;
                    ++total;
                }
            }
        }
    }

    // Print the count for the last .names block
    if (inNamesBlock) {
        //cout << "Count in .names block: " << count << std::endl;
    }
    file.close();
    return total;
}
int main(int argc, const char * argv[]) {
    if (argc >= 2) {
        string arg1 = argv[1];
        if (arg1 != "out.blif") {
      
        std::string www = "ace+bce+de+g";
        auto kernelsAndCokernels = findKernels(www, 0);
        
        // 打印 kernel 及其对应的 cokernel
        for (const auto& [kernel, cokernel] : kernelsAndCokernels) {
            std::cout << "Kernel: " << kernel << ", Cokernel: " << cokernel << std::endl;
        } /*
        vector<vector<string>> an = {{"a","b"}
            
        };
        std::vector<std::string> commonElements = findCommonElements(an);
        for(auto z : commonElements) cout<<"commonelemnt: "<<z;
        cout<<endl;
        std::vector<std::string> A = {"a"} ;
        std::vector<std::string> B = {"a"};
        // 调用 ALGEBRAIC_DIVISION 函数
        std::pair<std::set<std::string>, std::set<std::string>> result = ALGEBRAIC_DIVISION(A, B);
        
        // 输出结果
        std::cout << "Q: ";
        for (const auto& element : result.first) {
            std::cout << element << " ";
        }
        std::cout << std::endl;
        
        std::cout << "R: ";
        for (const auto& element : result.second) {
            std::cout << element << " ";
        }
        std::cout << std::endl;
        
        std::vector<std::string> dd = {"abdnaa0'","bewwa0'"} ;
        set <string> asd = findCommonLiterals(dd);
        for(auto z :asd)cout<<z<<endl;
        
        string s = "ajijo'i4'ajoajijo'i4'ajoajijo'i4'ajoajijo'";
        vector<string> da = splitString(s);
        for(auto z :da){
            cout<<z<<",";
            
        }
        cout<<endl;
        */
        ifstream readFile;
        
        readFile.open(argv[1]);
        
        vector<string> polynomials = processBlifFile(readFile);
        /*
        for (const auto& poly : polynomials) {
            cout << poly << endl;
        }
        cout <<"contains"<<endl;
        std::cout << std::boolalpha;
        std::cout << contains("ace", "ce") << std::endl;     // true
        std::cout << contains("a'", "a") << std::endl;    // false
        std::cout << contains("a0bc", "b") << std::endl;    // false
        std::cout << contains("a0'", "a") << std::endl;   // false
        std::cout << contains("a0'", "a0") << std::endl;  // false
        std::cout << contains("ba0'", "a") << std::endl;  // false
        std::cout << contains("b", "a") << std::endl;     // false
        std::cout << contains("a'", "a0") << std::endl;   // false
        std::cout << contains("a0", "a'") << std::endl;   // false
        std::cout << contains("a0b", "a0'") << std::endl; // false
        std::cout << contains("a0b'", "a0'") << std::endl; // false
        cout <<"removechar"<<endl;
        string g = removeChars("ab0'rt5", "ar");
        cout<<g<<endl;
        cout <<"iscube free"<<endl;
        std::cout << isCubeFree("a") << std::endl;     // true
        std::cout << isCubeFree("abc") << std::endl;                        // false
        std::cout << isCubeFree("ae+be") << std::endl;                      // false
        std::cout << isCubeFree("1") << std::endl;                      // false
        std::cout << isCubeFree("a0+a0'") << std::endl;                     // true
        std::cout << isCubeFree("aa0+aba0'") << std::endl;                    // false
        */
        set<string> allKernels;
        for (const auto& poly : polynomials) {
            //cout << poly << endl;
            auto [left , right] = splitEquation(poly);
            auto result = findKernels(right, 0);
            cout<<"K("<<left<<"): "<<endl;
            for(auto z : result){
                cout<<"Kernel: "<<z.first<<" Cokernel: "<<z.second<<endl;
                allKernels.insert(z.first);
            }
            
        }
        //cout << "All kenels:" << endl;
        vector<vector<string>> allkernelsVec ;
        for (const auto& element : allKernels) {
            //cout << element << endl;
            vector<string> div = splitPolynomial(element);
            allkernelsVec.push_back(div);
        }
        map<string, int> divisorCounts;
        vector<string> allDivisors; // 存储所有的 divisor
        
        // 第一次遍历：计算每个 divisor 的出现次数
        for (const auto& poly : polynomials) {
            auto [left, right] = splitEquation(poly);
            std::vector<string> polyrightvec = splitPolynomial(right);
            
            for (const auto& ker : allkernelsVec) {
                pair<set<string>, set<string>> divAns = ALGEBRAIC_DIVISION(polyrightvec, ker);
                if (!divAns.first.empty() && divAns.first.find("0") == divAns.first.end()) {
                    std::string divisor = joinPolynomial(ker);
                    divisorCounts[divisor]++;
                    allDivisors.push_back(divisor); // 添加 divisor 到列表中
                }
            }
        }
        
        // 找出出现次数最多的 divisor
        std::string mostCommonDivisor;
        int maxCount = 0;
        for (const auto& pair : divisorCounts) {
            if (pair.second > maxCount) {
                maxCount = pair.second;
                mostCommonDivisor = pair.first;
            }
        }
        vector<string> reducedPolyvec ;
        string reducedPolystring ;
        string temp;
        cout<<"Z="<<mostCommonDivisor<<endl;
        temp = temp + "Z=" +mostCommonDivisor;
        reducedPolyvec.push_back(temp);
        // 第二次遍历：检查多项式是否能被最常见的 divisor 成功除
        for (const auto& poly : polynomials) {
            auto [left, right] = splitEquation(poly);
            vector<string> polyrightvec = splitPolynomial(right);
            
            // 只使用最常见的 divisor
            vector<string> mostCommonDivisorVec = splitPolynomial(mostCommonDivisor);
            pair<set<std::string>, set<string>> divAns = ALGEBRAIC_DIVISION(polyrightvec, mostCommonDivisorVec);
            if (!divAns.first.empty() && divAns.first.find("0") == divAns.first.end()) {
                vector<std::string> divAnsQVec(divAns.first.begin(), divAns.first.end());
                vector<std::string> divAnsRVec(divAns.second.begin(), divAns.second.end());
                string Q = joinPolynomial(divAnsQVec);
                string R = joinPolynomial(divAnsRVec);
                
                if (divAns.second.find("0") == divAns.second.end()) {
                    right = multiplyPolynomials("Z", Q) + "+" + R;
                } else {
                    right = multiplyPolynomials("Z", Q) ;
                }
                  //cout<<left<<": "<<right<<endl;
                reducedPolystring = left +"="+ right;
                reducedPolyvec.push_back(reducedPolystring);
            }
            else{
                 //cout<<left<<": "<<right<<endl;
                reducedPolystring = left +"="+ right;
                reducedPolyvec.push_back(reducedPolystring);
            }
        }
        for (auto c : reducedPolyvec){
           // cout << c <<endl;
        }
        // 替换为你的输出文件名
        ifstream sss;
        sss.open(argv[1]);
        ofstream outputBlif("out.blif");
        writeintoFile(sss, outputBlif,reducedPolyvec);
        ifstream ddd;
        ddd.open(argv[1]);
        int total = countOnesAndZerosInBlif(ddd);
        cout<<"literal count: "<<total<<endl;

    }
}

    if (argc >= 2) {
        string arg1 = argv[1];
        if (arg1 == "out.blif") {
            ifstream ddd;
            ddd.open(argv[1]);
          int optimizedcount = countOnesAndZerosInBlif(ddd);
            cout<<"Optimized literal count: "<<optimizedcount<<endl;

        }
        
    }

}

