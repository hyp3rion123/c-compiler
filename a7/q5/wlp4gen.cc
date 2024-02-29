#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>

std::map<std::string, int> stackPositions; // symbol -> offset
std::map<std::string, std::pair<std::string, int>> symbolTable; // symbol -> (type, value)
int globalLabelCounter = 0;

class Tree {
    public: 
        std::string LHS; // e.g LHS of expr expr PLUS term is expr
        std::string rule; // e.g expr expr PLUS term
        std::vector<Tree *> children;
        std::string type = "";
};

void buildTree(Tree *t) {
    //Init
    std::string tempString;
    std::string nextLine;
    std::getline(std::cin, nextLine);

    //Update struct
    t->rule = nextLine;//append to vector of children
    std::stringstream childrenStream(nextLine);
    childrenStream >> t->LHS;
    while (childrenStream >> tempString && tempString != "") {
        //if we have reached : type, add type and exit loop
        if(!tempString.compare(":")) {
            childrenStream >> tempString;
            t->type = tempString;
            break;
        }
        Tree *child = new Tree();
        if (!std::all_of(t->LHS.begin(), t->LHS.end(), [](unsigned char c){ return std::isupper(c); }) && tempString.compare(".EMPTY") != 0) {
            buildTree(child);
        }
        if(child->rule.compare("") != 0) {
            // std::cout << "adding child " << child->rule << " to " << t->rule << std::endl;
            t->children.push_back(child);
        } else {
            delete child;
        }
    }  
}

void printTree(Tree *t) {
    if(t->rule.compare(".EMPTY") != 0) {
        std::cout << t->rule << std::endl;
        for(Tree *child: t->children) {
            // std::cout << "printing child " << child->LHS << " of " << t->LHS << std::endl;
            printTree(child);
        }
    }
}

void deleteTree(Tree *t) {
    for(Tree *child: t->children) {
        // std::cout << "printing child " << child->LHS << " of " << t->LHS << std::endl;
        deleteTree(child);
    }
    delete t;
}

void push(int reg) {
    std::cout << "sw $" + std::to_string(reg) + ", -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4 ; update stack pointer" << std::endl;
}

void pop(int reg) {    
    std::cout << "add $30, $30, $4 ; update stack pointer" << std::endl;
    std::cout << "lw $" + std::to_string(reg) + ", -4($30)" << std::endl;
}

void buildSymbolTableDeclrs(Tree *t, std::string &toReturn) {
    // std::cout << "children: " <<std::endl;
    // for(int i = 0; i < t->children.size(); i++) {
    //         std::cout << "child " << i << t->children[i]->rule << std::endl;
    // }
    //Extract first child
    std::stringstream children(t->rule);
    std::string child;
    children >> child;
    children >> child;
    if(!child.compare("dcls")) {
        buildSymbolTableDeclrs(t->children[0], toReturn);
        //Add symbol to table
        Tree *IDNode = t->children[1]->children[1];
        // std::cout << "id node: " << IDNode->rule << t->children.size() << std::endl;
        std::stringstream innerStream(IDNode->rule);
        innerStream >> child; //flush
        innerStream >> child;
        Tree *ValNode = t->children[3];
        std::string val;
        std::stringstream innerStream2(ValNode->rule);
        innerStream2 >> val; //flush
        innerStream2 >> val;
        std::pair<std::string, int> offsetPair = make_pair(child, (symbolTable.size()+2)*-4);
        std::pair<std::string, int> nodePair = make_pair(IDNode->type, stoi(val));
        std::pair<std::string, std::pair<std::string, int>> toInsert = make_pair(child, nodePair);
        stackPositions.insert(offsetPair);
        symbolTable.insert(toInsert);
        auto it = stackPositions.find(child);
        // std::cout << "inserted " << it->first << " at offset " << it->second << std::endl;
        toReturn += "lis $5\n.word " + val + " ; <-- " + child + "\n";
        // std::cout << "current return text: " << toReturn << std::endl;
        toReturn += "sw $5, -4($30)\nsub $30, $30, $4 ; update stack pointer\n";
    }
}

std::string buildSymbolTable(Tree *t) {
    //Insert a
    Tree *aChild = t->children[1]->children[0]->children[3]->children[1];
    //Extract ID
    std::stringstream childrenStream(aChild->rule);
    std::string child;
    childrenStream >> child; //flush
    childrenStream >> child;
    std::pair<std::string, int> offsetPair = make_pair(child, 0);
    std::pair<std::string, int> aPair = make_pair(aChild->type, 0);
    std::pair<std::string, std::pair<std::string, int>> aInsert = make_pair(child, aPair);
    stackPositions.insert(offsetPair);
    // symbolTable.insert(aInsert);

    //Insert b
    Tree *bChild = t->children[1]->children[0]->children[5]->children[1];
    //Extract ID
    std::stringstream childrenStream2(bChild->rule);
    childrenStream2 >> child; //flush
    childrenStream2 >> child;
    offsetPair = make_pair(child, -4);
    std::pair<std::string, int> bPair = make_pair(bChild->type, -4);
    std::pair<std::string, std::pair<std::string, int>> bInsert = make_pair(child, bPair);
    stackPositions.insert(offsetPair);
    // symbolTable.insert(bInsert);

    //Insert any other declrs
    std::string toReturn = "";
    buildSymbolTableDeclrs(t->children[1]->children[0]->children[8], toReturn);
    return toReturn;
}

void code(Tree *t) {
    if(!t->LHS.compare("expr")) {
        if(!t->children[0]->LHS.compare("expr")){
            code(t->children[0]);
            push(3);
            code(t->children[2]);
            pop(5);
            // std::cout << t->children[1]->LHS << std::endl;
            if(!t->children[1]->LHS.compare("PLUS")) {
                std::cout << "add $3, $5, $3" << std::endl;
            } else if(!t->children[1]->LHS.compare("MINUS")) {
                std::cout << "sub $3, $5, $3" << std::endl;
            }
        } else if(!t->children[0]->LHS.compare("term")) {
            code(t->children[0]);
        }
    } else if(!t->LHS.compare("factor")) {
        // std::cout << "here" << t->children[0]->rule;
        if(!t->children[0]->LHS.compare("LPAREN")) {
            code(t->children[1]);
        } else if(!t->children[0]->LHS.compare("ID")) {
            code(t->children[0]);
        } else if(!t->children[0]->LHS.compare("NUM")) {
            code(t->children[0]);
        }
    } else if(!t->LHS.compare("statement")) {
        if(!t->children[0]->LHS.compare("lvalue")) {
            code(t->children[2]);
            //Extract lvalue ID
            Tree *lvalNode = t->children[0]->children[0];
            std::stringstream IDStream(lvalNode->rule);
            std::string lval;
            IDStream >> lval;
            IDStream >> lval;
            //Search for offset of the lval
            auto it = stackPositions.find(lval);
            std::cout << "sw $3, " << it->second << "($29)" << std::endl;
        } else if(!t->children[0]->LHS.compare("IF")) {
            std::string label = "else" + std::to_string(globalLabelCounter);
            std::string label2 = "endif" + std::to_string(globalLabelCounter);
            globalLabelCounter++;
            code(t->children[2]); //test
            std::cout << "beq $3, $0, " << label << std::endl;
            code(t->children[5]);
            std::cout << "beq $0, $0, " << label2 << std::endl;
            std::cout << label << ":" << std::endl;
            code(t->children[9]);
            std::cout << label2 << ":" << std::endl;
        } else if(!t->children[0]->LHS.compare("WHILE")) {
            std::string label = "loop" + std::to_string(globalLabelCounter);
            std::string label2 = "endWhile" + std::to_string(globalLabelCounter);
            globalLabelCounter++;
            std::cout << label << ":" << std::endl;
            code(t->children[2]); //test
            std::cout << "beq $3, $0, " << label2 << std::endl;
            code(t->children[5]);
            std::cout << "beq $0, $0, " << label << std::endl;
            std::cout << label2 << ":" << std::endl;
        } else if(!t->children[0]->LHS.compare("PRINTLN")) {
            push(1);
            code(t->children[2]);
            std::cout << "add $1, $3, $0" << std::endl;
            push(31);
            std::cout << "lis $5\n.word print\njalr $5" << std::endl;
            pop(31);
            pop(1);
        }
    } else if(!t->LHS.compare("term")) {        
        if(!t->children[0]->LHS.compare("factor")) {
            code(t->children[0]);
        } else if(!t->children[0]->LHS.compare("term")) {
            code(t->children[0]);
            push(3);
            code(t->children[2]);
            pop(5);
            if(!t->children[1]->LHS.compare("STAR")) {
                std::cout << "mult $5, $3\nmflo $3" << std::endl;
            } else if(!t->children[1]->LHS.compare("SLASH")) {
                std::cout << "div $5, $3\nmflo $3" << std::endl;
            } else if(!t->children[1]->LHS.compare("PCT")) {
                std::cout << "div $5, $3\nmfhi $3" << std::endl;
            }
        }
    } else if(!t->LHS.compare("ID")) {
        //Extract ID
        std::stringstream varNameStream(t->rule);
        std::string varName;
        varNameStream >> varName;
        varNameStream >> varName;
        auto it = stackPositions.find(varName);
        // std::cout << "stack search for " << varName << " return " << it->second << std::endl;
        std::cout << "lw $3, " << it->second << "($29) ; <-- code(" << varName << ")" << std::endl;
        
        // it = stackPositions.find("log");
        // std::cout << "stack search for log" << varName << " return " << it->second << std::endl;
    } else if(!t->LHS.compare("NUM")) {
        //Extract NUM
        std::stringstream valueStream(t->rule);
        std::string value;
        valueStream >> value;
        valueStream >> value;
        std::cout << "lis $3\n.word " << value << std::endl;
    } else if(!t->LHS.compare("test")) {
        if(!t->children[1]->LHS.compare("LT") || !t->children[1]->LHS.compare("GE")) {
            code(t->children[0]);
            push(3);
            code(t->children[2]);
            pop(5);
            std::cout << "slt $3, $5, $3" << std::endl;
            if(!t->children[1]->LHS.compare("GE")) {
                std::cout << "sub $3, $11, $3" << std::endl;
            }
        } else if(!t->children[1]->LHS.compare("GT") || !t->children[1]->LHS.compare("LE")) {
            code(t->children[2]);
            push(3);
            code(t->children[0]);
            pop(5);
            std::cout << "slt $3, $5, $3" << std::endl;
            if(!t->children[1]->LHS.compare("LE")) {
                std::cout << "sub $3, $11, $3" << std::endl;
            }
        } else if(!t->children[1]->LHS.compare("EQ") || !t->children[1]->LHS.compare("NE")) {
            code(t->children[0]);
            push(3);
            code(t->children[2]);
            pop(5);
            std::cout << "slt $6, $3, $5 ; $6 = $3 < $5" << std::endl;
            std::cout << "slt $7, $5, $3 ; $7 = $5 < $3" << std::endl;
            std::cout << "add $3, $6, $7" << std::endl;
            if(!t->children[1]->LHS.compare("EQ")) {
                std::cout << "sub $3, $11, $3" << std::endl;
            }
        }
    } else if(!t->LHS.compare("statements")) {
        if(t->children.size() > 0 && t->children[0]->LHS.compare(".EMPTY") != 0) {
            code(t->children[0]);
            code(t->children[1]);
        }
    }
}

void buildPrologue(std::string prologueText) {
    // std::map<std::string, int> stackPositions;
    int currentPointer = -8; //always pushing a and b
    // int varsSkipped = 0; //skip a and b push since we push manually
    std::cout << "; begin prologue" << std::endl;
    std::cout << ".import print" << std::endl;
    std::cout << "lis $4" << std::endl;
    std::cout << ".word 4" << std::endl;
    std::cout << "lis $11" << std::endl;
    std::cout << ".word 1" << std::endl;
    std::cout << "sub $29, $30, $4 ; setup frame pointer" << std::endl;
    push(1);
    push(2);
    std::cout << prologueText << std::endl;
    //Store all variables
    // for(auto it = symbolTable.begin(); it != symbolTable.end(); it++) {
        // if (varsSkipped == 0) {
        //     //insert a in stack
        //     std::pair<std::string, int> posPair = make_pair(it->first, 0);
        //     stackPositions.insert(posPair);
        // } else if (varsSkipped == 1) {
        //     //insert b in stack
        //     std::pair<std::string, int> posPair = make_pair(it->first, -4);
        //     stackPositions.insert(posPair);
        // }
        // if(varsSkipped > 1) {
            // std::cout << "value of var " << it->first << " is " << it->second.second << std::endl;
            // std::cout << "lis $5\n.word " << it->second.second << " ; <-- " << it->first << std::endl;
            // push(5);
            // std::pair<std::string, int> posPair = make_pair(it->first, currentPointer);
            // stackPositions.insert(posPair);
            // currentPointer-=4;
            // std::cout << "new stack pos size: " << stackPositions.size() << std::endl;
        // }
        // varsSkipped++;
    // }

    std::cout << "; end prologue\n" << std::endl;
    // return stackPositions;
}

void buildEpilogue() {
    std::cout << "; begin epilogue" << std::endl;
    //Update stack pointer
    for(auto it = symbolTable.begin(); it != symbolTable.end(); it++) {
        std::cout << "add $30, $30, $4" << std::endl;
    }

    std::cout << "jr $31" << std::endl;
}

void buildReturn( Tree *t) {
    //Extract return expr
    Tree *returnExpr = t->children[1]->children[0]->children[11];
    code(returnExpr);
}

// void buildStatementsHelper(Tree *t) {
//     if(t->children.size() > 0 && t->children[0]->LHS.compare(".EMPTY") != 0) {
//         buildStatementsHelper(t->children[0]);
//         code(t->children[1]);
//     }
// }

void buildStatements(Tree *t) {
    //Extract statements
    Tree *statements = t->children[1]->children[0]->children[9];
    // std::cout << statements->LHS << statements->children.size() << std::endl;
    // if(statements->children.size() > 0 && statements->children[0]->LHS.compare(".EMPTY") != 0) {
    //     buildStatementsHelper(statements->children[0]);
    //     code(statements->children[1]);
    // }
    code(statements);
}


int main() {
    Tree *tree = new Tree();
    buildTree(tree);
    std::string prologueText = buildSymbolTable(tree);
    buildPrologue(prologueText);
    buildStatements(tree);
    buildReturn(tree);
    buildEpilogue();
    // printTree(tree);
    deleteTree(tree);
    return 0;
}