// Idea - create two queues: reduction sequence and input sequence(where values are strings), and a map of strings
// going from RHS to LHS as given by the .ACTIONS component in the file.
// Initialize the input sequence QUEUE as the .INPUT component in the file
// Initialize the reduction sequence VECTOR as empty
// Initialize map of rules LHS -> RHS (strings)


//shift
// pop front of input sequence queue
// push back poppped item to reduction sequence vector


//reduce
//find the RHS of reduce rule specified, count the number of tokens, remove that many from the reduction vector(pop-back that many times),
// then push every token from the LHS of the rule to the reduction vector
//special case to check for first, if RHS of reduce rule is .EMPTY, ignore the first part(popping from reduction vector), simply push every token
// on the LHS to the reduction vector


//print 
// iterate through reduction sequence printing every token, print a '.', then iterate through the input sequence and print every token

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <queue>
 
int main()
{
    int readingStage = 0; //0 -> .CFG, 1 -> .INPUT, 2 -> .ACTIONS, 3 -> .END
    std::queue<std::string> inputSeq;
    std::vector<std::string> reduceSeq;
    std::string line;
    std::vector<std::pair<std::string, std::string>> rules;
 
    while (std::getline(std::cin, line) && !line.empty() && (readingStage != 3)) {
        //std::cout << line << std::endl;
        if(!line.compare(".CFG")) {
            readingStage = 0;
        } else if(!line.compare(".INPUT")) {
            readingStage = 1;
        } else if(!line.compare(".ACTIONS")) {
            readingStage = 2;
        } else if(!line.compare(".END")) {
            readingStage = 3;
        } else if(readingStage == 0) {
            //add rule to rules
            std::stringstream ssin(line);
            std::pair<std::string, std::string> toInsert;
            std::string first;
            std::string rest = "";
            //Get LHS
            ssin >> first;
            toInsert.second = first;
            //Get RHS
            ssin >> first;
            rest += first;
            while (ssin.good()) {
                ssin >> first;
                rest += " ";
                rest += first;
            }
            toInsert.first = rest;
            rules.push_back(toInsert);
            // std::cout << "inserted " << toInsert.first << " and " << toInsert.second << std::endl; 
        } else if(readingStage == 1) {
            //treat input as 1 string! add input to input queue - dont forget about whitespace
            std::stringstream ssin(line);
            std::string next;
            while (ssin.good()) {
                ssin >> next;
                inputSeq.push(next);
                // std::cout << "inserting as input token: " << next << std::endl;
            }
        } else if(readingStage == 2) {
            //take the appropriate action
            if(!line.compare("print")) {
                //print reduction sequence
                for(int i = 0; i < reduceSeq.size(); i++) {
                    std::cout << reduceSeq[i] << " ";
                }
                std::cout << ".";
                //print input sequence
                std::queue tmp_q = inputSeq;
                while (!tmp_q.empty()) {
                    std::string q_element = tmp_q.front();
                    std::cout <<" " << q_element;
                                        
                    tmp_q.pop();
                } 
                std::cout << std::endl;

            } else if(!line.compare("shift")) {
                //shift
                std::string front = inputSeq.front();
                inputSeq.pop();
                reduceSeq.push_back(front);
            } else {
                //reduce
                // extract rule number index 7
                int ruleNum = stoi(line.substr(7));
                std::string RHS = rules[ruleNum].first;
                //remove RHS from reduceSeq
                if(RHS.compare(".EMPTY") != 0) {
                    std::stringstream ssin(RHS);
                    std::string next;
                    while (ssin.good()) {
                        ssin >> next;
                        // std::cout << "removing: " << next << " by rule " << ruleNum << std::endl;
                        reduceSeq.pop_back();
                    }
                }
                //add LHS to reduceSeq
                std::string LHS = rules[ruleNum].second;
                reduceSeq.push_back(LHS); // it's only one string so no need to iterate over stream
                // std::cout << "adding: " << LHS << std::endl;
            }
        } 
    }
    // std::cout << "finished reading..." << std::endl;
    //debuggin
    //print rules
    // for(std::map<std::string, std::string>::const_iterator it = rules.begin();
    // it != rules.end(); ++it)
    //     {
    //         std::cout << it->first << " " << it->second << "\n";
    //     }
    // for(int i = 0; i < rules.size(); i++ ) {
    //     std::cout << "RULE " << i << ": " << rules[i].first << ", " << rules[i].second << std::endl;
    // }
    return 0;
}
