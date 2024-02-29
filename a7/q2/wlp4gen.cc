#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>

std::map<std::string, int> stackPositions;

//Helper function
inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void code(std::string expr) {
    //right now codes are just of the form code(a) or code(b)
    //Find expr in stackPositions
    auto it = stackPositions.find(expr);
    std::cout << "lw $3, " << it->second <<"($29) ; $3 <- " << it->first << std::endl;
}

void push(int reg) {
    std::cout << "sw $" + std::to_string(reg) + ", -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4 ; update stack pointer" << std::endl;
}

void pop(int reg) {    
    std::cout << "add $30, $30, $4 ; update stack pointer" << std::endl;
    std::cout << "lw $" + std::to_string(reg) + ", -4($30)" << std::endl;
}

std::map<std::string, int> buildPrologue(std::map<std::string, std::pair<std::string, int>> symbolTable) {
    std::map<std::string, int> stackPositions;
    int currentPointer = -8; //always pushing a and b
    int varsSkipped = 0; //skip a and b push since we push manually
    std::cout << "; begin prologue" << std::endl;
    std::cout << "lis $4" << std::endl;
    std::cout << ".word 4" << std::endl;
    std::cout << "sub $29, $30, $4 ; setup frame pointer" << std::endl;
    push(1);
    push(2);
    
    //Store all variables
    for(auto it = symbolTable.begin(); it != symbolTable.end(); it++) {
        if (varsSkipped == 0) {
            //insert a in stack
            std::pair<std::string, int> posPair = make_pair(it->first, 0);
            stackPositions.insert(posPair);
        } else if (varsSkipped == 1) {
            //insert b in stack
            std::pair<std::string, int> posPair = make_pair(it->first, -4);
            stackPositions.insert(posPair);
        }
        if(varsSkipped > 1) {
            std::cout << "lis $5\n.word " << it->second.second << std::endl;
            push(5);
            std::pair<std::string, int> posPair = make_pair(it->first, currentPointer);
            stackPositions.insert(posPair);
            currentPointer-=4;
        }
        varsSkipped++;
    }

    std::cout << "; end prologue\n" << std::endl;
    return stackPositions;
}

void buildEpilogue(std::map<std::string, std::pair<std::string, int>> symbolTable) {
    std::cout << "; begin epilogue" << std::endl;
    //Update stack pointer
    for(auto it = symbolTable.begin(); it != symbolTable.end(); it++) {
        std::cout << "add $30, $30, $4" << std::endl;
    }

    std::cout << "jr $31" << std::endl;
}

void parseTree() {
    //Init
    std::map<std::string, std::pair<std::string, int>> symbolTable;
    std::string nextLine;

    //Helper variables
    bool seenWain = false;
    bool seenReturn = false;
    bool seenDecls = false;
    bool seenStatements = false;
    std::string returnBody; //used so that we can print this in between prologue and epilogue

    //Parsing input
    while(std::getline(std::cin, nextLine)) {
        if(!seenWain) {
            if(!nextLine.compare("WAIN wain")) {
                seenWain = true;

                //Add a to the symboltable - flush until next 5th line
                for (int i = 0; i < 5; i++) {
                    std::getline(std::cin, nextLine);
                }
                //Nextline is of the form ID __ : int
                std::string extractedID;
                std::string extractedType;
                std::stringstream paramStream(nextLine);
                paramStream >> extractedID; // gets ID
                paramStream >> extractedID; // gets actual id
                paramStream >> extractedType; // gets ;
                paramStream >> extractedType; // gets actual type
                //Add to map
                std::pair<std::string, int> second = std::make_pair(extractedType, -1);
                std::pair<std::string, std::pair<std::string, int>> toInsert = std::make_pair(extractedID, second);
                symbolTable.insert(toInsert);

                //Add b to the symboltable - flush until next 5th line
                for (int i = 0; i < 5; i++) {
                    std::getline(std::cin, nextLine);
                }
                //Nextline is of the form ID __ : int
                std::stringstream paramStream2(nextLine);
                paramStream2 >> extractedID; // gets ID
                paramStream2 >> extractedID; // gets actual id
                paramStream2 >> extractedType; // gets ;
                paramStream2 >> extractedType; // gets actual type
                //Add to map
                second = std::make_pair(extractedType, 2);
                toInsert = std::make_pair(extractedID, second);
                symbolTable.insert(toInsert);
            }
        }

        //Get return value
        if(!seenReturn) {
            std::string retLine = "RETURN return";
            auto res = std::mismatch(retLine.begin(), retLine.end(), nextLine.begin());
            if (res.first == retLine.end()) {
                //If returning - flush next 2 lines, get 3rd
                for (int i = 0; i < 3; i++) {
                    std::getline(std::cin, nextLine);
                }
                std::string factorType;
                std::stringstream factorStream(nextLine);
                factorStream >> factorType; // gets factor
                factorStream >> factorType; // gets actual type
                if(!factorType.compare("LPAREN")) {
                    while(!factorType.compare("LPAREN")) {
                        for (int i = 0; i < 4; i++) {
                            std::getline(std::cin, nextLine);
                        }
                        std::stringstream innerFactorStream(nextLine);
                        innerFactorStream >> factorType; // gets factor
                        innerFactorStream >> factorType; // gets actual type
                    }
                }
                if(!factorType.compare("NUM")) {
                    std::getline(std::cin, nextLine);
                    //Extract number
                    std::string extractedNum;
                    std::stringstream paramStream(nextLine);
                    paramStream >> extractedNum; // gets NUM
                    paramStream >> extractedNum; // gets actual value
                    returnBody += "lis $4\n.word " + extractedNum + "\nadd $3, $4, $0\nlis $4\n.word 4\n";
                    seenReturn = true;
                } else if(!factorType.compare("ID")) {
                    std::getline(std::cin, nextLine);
                    //Extract ID
                    std::string extractedID;
                    std::stringstream paramStream(nextLine);
                    paramStream >> extractedID; // gets ID
                    paramStream >> extractedID; // gets actual id
                    auto it = stackPositions.find(extractedID);
                    returnBody += "lw $3, " + std::to_string(it->second) + "($29) ; variable " + it->first + " has offset " + std::to_string(it->second) + " w.r.t. $29\n";
                    seenReturn = true;
                }
            }
        }

        //Get decls
        if(!seenDecls) {
            std::string retLine = "dcls";
            auto res = std::mismatch(retLine.begin(), retLine.end(), nextLine.begin());
            if (res.first == retLine.end()) {
                seenDecls = true;
                // std::cout << "seeing dcls";
                //Count how many decls there are
                int declsCount = 0;
                //Check if dcls is empty - if it is we can skip
                std::string extractedDcls;
                std::stringstream paramStream(nextLine);
                paramStream >> extractedDcls; // gets dcls
                paramStream >> extractedDcls; // gets first arg
                while(extractedDcls.compare(".EMPTY") != 0) {
                    declsCount++;
                    std::getline(std::cin, nextLine);
                    std::stringstream paramStream2(nextLine);
                    paramStream2 >> extractedDcls; // gets dcls
                    paramStream2 >> extractedDcls; // gets first arg
                }
                // std::cout << "found " << declsCount << " dcls " << std::endl;
                //Iterate over each decl
                // std::cout << "nextline current is " << nextLine << std::endl;
                for(int i = 0; i < declsCount; i++) {
                    //Flush first 3 lines - keep 4th
                    for(int j = 0; j < 4; j++) {
                        std::getline(std::cin, nextLine);
                    }
                    // std::cout << "current line: " << nextLine << std::endl;
                    //Extract ID and type
                    std::string newType;
                    std::string newID;
                    std::stringstream paramStream2(nextLine);
                    paramStream2 >> newID; // flush ID
                    paramStream2 >> newID; // gets ID
                    paramStream2 >> newType; // flush :
                    paramStream2 >> newType; // gets type

                    //Flush first 1 line - keep 2nd
                    for(int j = 0; j < 2; j++) {
                        std::getline(std::cin, nextLine);
                    }
                    // std::cout << "current line: " << nextLine << std::endl;
                    //Extract value
                    std::string newValue;
                    std::stringstream paramStream3(nextLine);
                    paramStream3 >> newValue; // flush NUM
                    paramStream3 >> newValue; // gets actual number

                    //Insert in map
                    // std::cout << "insertin decl of variable " << newID << " of type " << newType << " with value " << newValue << std::endl;
                    std::pair<std::string, int> second = std::make_pair(newType, stoi(newValue));
                    std::pair<std::string, std::pair<std::string, int>> toInsert = std::make_pair(newID, second);
                    symbolTable.insert(toInsert);

                    std::getline(std::cin, nextLine); //flush semi
                }
                //Reached end of decls - can build prologue now
                stackPositions = buildPrologue(symbolTable);
            }            
        }

        //Get Statements
        if(!seenStatements) {
            std::string retLine = "statements";
            auto res = std::mismatch(retLine.begin(), retLine.end(), nextLine.begin());
            if (res.first == retLine.end()) {
                seenStatements = true;
                // std::cout << "seeing dcls";
                //Count how many statements there are
                int statementsCount = 0;
                //Check if dcls is empty - if it is we can skip
                std::string extractedStates;
                std::stringstream paramStream(nextLine);
                paramStream >> extractedStates; // gets statements
                paramStream >> extractedStates; // gets first arg
                while(extractedStates.compare(".EMPTY") != 0) {
                    statementsCount++;
                    std::getline(std::cin, nextLine);
                    std::stringstream paramStream2(nextLine);
                    paramStream2 >> extractedStates; // gets dcls
                    paramStream2 >> extractedStates; // gets first arg
                }
                // std::cout << "found " << statementsCount << " states " << std::endl;
                //Iterate over each decl
                // std::cout << "nextline current is " << nextLine << std::endl;

                for(int i = 0; i < statementsCount; i++) {
                    //Flush first 2 lines - keep 3rd
                    for(int j = 0; j < 3; j++) {
                        std::getline(std::cin, nextLine);
                    }
                    // std::cout << "current line: " << nextLine << std::endl;
                    //Extract ID
                    std::string currID;
                    std::stringstream paramStream2(nextLine);
                    paramStream2 >> currID; // flush ID
                    paramStream2 >> currID; // gets ID

                    //Flush first 4 line - keep 5th
                    for(int j = 0; j < 5; j++) {
                        std::getline(std::cin, nextLine);
                    }
                    // std::cout << "current line: " << nextLine << std::endl;
                    //Extract type
                    std::string newType;
                    std::stringstream paramStream3(nextLine);
                    paramStream3 >> newType; // get type

                    if(!newType.compare("NUM")) {
                        //Extract num
                        // std::cout << "assigning " << currID << "to something - code for NUM" << std::endl;
                        std::string numAss;
                        std::stringstream paramStream4(nextLine);
                        paramStream4 >> numAss; // flush NUM
                        paramStream4 >> numAss; // gets number val
                        //Get value of assignment var from map
                        auto itCurr = stackPositions.find(currID);
                        //output 
                        std::cout << "lis $3\n.word " << numAss << "\nsw $3, " << itCurr->second << "($29) ; assigning " << currID << " <- " << numAss << std::endl;
                        //update in map
                        auto it = symbolTable.find(currID);
                        it->second.second = stoi(numAss);

                    } else if(!newType.compare("ID")) {
                        //Extract ID of assignment
                        // std::cout << "assigning " << currID << "to something - code for ID" << std::endl;
                        std::string othersID;
                        std::stringstream paramStream4(nextLine);
                        paramStream4 >> othersID; // flush ID
                        paramStream4 >> othersID; // gets ID
                        //Get value of assignment var from map
                        auto itCurr = stackPositions.find(currID);
                        //Adjust current var to new value
                        code(othersID);
                        std::cout << "sw $3, " << itCurr->second << "($29) ; assigning " << currID << " <- " << othersID << std::endl;
                        //find value of other
                        auto otherIT = symbolTable.find(othersID);
                        //update in map
                        auto it = symbolTable.find(currID);
                        it->second.second = otherIT->second.second;
                    }
                    std::getline(std::cin, nextLine); //flush semi
                }

            }
        }

    }
    std::cout << returnBody << std::endl;
    buildEpilogue(symbolTable);
}

int main() {
    parseTree();
    return 0;
}