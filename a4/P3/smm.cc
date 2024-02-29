#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <utility>
#include <vector>
#include <algorithm>
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

  std::getline(in, s); // Alphabet section (skip header)
  // Read characters or ranges separated by whitespace
  while(in >> s) {
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
//   std::cout << "ALPHABET: ";
//   for (char c : alphabet) {
//       std::cout << c << ", ";
//   }
//   std::cout << std::endl;

  std::getline(in, s); // States section (skip header)
  // Read states separated by whitespace
  while(in >> s) {
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
//   std::cout << "STATES: ";
//   for (std::string s : states) {
//       std::cout << s << ", ";
//   }
//   std::cout << std::endl;
//   std::cout << "INITIAL STATE " << initial_state << std::endl;
//   std::cout << "ACCEPTING STATES: ";
//   for (std::string s : accepting_states) {
//       std::cout << s << ", ";
//   }
//   std::cout << std::endl;

  std::getline(in, s); // Transitions section (skip header)
  // Read transitions line-by-line
  while(std::getline(in, s)) {
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
//   std::cout << "TRANSITIONS: ";
//   for (auto it = transitions.begin(); it != transitions.end(); ++it) {
//     std::cout << "(" << it->first.first << ", " <<  it->first.second << ") transitions to " << it->second << '\n';
//   }


  // Input section (already skipped header)
  while(in >> s) {
    //s contains 1 string with no whitespace or new lines
    std::string current_state = initial_state; // keep track of current state
    //// Variable 's' contains an input string for the DFA else {
    bool broken = false;
    std::string toPrint = "";
    for (int i = 0; i < s.length(); i++) {
        std::pair<std::string, char> current_transition;
        current_transition.first = current_state;
        current_transition.second = s[i];
        auto it = transitions.find(current_transition);
        if(it == transitions.end()) {
            // std::cout << "transition from " << current_state << " with " << s[i] << " doesnt exist" << std::endl;
            bool final_is_accepting = std::count(std::begin(accepting_states), std::end(accepting_states), current_state) > 0;
            if(final_is_accepting) {
                std::cout << toPrint << std::endl;
                toPrint = "";
                current_state = initial_state;
                i--;
            } else {
                std::cerr << "ERROR - No transition and state is unaccepting" << std::endl;
                broken = true;
                break;
            }
        } else {
            //Transition exists
            current_state = it->second;
            toPrint+=s[i];
        }
    }
    if(!broken) {
        bool final_is_accepting = std::count(std::begin(accepting_states), std::end(accepting_states), current_state) > 0;
        if(final_is_accepting) {
            std::cout << toPrint << std::endl;
        } else {
            std::cerr << "ERROR - Final state is unaccepting" << std::endl;
        }
    }
  }
}