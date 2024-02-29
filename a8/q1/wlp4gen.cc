#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>

std::map<std::string, std::map<std::string, std::pair<std::string, int>>> symbolTable; // procedure ID -> (symbol -> (type, offset))
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

void buildSymbolTableDeclrs(Tree *t, std::string &toReturn, std::string procedureID) {
    // std::cout << "PROCEDURE ID: " << procedureID << std::endl;
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
        buildSymbolTableDeclrs(t->children[0], toReturn, procedureID);
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
        int offset = symbolTable[procedureID].size();
        if(!procedureID.compare("main")) {
            offset += 2;
        } else {
            offset -= symbolTable["main"].size();
        }
        std::pair<std::string, int> offsetPair = make_pair(IDNode->type, offset*-4);
        std::pair<std::string, std::pair<std::string, int>> toInsert = make_pair(child, offsetPair);
        // std::cout << "inserted " << toInsert.first << " of type " << toInsert.second.first << "at offset " << toInsert.second.second << std::endl;
        symbolTable[procedureID].insert(toInsert);
        // auto it = stackPositions.find(child);
        // std::cout << "inserted " << it->first << " at offset " << it->second << std::endl;
        // toReturn += "lis $5\n.word " + val + " ; <-- " + child + "\n";
        std::cout << "lis $5\n.word " + val + " ; <-- " + child + "\n";
        // std::cout << "current return text: " << toReturn << std::endl;
        // toReturn += "sw $5, -4($30)\nsub $30, $30, $4 ; update stack pointer\n";
        std::cout << "sw $5, -4($30)\nsub $30, $30, $4 ; update stack pointer\n";
    }
}

std::string buildSymbolTable(Tree *t, std::string procedureID) {
    //Get wain 
    Tree *root = t->children[1];
    while(root->children[0]->LHS.compare("main") != 0) {
        root = root->children[1];
    }
    root = root->children[0];
    //root contains main -> ....
    //Insert a
    Tree *aChild = root->children[3]->children[1];
    //Extract ID
    std::stringstream childrenStream(aChild->rule);
    std::string child;
    childrenStream >> child; //flush
    childrenStream >> child;
    // std::pair<std::string, int> offsetPair = make_pair(aChild->type, 0);
    std::pair<std::string, int> aPair = make_pair(aChild->type, 0);
    std::pair<std::string, std::pair<std::string, int>> aInsert = make_pair(child, aPair);
    // stackPositions.insert(offsetPair);
    symbolTable[procedureID].insert(aInsert);

    //Insert b
    Tree *bChild = root->children[5]->children[1];
    //Extract ID
    std::stringstream childrenStream2(bChild->rule);
    childrenStream2 >> child; //flush
    childrenStream2 >> child;
    // offsetPair = make_pair(child, -4);
    std::pair<std::string, int> bPair = make_pair(bChild->type, -4);
    std::pair<std::string, std::pair<std::string, int>> bInsert = make_pair(child, bPair);
    // stackPositions.insert(offsetPair);
    symbolTable[procedureID].insert(bInsert);

    //Insert any other declrs
    std::string toReturn = "";
    buildSymbolTableDeclrs(root->children[8], toReturn, procedureID);
    return toReturn;
}

// void argCount(Tree *argList, int &n) {
//     if(argList->children.size() > 1) {//has another arglist
//         argCount(argList->children[2], n);
//         n++;
//     }
// }

void addProcedureParams(Tree *t, std::string procedureID) {
    // t = parmlist -> ...
    //Add dcl to symboltable
    Tree* typeNode = t->children[0]->children[0]->children[0];
    std::string type;
    std::stringstream typeParams(typeNode->rule);
    typeParams >> type;
    typeParams >> type;
    // std::cout << "my size: " << symbolTable[procedureID].size() << " main size: " << symbolTable["main"].size() << std::endl;
    // for(auto it = symbolTable[procedureID].begin(); it != symbolTable[procedureID].end(); it++) {
    //     std::cout << it->first << ", " << it->second.first << ", " << it->second.second << std::endl;
    // }
    // std::cout << "computing offset, my size: " << symbolTable[procedureID].size() << " main size: " << symbolTable["main"].size() << std::endl;
    int offset = (symbolTable[procedureID].size() - symbolTable["main"].size()) * -4;
    // std::cout << "adding node with offset " << offset << " and type " << type << std::endl;
    Tree *idNode = t->children[0]->children[1];
    std::string id;
    std::stringstream idParams(idNode->rule);
    idParams >> id;
    idParams >> id;
    auto searchDuplicate = symbolTable[procedureID].find(id);
    if(searchDuplicate != symbolTable[procedureID].end()) {//rename duplicate in main
    // std::cout << "found duplicate id" << id;
        std::pair<std::string, std::pair<std::string, int>> toReturn = make_pair(searchDuplicate->first, searchDuplicate->second);
        std::string newID = toReturn.first + "old";
        toReturn.first = newID;
        symbolTable[procedureID].erase(id);
        symbolTable[procedureID].insert(toReturn);
        // std::cout << procedureID << " table changed value for " << id << " to " << symbolTable[procedureID][id].first << std::endl;
        // std::cout << "new value is " << symbolTable[procedureID][id].first << std::endl;
    }
    // std::cout << "new value is " << symbolTable[procedureID][id].first << std::endl;
    std::pair<std::string, int> innerPair = std::make_pair(type, offset);
    std::pair<std::string, std::pair<std::string, int>> mapPair = make_pair(id, innerPair);
    // std::cout << "IN ADDPROCPARAM: " << mapPair.first << ", " << mapPair.second.first << ", " << mapPair.second.second << std::endl;
    // std::cout << symbolTable[procedureID].find(mapPair.first)->first << std::endl;
    symbolTable[procedureID].insert(mapPair);
    // std::cout << "new symbol table is: " << std::endl;
    // for (auto it = symbolTable[procedureID].begin(); it != symbolTable[procedureID].end(); it++) {
    //     std::cout << it->first << ", " << it->second.first << ", " << it->second.second << std::endl;
    // }
    //Recurse
    if(t->children.size() > 1) {
        addProcedureParams(t->children[2], procedureID);
    }
}
void buildPrologueProcedure(Tree *t) {
    //Initialize new map
    std::map<std::string, std::pair<std::string, int>> procSymbolMap;
    //Extract map id
    Tree *idChild = t->children[1];
    std::stringstream idParams(idChild->rule);
    std::string id;
    idParams >> id;
    idParams >> id;
    std::pair<std::string, std::map<std::string, std::pair<std::string, int>>> newProcMap = std::make_pair(id, procSymbolMap);
    symbolTable.insert(newProcMap);

    //Insert values from symboltable with new offset
    for(auto it = symbolTable["main"].begin(); it != symbolTable["main"].end(); it++) {
        std::pair<std::string, std::pair<std::string, int>> mainPair = make_pair(it->first, it->second);
        mainPair.second.second += (symbolTable["main"].size() * 4);
        symbolTable[id].insert(mainPair);
        // std::cout << "inserting " << mainPair.first << ", (" << mainPair.second.first << ", " << mainPair.second.second << ") into " << id << ::std::endl;
    }
    //Insert params with offset
    if(t->children[3]->children.size() > 0) {
        addProcedureParams(t->children[3]->children[0], id);
    }
    
}

void popLocalVariables(Tree *t) {
    //Pop all declrs, t = procedure -> ....
    Tree *dcls = t->children[6];
    std::string firstChild;
    std::stringstream dclsParams(t->children[6]->rule);
    dclsParams >> firstChild;
    dclsParams >> firstChild;
    while(firstChild.compare(".EMPTY") != 0) {
        // std::cout << "POPPING" << std::endl;
        pop(3);
        dcls = dcls->children[0];
        std::stringstream dclsParams1(dcls->rule);
        dclsParams1 >> firstChild;
        dclsParams1 >> firstChild;
    }
}

void code(Tree *t, std::string procedureID) {
    // std::cout << "CALLED WITH " << procedureID << std::endl;
    if(!t->LHS.compare("expr")) {
        if(!t->children[0]->LHS.compare("expr")){
            code(t->children[0], procedureID);
            push(3);
            code(t->children[2], procedureID);
            pop(5);
            // std::cout << t->children[1]->LHS << std::endl;
            if(!t->children[1]->LHS.compare("PLUS")) {
                std::cout << "add $3, $5, $3" << std::endl;
            } else if(!t->children[1]->LHS.compare("MINUS")) {
                std::cout << "sub $3, $5, $3" << std::endl;
            }
        } else if(!t->children[0]->LHS.compare("term")) {
            code(t->children[0], procedureID);
        }
    } else if(!t->LHS.compare("factor")) {
        // std::cout << "int facotr logic " << t->rule << std::endl;
        if(!t->children[0]->LHS.compare("LPAREN")) {
            code(t->children[1], procedureID);
        } else if(!t->children[0]->LHS.compare("NUM")) {
            code(t->children[0], procedureID);
        } else if(!t->children[0]->LHS.compare("ID")) {
            
            if(t->children.size() == 1) {
                code(t->children[0], procedureID);
            } else if(t->children.size() > 2){ //factor → ID LPAREN arglist RPAREN OR factor → ID LPAREN RPAREN
                push(29);
                push(31);
                if(t->children[2]->LHS.compare("RPAREN") != 0) { // not rparen
                    code(t->children[2], procedureID); //arglist 
                }            
                //Extract procedure ID
                std::string jalrProcedureId;
                std::stringstream jalrParams(t->children[0]->rule);
                jalrParams >> jalrProcedureId;
                jalrParams >> jalrProcedureId;    
                std::cout << "lis $5\n.word " << jalrProcedureId << "\njalr $5" << std::endl;
                if(t->children[2]->LHS.compare("RPAREN") != 0) { // not rparen
                    //Count args
                    int args = 1;
                    Tree *argList = t->children[2];
                    while(argList->children.size() > 1) {
                        args++;
                        argList = argList->children[2];
                    }
                    // std::cout << procedureID << " has " << args << " arguments" << std::endl;
                    for(int idx = 0; idx < args; idx++) {
                        pop(21); //garbage
                    }
                }
                pop(31);
                pop(29);
            }
        }
    } else if(!t->LHS.compare("statement")) {
        if(!t->children[0]->LHS.compare("lvalue")) {
            code(t->children[2], procedureID);
            //Extract lvalue ID
            Tree *lvalNode = t->children[0]->children[0];
            std::stringstream IDStream(lvalNode->rule);
            std::string lval;
            IDStream >> lval;
            IDStream >> lval;
            //Search for offset of the lval
            // auto it = stackPositions.find(lval);
            auto it = symbolTable[procedureID].find(lval);
            std::cout << "sw $3, " << it->second.second << "($29)" << std::endl;
        } else if(!t->children[0]->LHS.compare("IF")) {
            std::string label = "else" + std::to_string(globalLabelCounter);
            std::string label2 = "endif" + std::to_string(globalLabelCounter);
            globalLabelCounter++;
            code(t->children[2], procedureID); //test
            std::cout << "beq $3, $0, " << label << std::endl;
            code(t->children[5], procedureID);
            std::cout << "beq $0, $0, " << label2 << std::endl;
            std::cout << label << ":" << std::endl;
            code(t->children[9], procedureID);
            std::cout << label2 << ":" << std::endl;
        } else if(!t->children[0]->LHS.compare("WHILE")) {
            std::string label = "loop" + std::to_string(globalLabelCounter);
            std::string label2 = "endWhile" + std::to_string(globalLabelCounter);
            globalLabelCounter++;
            std::cout << label << ":" << std::endl;
            code(t->children[2], procedureID); //test
            std::cout << "beq $3, $0, " << label2 << std::endl;
            code(t->children[5], procedureID);
            std::cout << "beq $0, $0, " << label << std::endl;
            std::cout << label2 << ":" << std::endl;
        } else if(!t->children[0]->LHS.compare("PRINTLN")) {
            push(1);
            code(t->children[2], procedureID);
            std::cout << "add $1, $3, $0" << std::endl;
            push(31);
            std::cout << "lis $5\n.word print\njalr $5" << std::endl;
            pop(31);
            pop(1);
        }
    } else if(!t->LHS.compare("term")) {        
        if(!t->children[0]->LHS.compare("factor")) {
            code(t->children[0], procedureID);
        } else if(!t->children[0]->LHS.compare("term")) {
            code(t->children[0], procedureID);
            push(3);
            code(t->children[2], procedureID);
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
        // auto it = stackPositions.find(varName);
        auto it = symbolTable[procedureID].find(varName);
        // std::cout << "stack search for " << varName << " return " << it->second << std::endl;
        std::cout << "lw $3, " << it->second.second << "($29) ; <-- code(" << varName << ")" << std::endl;
        
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
            code(t->children[0], procedureID);
            push(3);
            code(t->children[2], procedureID);
            pop(5);
            std::cout << "slt $3, $5, $3" << std::endl;
            if(!t->children[1]->LHS.compare("GE")) {
                std::cout << "sub $3, $11, $3" << std::endl;
            }
        } else if(!t->children[1]->LHS.compare("GT") || !t->children[1]->LHS.compare("LE")) {
            code(t->children[2], procedureID);
            push(3);
            code(t->children[0], procedureID);
            pop(5);
            std::cout << "slt $3, $5, $3" << std::endl;
            if(!t->children[1]->LHS.compare("LE")) {
                std::cout << "sub $3, $11, $3" << std::endl;
            }
        } else if(!t->children[1]->LHS.compare("EQ") || !t->children[1]->LHS.compare("NE")) {
            code(t->children[0], procedureID);
            push(3);
            code(t->children[2], procedureID);
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
            code(t->children[0], procedureID);
            code(t->children[1], procedureID);
        }
    } else if(!t->LHS.compare("arglist")) {
        code(t->children[0], procedureID); //code expr
        push(3);
        if(t->children.size() > 1) {
            code(t->children[2], procedureID); //recursively call code(arglist)
        }
    } else if(!t->LHS.compare("procedure")) {
        std::cout << procedureID << ":" << std::endl;
        std::cout << "sub $29, $30, $4" << std::endl;
        buildPrologueProcedure(t); //handles creation of symbol table and adds symbols based on offsets
        push(5);
        push(6);
        push(7);
        std::string dclsString = "";
        //Extract procedure ID
        std::string currID;
        std::stringstream procParams(t->children[1]->rule);
        procParams >> currID;
        procParams >> currID;
        buildSymbolTableDeclrs(t->children[6], dclsString, currID);//dcls
        code(t->children[7], currID);//stmts
        code(t->children[9], currID);//return expr
        //pop local variables
        popLocalVariables(t);
        //end pop local variables
        pop(7);
        pop(6);
        pop(5);
        std::cout << "jr $31" << std::endl;
    }
}

void buildPrologue(std::string prologueText) {
    // std::map<std::string, int> stackPositions;
    int currentPointer = -8; //always pushing a and b
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

    std::cout << "; end prologue\n" << std::endl;
}

void buildEpilogue() {
    std::cout << "; begin epilogue" << std::endl;
    //Update stack pointer
    for(auto it = symbolTable["main"].begin(); it != symbolTable["main"].end(); it++) {
        std::cout << "add $30, $30, $4" << std::endl;
    }

    std::cout << "jr $31" << std::endl;
}

void buildReturn( Tree *t) {
    Tree *root = t->children[1];
    while(root->children[0]->LHS.compare("main") != 0) {
        root = root->children[1];
    }
    root = root->children[0];
    //Extract return expr - ASSUMING MAIN
    Tree *returnExpr = root->children[11];
    code(returnExpr, "main");
}

void buildStatements(Tree *t) {
    //Extract statements
    Tree *statements = t->children[1]->children[0]->children[9];
    // std::cout << statements->LHS << statements->children.size() << std::endl;
    // if(statements->children.size() > 0 && statements->children[0]->LHS.compare(".EMPTY") != 0) {
    //     buildStatementsHelper(statements->children[0]);
    //     code(statements->children[1]);
    // }
    //ASSUMING MAIN
    code(statements, "main");
}

void buildProcedures(Tree *t) {
    // proceduresCode
    if(t->children[0]->LHS.compare("main") != 0) {
        // std::cout << t->children[0]->LHS << std::endl;
        buildProcedures(t->children[1]);
        //Extract procedure id for code()
        std::stringstream idParams(t->children[0]->children[1]->rule);
        std::string id;
        idParams >> id;
        idParams >> id;
        // std::cout << "id: " << id << "rule: " << t->children[0]->children[1]->rule << std::endl;
        code(t->children[0], id);
    }    
}

int main() {
    Tree *tree = new Tree();
    buildTree(tree);
    //Init wain symbol map
    std::map<std::string, std::pair<std::string, int>> wainMap;
    std::pair<std::string, std::map<std::string, std::pair<std::string, int>>> firstSymbolTable = make_pair("main", wainMap);
    symbolTable.insert(firstSymbolTable);
    //
    std::string prologueText = buildSymbolTable(tree, "main");
    buildPrologue(prologueText);
    buildStatements(tree);
    buildReturn(tree);
    buildEpilogue();
    buildProcedures(tree->children[1]);

    // printTree(tree);
    deleteTree(tree);
    return 0;
}