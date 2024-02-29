#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <utility>
#include <vector>
#include <algorithm>
#include <fstream>
#include <stdlib.h>
const std::string ALPHABET    = ".ALPHABET";
const std::string STATES      = ".STATES";
const std::string TRANSITIONS = ".TRANSITIONS";
const std::string INPUT       = ".INPUT";
const std::string EMPTY       = ".EMPTY";

bool isChar(std::string s) {
  return s.length() == 1;
}
bool isRange(std::string s) {
  return s.length() == 3 && s[1] == '-';
}

void printToken(std::string s) {
  if(!s.compare("[")) {
    std::cout << "LBRACK [" << std::endl;
  } else if (!s.compare("]")) {
    std::cout << "RBRACK ]" << std::endl;
  } else if (!s.compare(";")) {
    std::cout << "SEMI ;" << std::endl;
  } else if (!s.compare(",")) {
    std::cout << "COMMA ," << std::endl;
  } else if (!s.compare("%")) {
    std::cout << "PCT %" << std::endl;
  } else if (!s.compare("/")) {
    std::cout << "SLASH /" << std::endl;
  } else if (!s.compare("(")) {
    std::cout << "LPAREN (" << std::endl;
  } else if (!s.compare(")")) {
    std::cout << "RPAREN )" << std::endl;
  } else if (!s.compare("{")) {
    std::cout << "LBRACE {" << std::endl;
  } else if (!s.compare("}")) {
    std::cout << "RBRACE }" << std::endl;
  } else if (!s.compare("=")) {
    std::cout << "BECOMES =" << std::endl;
  } else if (!s.compare("==")) {
    std::cout << "EQ ==" << std::endl;
  } else if (!s.compare("!=")) {
    std::cout << "NE !=" << std::endl;
  }  else if (!s.compare("<")) {
    std::cout << "LT <" << std::endl;
  } else if (!s.compare(">")) {
    std::cout << "GT >" << std::endl;
  } else if (!s.compare("<=")) {
    std::cout << "LE <=" << std::endl;
  }else if (!s.compare(">=")) {
    std::cout << "GE >=" << std::endl;
  }else if (!s.compare("+")) {
    std::cout << "PLUS +" << std::endl;
  }else if (!s.compare("-")) {
    std::cout << "MINUS -" << std::endl;
  }else if (!s.compare("*")) {
    std::cout << "STAR *" << std::endl;
  }else if (!s.compare("<")) {
    std::cout << "LT <" << std::endl;
  }else if (!s.compare("&")) {
    std::cout << "AMP &" << std::endl;
  } else if (!s.compare("0")) {
    std::cout << "NUM 0" << std::endl;
  } else if (isdigit(s[0])) {
    //check num is in range
    long n = stol(s);
    if(n > 2147483647) {
      std::cerr << "ERROR : num is out of range" << std::endl;
      //exit(0);
    }
    std::cout << "NUM " << s << std::endl;
  } else if(!s.compare("return")) {
    std::cout << "RETURN return" << std::endl;
  } else if(!s.compare("if")) {
    std::cout << "IF if" << std::endl;
  } else if(!s.compare("else")) {
    std::cout << "ELSE else" << std::endl;
  } else if(!s.compare("while")) {
    std::cout << "WHILE while" << std::endl;
  }else if(!s.compare("println")) {
    std::cout << "PRINTLN println" << std::endl;
  }else if(!s.compare("wain")) {
    std::cout << "WAIN wain" << std::endl;
  }else if(!s.compare("int")) {
    std::cout << "INT int" << std::endl;
  }else if(!s.compare("delete")) {
    std::cout << "DELETE delete" << std::endl;
  }else if(!s.compare("new")) {
    std::cout << "NEW new" << std::endl;
  }else if(!s.compare("NULL")) {
    std::cout << "NULL NULL" << std::endl;
  } else if(!s.compare("//")) {} 
   else {//is id
    std::cout << "ID " << s << std::endl;
  }
}

// Locations in the program that you should modify to store the
// DFA information have been marked with four-slash comments:
//// (Four-slash comment)
int main() {
  std::istream& in = std::cin;
  std::string s;
  std::vector<char> alphabet;
  std::vector<std::string> states;
  std::vector<std::string> accepting_states;
  std::string initial_state;
  std::map<std::pair<std::string, char>, std::string> transitions;

  std::ifstream rfile;
  rfile.open("wlp4.dfa");
  if(rfile.is_open()) {
    std::getline(rfile, s); // Alphabet section (skip header)
    // Read characters or ranges separated by whitespace
    while(std::getline(rfile, s)) {
      if (s == STATES) { 
        break; 
      } else {
        if (isChar(s)) {
          //// Variable 's[0]' is an alphabet symbol
          alphabet.push_back(s[0]);
        } else if (isRange(s)) {
          for(char c = s[0]; c <= s[2]; ++c) {
            //// Variable 'c' is an alphabet symbol
            alphabet.push_back(c);
          }
        } 
      }
    }
    //debugging alphabet
    // std::cout << "ALPHABET: " << alphabet.size();
    // for (char c : alphabet) {
    //     std::cout << c << ", ";
    // }
    // std::cout << std::endl;

  //  std::getline(rfile, s); // States section (skip header)
    // Read states separated by whitespace
    while(std::getline(rfile, s)) {
      if (s == TRANSITIONS) { 
        break; 
      } else {
        static bool initial = true;
        bool accepting = false;
        if (s.back() == '!' && !isChar(s)) {
          accepting = true;
          s.pop_back();
        }
        //// Variable 's' contains the name of a state
        if (initial) {
          //// The state is initial
          initial = false;
          initial_state = s;
        }
        if (accepting) {
          //// The state is accepting
          accepting_states.push_back(s);
        } else {
              states.push_back(s); //non-accepting states
        }
      }
    }

    //debugging states
    // std::cout << "STATES: ";
    // for (std::string s : states) {
    //     std::cout << s << ", ";
    // }
    // std::cout << std::endl;
    // std::cout << "INITIAL STATE " << initial_state << std::endl;
    // std::cout << "ACCEPTING STATES: ";
    // for (std::string s : accepting_states) {
    //     std::cout << s << ", ";
    // }
    // std::cout << std::endl;

    //std::getline(rfile, s); // Transitions section (skip header)
    // Read transitions line-by-line
    while(std::getline(rfile, s)) {
      // std::cout << s << std::endl;
      if (s == INPUT) { 
        // Note: Since we're reading line by line, once we encounter the
        // input header, we will already be on the line after the header
        break; 
      } else {
        std::string fromState, toState;
        std::vector<char> symbols;
        std::istringstream line(s);
        line >> fromState;
        bool last;
        while(line >> s) {
          if(line.peek() == EOF) { // If we reached the last item on the line
            // Then it's the to-state
            toState = s;
            // std::cout << "set the toState to: " << toState << std::endl;
          } else { // Otherwise, there is more stuff on the line
            // We expect a character or range
            if (isChar(s)) {
              symbols.push_back(s[0]);
            } else if (isRange(s)) {
              for(char c = s[0]; c <= s[2]; ++c) {
                symbols.push_back(c);
              //   std::cout << "pushing back " << c << std::endl;
              }
            }
          }
        }
        for ( char c : symbols ) {
          //// There is a transition from 'fromState' to 'toState' on 'c'
          std::pair<std::string, char> map_item;
          map_item.first = fromState;
          map_item.second = c;
          transitions.emplace(map_item, toState);
          // std::cout << "Added (" << fromState << ", " << c << ") as a transition to " << toState << std::endl; 
        }
      }
    }
    
    //debugging transitions
    // std::cout << "TRANSITIONS: ";
    // for (auto it = transitions.begin(); it != transitions.end(); ++it) {
    //   std::cout << "(" << it->first.first << ", " <<  it->first.second << ") transitions to " << it->second << '\n';
    // }

    rfile.close();
  }


  // Input section (already skipped header)
  while(getline(in, s)) {
    bool commenting = false;
    // std::cout << s << std::endl;
    //s contains 1 string with no whitespace or new lines
    std::string current_state = initial_state; // keep track of current state
  //   //// Variable 's' contains an input string for the DFA else {
    bool broken = false;
    std::string toPrint = "";
    for (int i = 0; i < s.length(); i++) {
      // std::cout << current_state << toPrint << std::endl;
      if(!current_state.compare("comment")) {
        current_state = initial_state;
        break;
      } else if (current_state.compare("stopID") == 0) {
        // std::cout << "printing " << toPrint.substr(0, toPrint.length()-1) << std::endl;
        printToken(toPrint.substr(0, toPrint.length()-1));
        toPrint = "";
        i--;
        current_state = initial_state;
      } else if(isspace(s[i])) {
        // std::cout << "calling isspace block on " << toPrint << std::endl;
          if(toPrint.length() > 0) {
            printToken(toPrint);
          }
          toPrint = "";
          current_state = initial_state;
      } else if(std::find(alphabet.begin(), alphabet.end(), s[i]) == alphabet.end()) {
        std::cerr << "ERROR: invalid char parsed" << std::endl;
        // exit(0);
      }
      std::pair<std::string, char> current_transition;
      current_transition.first = current_state;
      current_transition.second = s[i];
      auto it = transitions.find(current_transition);
      if(it == transitions.end()) {
          // std::cout << "transition from " << current_state << " with " << s[i] << " doesnt exist" << std::endl;
          bool final_is_accepting = std::count(std::begin(accepting_states), std::end(accepting_states), current_state) > 0;
          if(final_is_accepting) {
              // std::cout << toPrint << std::endl;
              //toPrint = "";
              current_state = initial_state;
              //i--;
          } else {
              std::cerr << "ERROR - No transition and state is unaccepting" << std::endl;
               broken = true;
              break;
          }
      } else {
          //Transition exists
          current_state = it->second;
          // std::cout << "new state is" << current_state << std::endl;
          toPrint+=s[i];
          if(!current_state.compare("finishsymb")) {
            printToken(toPrint);
            toPrint = "";
            current_state = initial_state;
          } else if(!current_state.compare("comment")) {
            toPrint = "";
            current_state = initial_state;
            break;
          } else if(!current_state.compare("zero")) {
            toPrint = "";
            printToken("0");
            current_state = initial_state;
          } else if(!current_state.compare("finish")) {
            toPrint = "";
            printToken("!=");
            i++;
            current_state = initial_state;
          }  else if (current_state.compare("stopID") == 0) {
            // std::cout << "printing " << toPrint.substr(0, toPrint.length()-1) << std::endl;
            printToken(toPrint.substr(0, toPrint.length()-1));
            toPrint = "";
            i--;
            current_state = initial_state;
          }
      }
    }
    if(!broken) {
        bool final_is_accepting = std::count(std::begin(accepting_states), std::end(accepting_states), current_state) > 0;
        if(final_is_accepting) {
          if(toPrint != "") {
            printToken(toPrint);
          }
        } else {
            std::cerr << "ERROR - Final state " << current_state << " is unaccepting" << std::endl;
        }
    }
  }
}