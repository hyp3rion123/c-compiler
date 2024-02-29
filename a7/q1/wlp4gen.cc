#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>

//Helper function
inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

std::map<std::string, int> buildPrologue(std::map<std::string, std::pair<std::string, int>> symbolTable) {
    std::map<std::string, int> stackPositions;
    int currentPointer = 0;
    std::cout << "; begin prologue" << std::endl;
    std::cout << "lis $4" << std::endl;
    std::cout << ".word 4" << std::endl;
    std::cout << "sub $29, $30, $4 ; setup frame pointer" << std::endl;
    
    //Store all variables
    for(auto it = symbolTable.begin(); it != symbolTable.end(); it++) {
        std::cout << "sw $" << it->second.second << ", " << currentPointer << "($29) ; push variable " << it->first << std::endl;
        std::cout << "sub $30, $30, $4 ; update stack pointer" << std::endl;
        std::pair<std::string, int> posPair = make_pair(it->first, currentPointer);
        stackPositions.insert(posPair);
        currentPointer-=4;
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
                std::pair<std::string, int> second = std::make_pair(extractedType, 1);
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
                //Reached return - can build prologue now
                std::map<std::string, int> stackPositions = buildPrologue(symbolTable);

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

    }
    std::cout << returnBody << std::endl;
    buildEpilogue(symbolTable);
}

int main() {
    parseTree();
    return 0;
}