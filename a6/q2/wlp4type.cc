#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>

std::string prevLine = "";
int linesAgo = 0; //to keep track of staleness of prevLine
std::map<std::string, std::string> symbolTable;

class Tree {
    public: 
        std::string LHS; // e.g LHS of expr expr PLUS term is expr
        std::string rule; // e.g expr expr PLUS term
        std::vector<Tree *> children;
        std::string type = "";
};

void buildTree(Tree *t) {
    std::string tempString;
    //read next line and add as rule
    std::string nextLine;
    std::getline(std::cin, nextLine);
    if(!nextLine.compare("STAR *")) {
        prevLine = "STAR *";
        linesAgo = 0;
    } else if(!nextLine.compare("INT int")) {
        prevLine = "INT int";
        linesAgo = 0;
    } else if(!nextLine.compare("RETURN return")) {//error check return value of wain
        prevLine = "RETURN return";
        linesAgo = 0;
    } else if ((linesAgo > 1 && prevLine.compare("RETURN return") != 0) || !nextLine.compare("SEMI ;")){
        prevLine = "";
        linesAgo = 0;
    } else {
        linesAgo++;
    }
    // std::cout << "NEW LINE IS " << nextLine << std::endl;
    t->rule = nextLine;
    //append to vector of children
    std::stringstream childrenStream(nextLine);
    childrenStream >> t->LHS;
    while (childrenStream >> tempString && tempString != "") {
        Tree *child = new Tree();
        if (std::all_of(tempString.begin(), tempString.end(), [](unsigned char c){ return std::isupper(c); }) || !tempString.compare(".EMPTY")) {      
            // std::cout << "adding terminal child " << tempString << " to tree of " << t->rule << std::endl;        
            //child is terminal so don't recurse
            child->LHS = tempString;
            if(!tempString.compare(".EMPTY")) {
                child->rule = tempString;
            } else {
                std::getline(std::cin, nextLine);
                if(!nextLine.compare("STAR *")) {
                    prevLine = "STAR *";
                    linesAgo = 0;
                } else if(!nextLine.compare("INT int")) {
                    prevLine = "INT int";
                    linesAgo = 0;
                } else if(!nextLine.compare("RETURN return")) {//error check return value of wain
                    prevLine = "RETURN return";
                    linesAgo = 0;
                } else if ((linesAgo > 1 && prevLine.compare("RETURN return") != 0) || !nextLine.compare("SEMI ;")){
                    prevLine = "";
                    linesAgo = 0;
                } else {
                    linesAgo++;
                }
                // std::cout << "NEW LINE IS " << nextLine << std::endl;
                // std::cout << "adding rule " << nextLine << " to terimanl child " << child->LHS << std::endl;  
                if(!child->LHS.compare("NUM")) {
                    child->type = "int";
                }else if(!child->LHS.compare("NULL")) {
                    child->type = "int*";
                } else if(!child->LHS.compare("ID")) {
                    std::string childValue;
                    std::stringstream tempStream(nextLine);
                    tempStream >> childValue; //flush first part - ID
                    tempStream >> childValue;      
                    // std::cout << "in ID logic, prev line is: " << prevLine << std::endl;
                    if(!prevLine.compare("STAR *")) {
                        //checking if it is duplicating
                        auto it = symbolTable.find(childValue);
                        if (it != symbolTable.end()) {
                            std::cerr << "ERROR - DUPLICATE SYMBOL" << std::endl;
                            // std::cout << "found duplicate" << std::endl;
                        }
                        //is a new variable - add to symboltable
                        child->type = "int*";
                        //error check if this is the second argument of wain - if type is int* throw error
                        if(symbolTable.size() == 1) {
                            std::cerr << "ERROR - SECOND ARG OF WAIN IS INT*" << std::endl;
                        }
                        std::pair<std::string, std::string> newPair = make_pair(childValue, "int*");
                        symbolTable.insert(newPair);
                        // std::cout << "inserting pair " << childValue << ", int*" << std::endl;
                    } else if(!prevLine.compare("INT int")) {
                        //checking if it is duplicating
                        auto it = symbolTable.find(childValue);
                        if (it != symbolTable.end()) {
                            std::cerr << "ERROR - DUPLICATE SYMBOL" << std::endl;
                            // std::cout << "found duplicate" << std::endl;
                        }
                        //is a new variable - add to symboltable
                        child->type = "int";
                        std::pair<std::string, std::string> newPair = make_pair(childValue, "int");
                        symbolTable.insert(newPair);
                        // std::cout << "inserting pair " << childValue << ", int" << std::endl;
                    } else {//it is not a new declr
                        //check if it exists - if it doesn't throw error that symbol doesn't exist
                        auto it = symbolTable.find(childValue);
                        if (it == symbolTable.end()) {
                            std::cerr << "ERROR - SYMBOL DOESN'T EXIST" << std::endl;
                            // std::cout << "found duplicate" << std::endl;
                        } else {
                            child->type = it->second;
                        }
                    }
                }
                child->rule = nextLine;
            }      
        } else {
            // std::cout << "adding non-terminal child " << tempString << " to tree of " << t->rule << std::endl;
            buildTree(child);  
        }
        t->children.push_back(child);
    }
    //check if current node is a terminal node representing a num, null, or id token - assign the type if so
    
    //check if children are of any type
    if(!t->children[0]->type.compare("int") || !t->children[0]->type.compare("int*")) {
        t->type = t->children[0]->type;
    }else if (t->children.size() > 1 && (!t->LHS.compare("expr") || !t->LHS.compare("term") || !t->LHS.compare("factor") || !t->LHS.compare("lvalue")) && 
    (!t->children[1]->type.compare("int") || !t->children[1]->type.compare("int*"))) {
        t->type = t->children[1]->type;
    }
    if (!prevLine.compare("RETURN return")) {
        // std::cout << "prevLine: " << prevLine << std::endl;
        if (t->type != "int") {
            std::cerr << "ERROR - RETURN TYPE IS NOT INT" << std::endl;
        }
    }
}

void verifyChildTypes(Tree *t) {
    // std::cout << "Children of " << t->LHS << ": ";
    // for(Tree *child: t->children) {
    //     std::cout << child->LHS << ", ";
    // }
    // std::cout << std::endl;
    if(!t->LHS.compare("dcls") && t->children.size() > 1) {
        //get type of the dcl
        std::string dclChildType = t->children[1]->children[1]->type;
        // std::cout << "dcl child type is " << dclChildType << std::endl;
        //get type of rhs of expression
        std::string rhsType = t->children[3]->LHS;
        // std::cout << "dcl rhs type is " << rhsType << std::endl;
        //if they are not equivalent, throw error
        if((!dclChildType.compare("int") && !rhsType.compare("NULL")) || (!dclChildType.compare("int*") && !rhsType.compare("NUM"))) {
            std::cerr << "ERROR - MISMATCHED VARIABLE ASSIGNMENT" << std::endl;
            return;
        }
    }
    for(Tree *child: t->children) {
       verifyChildTypes(child);
    }
}

void deleteTree(Tree *t) {
    for(Tree *child: t->children) {
        // std::cout << "printing child " << child->LHS << " of " << t->LHS << std::endl;
        deleteTree(child);
    }
    delete t;
}

void printTree(Tree *t) {
    if(t->rule.compare(".EMPTY") != 0) {
        std::cout << t->rule;
        if(t->type.compare("") != 0) {
            std::cout << " : " << t->type;
        }   
        std::cout << std::endl;
        for(Tree *child: t->children) {
            // std::cout << "printing child " << child->LHS << " of " << t->LHS << std::endl;
            printTree(child);
        }
    }
}

int main() {
    Tree *tree = new Tree();
    buildTree(tree);
    verifyChildTypes(tree);
    printTree(tree);
    deleteTree(tree);
    return 0;
}