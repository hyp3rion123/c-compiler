// Ideas:
// Keep track of number of succesful shifts in some int
// Stack of states(vector)
// Map of (state, symbol) -> reduction rule (reductions)
// Map of (state, symbol) -> state (transitions)
// On every state, check if next symbol exists (from input string)
//       if it does, check if a reduction rule exists for next symbol
//              if it does, reduce using obtained rule:
//                  1. Check how many tokens we are reducing back
//                  2. Store LHS of rule being used
//                  3. Pop that many states from state stack
//                  4. Shift from new state using LHS
//              otherwise, shift, increment # of successful shifts
//                  1. Search for a shift in map of transitions
//                  2. If none exists, return error
//                  3. Otherwise, push to states stack and move input/reduction vectors as usual
//       otherwise, check if reduction rule exists for .ACCEPT, 
//              if it does, we are done
//              otherwise, output to stderr 
//

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <queue>
 
int main()
{
    int readingStage = 0; //0 -> .CFG, 1 -> .INPUT, 2 -> .TRANSITIONS, 3 -> REDUCTIONS, 4 -> END
    std::queue<std::string> inputSeq;
    std::vector<std::string> reduceSeq;
    std::map<std::pair<int, std::string>, int> reductions;
    std::map<std::pair<int, std::string>, int> transitions;
    std::vector<int> states; //stack
    std::string line;
    std::vector<std::pair<std::string, std::string>> rules;
    int tokens_shifted = 0;
    
    //push first state to states stack by default
    states.push_back(0);

    while (std::getline(std::cin, line) && !line.empty()) {
        //  std::cout << "reading stage is " << readingStage << std::endl;
        if(!line.compare(".CFG")) {
            readingStage = 0;
        } else if(!line.compare(".INPUT")) {
            readingStage = 1;
        } else if(!line.compare(".TRANSITIONS")) {
            readingStage = 2;
        } else if(!line.compare(".REDUCTIONS")) {
            readingStage = 3;
        } else if(!line.compare(".END")) {
            readingStage = 4;
            //std::cout << "reading stage is 4" << std::endl;
            break;
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
            //std::cout << "inserted " << toInsert.first << " and " << toInsert.second << std::endl; 
        } else if(readingStage == 1) {
            //treat input as 1 string! add input to input queue - dont forget about whitespace
            std::stringstream ssin(line);
            std::string next;
            while (ssin.good()) {
                ssin >> next;
                inputSeq.push(next);
                //std::cout << "inserting as input token: " << next << std::endl;
            }
        } else if(readingStage == 2) {
            //create a state/symbol pair that transitions to another state and add to transitions map
            std::stringstream ssin(line);
            std::string temp;
            int from_State;
            int to_State;
            std::string trans_symbol;
            ssin >> temp; //from_state
            from_State = stoi(temp);
            ssin >> trans_symbol; //from_state
            ssin >> temp; //to_State
            to_State = stoi(temp);

            std::pair<int, std::string> key = make_pair(from_State, trans_symbol);
            std::pair< std::pair<int, std::string>, int> toInsert = make_pair(key, to_State);
            transitions.insert(toInsert);
        } else if(readingStage == 3) {
            //create a state/tag pair that refers to a rule and add to reductions map
            std::stringstream ssin(line);
            std::string temp;
            int state_number;
            int rule_number;
            std::string tag;
            ssin >> temp; //state_number
            state_number = stoi(temp);
            ssin >> temp; //rule_number
            rule_number = stoi(temp);
            ssin >> tag; //tag

            std::pair<int, std::string> key = make_pair(state_number, tag);
            std::pair< std::pair<int, std::string>, int> toInsert = make_pair(key, rule_number);
            reductions.insert(toInsert);
            // //std::cout << "inserted " << key.first << ", " << key.second << ": " << rule_number << std::endl;
        }        
    }
    //Print initial seq
    std::cout << ".";
    //print input sequence
    std::queue tmp_q = inputSeq;
    while (!tmp_q.empty()) {
        std::string q_element = tmp_q.front();
        std::cout <<" " << q_element;
                            
        tmp_q.pop();
    } 
    std::cout << std::endl;
    while (true) {
        //Check if there are any more symbols in input
        //std::cout << inputSeq.size() << std::endl;
        if(!inputSeq.empty()) {
            // //std::cout << "hello" << std::endl;
            std::string next_token = inputSeq.front();
            int curr_state = states.back();
            //check if a reduction rule exists for the next symbol
            std::pair<int, std::string> to_search = make_pair(curr_state, next_token);
            std::map<std::pair<int, std::string>, int>::iterator it;            
            it = reductions.find(to_search);
            if(it != reductions.end()) {//reduction found-reduce
                //std::cout << "reduction found...reducing" << std::endl;
                std::string RHS = rules[it->second].first;
                int states_popped = 0;
                //remove RHS from reduceSeq
                if(RHS.compare(".EMPTY") != 0) {
                    std::stringstream ssin(RHS);
                    std::string next;
                    while (ssin.good()) {
                        ssin >> next;
                        //std::cout << "removing: " << next << " by rule " << it->second << std::endl;
                        reduceSeq.pop_back();
                        states_popped += 1;
                    }
                }
                //std::cout << "popping " << states_popped << " states" << std::endl;
                //store LHS of current state
                std::string LHS = rules[it->second].second;
                for (int i = 0; i < states_popped; i++) {
                    states.pop_back();
                }
                //find the transition from new state
                to_search = make_pair(states.back(), LHS);
                it = transitions.find(to_search);
                //shift on prev LHS
                states.push_back(it->second);//push new state
                //std::cout << "after reduce, transitioning to state " << it->second << std::endl;
                //update reduce seq                    
                reduceSeq.push_back(LHS); // it's only one string so no need to iterate over stream
                //std::cout << "adding: " << LHS << std::endl;
                //std::cout << "reduction done " << std::endl;
                //print reduction sequence
                //std::cout << "ACTUAL OUTPUT: " << std::endl;
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
            } else {//reduction not found-shift
                //std::cout << "reduction not found...attempting shift" << std::endl;
                //build search in transitions
                it = transitions.find(to_search);
                if (it != transitions.end()) {
                    //std::cout << "transition found to state " << it->second << std::endl;
                    //shift found
                    //modify input/reduce seqs
                    std::string front = inputSeq.front();
                    inputSeq.pop();
                    reduceSeq.push_back(front);
                    //add to state stack
                    states.push_back(it->second);
                    tokens_shifted += 1;
                    //std::cout << "shifting, popped " << front << " from input " << " moving to state " << it->second << std::endl;
                    //print reduction sequence
                    //std::cout << "ACTUAL OUTPUT: " << std::endl;
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
                } else {
                    //no shift - ERROR
                    //std::cout << "no transition found...outputting error" << std::endl;
                    std::cerr << "ERROR at " << tokens_shifted+1 << std::endl;
                    return 0;
                }
            }
        } else {
            //inputseq is empty - check if state accepts
            std::string accept = ".ACCEPT";
            int curr_state = states.back();
            //check if a reduction rule exists for the next symbol
            std::pair<int, std::string> to_search = make_pair(curr_state, accept);
            std::map<std::pair<int, std::string>, int>::iterator it;            
            it = reductions.find(to_search);
            if(it != reductions.end()) {//state is accepting
                //reduce to initial state
                std::string LHS_First = rules[0].second;
                //clear reduceSeq
                while(!reduceSeq.empty()) {
                    reduceSeq.pop_back();
                }
                //push LHS
                reduceSeq.push_back(LHS_First);
            }   
            //print reduction sequence
            //std::cout << "ACTUAL OUTPUT: " << std::endl;
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
            return 0;
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
    // for(auto it = transitions.begin();
    // it != transitions.end(); ++it)
    //     {
    //         std::cout << it->first.first << " " << it->first.second << ", " << it->second << "\n";
    //     }
    // for(auto it = reductions.begin();
    // it != reductions.end(); ++it)
    //     {
    //         std::cout << it->first.first << " " << it->first.second << ", " << it->second << "\n";
    //     }
    return 0;
}
