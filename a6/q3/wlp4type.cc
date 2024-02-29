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
                    if(!prevLine.compare("STAR *")&& linesAgo <= 1) {
                        //checking if it is duplicating
                        auto it = symbolTable.find(childValue);
                        if (it != symbolTable.end() && linesAgo == 0) {
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
                    } else if(!prevLine.compare("INT int")&& linesAgo <= 1) {
                        //checking if it is duplicating
                        auto it = symbolTable.find(childValue);
                        if (it != symbolTable.end() && linesAgo == 0) {
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
    
    //check if children are of any type
    if(!t->children[0]->type.compare("int") || !t->children[0]->type.compare("int*")) {
        t->type = t->children[0]->type;
    }else if (t->children.size() > 1 && (!t->LHS.compare("expr") || !t->LHS.compare("term") || !t->LHS.compare("factor") || !t->LHS.compare("lvalue")) && 
    (!t->children[1]->type.compare("int") || !t->children[1]->type.compare("int*"))) {
        t->type = t->children[1]->type;
    }
    // if (!prevLine.compare("RETURN return")) {
    //     // std::cout << "prevLine: " << prevLine << std::endl;
    //     if (t->type != "int") {
    //         std::cerr << "ERROR - RETURN TYPE IS NOT INT" << std::endl;
    //     } else {
    //         prevLine = "";
    //         linesAgo = 0;
    //     }
    // }
}

void verifyChildTypes(Tree *t) {
    if(!t->LHS.compare("dcls") && t->children.size() > 1) {
        //get type of the dcl
        std::string dclChildType = t->children[1]->children[1]->type;
        //get type of rhs of expression
        std::string rhsType = t->children[3]->LHS;
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

void updateParentsType(Tree *t) {
    for(Tree *child: t->children) {
       updateParentsType(child);
    }
    //check if children are of any type
    if(t->rule.compare("factor STAR factor") != 0 && t->rule.compare("factor AMP lvalue") != 0 && t->rule.compare("lvalue STAR factor") != 0
    && t->rule.compare("expr expr PLUS term") != 0  && t->rule.compare("expr expr MINUS term") != 0 && t->rule.compare("term term STAR factor") != 0
    && t->rule.compare("term term SLASH factor") != 0 && t->rule.compare("term term PCT factor") != 0) {
        if (t->children.size() > 1 && (!t->LHS.compare("expr") || !t->LHS.compare("term") || !t->LHS.compare("factor") || !t->LHS.compare("lvalue")) && 
        (!t->children[1]->type.compare("int") || !t->children[1]->type.compare("int*"))) {
            t->type = t->children[1]->type;
        } else if(!t->children.empty()){
            if (!t->children[0]->type.compare("int") || !t->children[0]->type.compare("int*")) {
                t->type = t->children[0]->type;    
            }
        }
    }
}

void verifyOperationTypes(Tree *t) {
    for(Tree *child: t->children) {            
        verifyOperationTypes(child);
    }
    // std::cout << "rule: " << t->rule << std::endl;
    if(!t->rule.compare("factor STAR factor") || !t->rule.compare("lvalue STAR factor") ) {//dereference
        //check if factor child type is int*, if it is, t type becomes int, otherwise, throw error
        
        std::string childType = t->children[1]->type;
        if(!childType.compare("int*")) {
            t->type = "int";
        } else {
            std::cerr << "ERROR - DEREFERENCING NON-INT VALUE1" << std::endl;
        }
    } else if(!t->rule.compare("factor NEW INT LBRACK expr RBRACK")) {//dereference
        //check if expr child type is int*, if it is, t type becomes int, otherwise, throw error
        updateParentsType(t);
        std::string childType = t->children[3]->type;
        if(!childType.compare("int")) {
            t->type = "int*";
        } else {
            std::cerr << "ERROR - DEREFERENCING NON-INT VALUE2" << std::endl;
        }
    } else if(!t->rule.compare("factor AMP lvalue")) {//take address
        //check if lvalue is of type int, if it is, t becomes a pointer, oterwise throw error
        updateParentsType(t);        
        std::string childType = t->children[1]->type;
        if(!childType.compare("int")) {
            t->type = "int*";
        } else {
            std::cerr << "ERROR - DEREFERENCING NON-INT VALUE3" << std::endl;
        }
    } else if(!t->rule.compare("expr expr PLUS term")) {//take address
        //if expr and term are two ints or one int and one pointer, add, otherwise throw error
        updateParentsType(t);    
        std::string exprType = t->children[0]->type;
        std::string termType = t->children[2]->type;
        
        if(!exprType.compare("int") && !termType.compare("int")) {
                        // std::cout << "setting type to int" << std::endl;

            t->type = "int";
        } else if((!exprType.compare("int*") && !termType.compare("int")) || (!exprType.compare("int") && !termType.compare("int*"))) {
            // std::cout << "setting type to int*" << std::endl;
            t->type = "int*";
        } else {
            std::cerr << "ERROR - DEREFERENCING NON-INT VALUE4" << std::endl;
        }
    } else if(!t->rule.compare("expr expr MINUS term")) {//take address
        //if expr and term are two ints or one int and one pointer, add, otherwise throw error
        updateParentsType(t);    
        std::string exprType = t->children[0]->type;
        std::string termType = t->children[2]->type;
        
        if((!exprType.compare("int") && !termType.compare("int")) ||(!exprType.compare("int*") && !termType.compare("int*"))) {
            t->type = "int";
        } else if(!exprType.compare("int*") && !termType.compare("int")) {
            // std::cout << "setting type to int*" << std::endl;
            t->type = "int*";
        } else {
            std::cerr << "ERROR - DEREFERENCING NON-INT VALUE5" << std::endl;
        }
    } else if((!t->rule.compare("term term STAR factor")) || (!t->rule.compare("term term SLASH factor")) || !t->rule.compare("term term PCT factor")) {//take address
        //if expr and term are two ints or one int and one pointer, add, otherwise throw error
        updateParentsType(t);    
        std::string exprType = t->children[0]->type;
        std::string termType = t->children[2]->type;
        
        if((!exprType.compare("int") && !termType.compare("int"))) {
            t->type = "int";
        } else {
            std::cerr << "ERROR - DEREFERENCING NON-INT VALUE6" << std::endl;
        }
    }  
    updateParentsType(t); // ripple upwards the changes that were made to the types from the operations
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

void verifyReturnType(Tree *t) {
    std::string returnType = t->children[1]->children[0]->children[11]->type;
    if(returnType != "int") {
        std::cerr << "ERROR - RETURN TYPE NOT INT" << std::endl;
    }
}

int main() {
    Tree *tree = new Tree();
    buildTree(tree);
    verifyChildTypes(tree);
    verifyOperationTypes(tree);
    verifyReturnType(tree);
    printTree(tree);
    deleteTree(tree);
    return 0;
}