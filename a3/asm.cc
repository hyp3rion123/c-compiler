#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <map>
#include "scanner.h"

void printTokens(const std::vector<Token> tokens, std::string instr) {
  if (instr == "add" || instr == "sub") {
    int lastPart = 32;
    if(instr == "sub") {
      lastPart = 34;
    }
    int instrEnc = (0<<26) | (stoi(tokens[1].getLexeme().substr(1))<<21) | (stoi(tokens[2].getLexeme().substr(1))<<16) | (stoi(tokens[0].getLexeme().substr(1))<<11) | lastPart;
    unsigned char c = instrEnc >> 24;
    std::cout << c;
    c = instrEnc >> 16;
    std::cout << c;
    c = instrEnc >> 8;
    std::cout << c;
    c = instrEnc;
    std::cout << c;
  } else if (instr == "mfhi" || instr == "mflo" || instr == "lis") {
    int lastPart = 16;
    if(instr == "mflo") {
      lastPart = 18;
    } else if(instr == "lis") {
      lastPart = 20;
    }
    int instrEnc = (0<<26) | (stoi(tokens[0].getLexeme().substr(1))<<11) | lastPart;
    unsigned char c = instrEnc >> 24;
    std::cout << c;
    c = instrEnc >> 16;
    std::cout << c;
    c = instrEnc >> 8;
    std::cout << c;
    c = instrEnc;
    std::cout << c;
  } else if (instr == "jr" || instr == "jalr") {
    int lastPart = 8;
    if(instr == "jalr") {
      lastPart = 9;
    }
    int instrEnc = (0<<26) | (stoi(tokens[0].getLexeme().substr(1))<<21) | lastPart;
    unsigned char c = instrEnc >> 24;
    std::cout << c;
    c = instrEnc >> 16;
    std::cout << c;
    c = instrEnc >> 8;
    std::cout << c;
    c = instrEnc;
    std::cout << c;
  } else if (instr == "div" || instr == "divu" || instr == "mult" || instr == "multu") {
    int lastPart = 27;
    if(instr == "mult") {
      lastPart = 24;
    } else if(instr == "multu") {
      lastPart = 25;
    } else if(instr == "div") {
      lastPart = 26;
    }
    int instrEnc = (0<<26) | (stoi(tokens[0].getLexeme().substr(1))<<21) | (stoi(tokens[1].getLexeme().substr(1))<<16) | lastPart;
    unsigned char c = instrEnc >> 24;
    std::cout << c;
    c = instrEnc >> 16;
    std::cout << c;
    c = instrEnc >> 8;
    std::cout << c;
    c = instrEnc;
    std::cout << c;
  } else if (instr == "slt" || instr == "sltu") {
    int lastPart;
    if (instr == "slt") {
      lastPart = 42;
    } else {
      lastPart = 43;
    }
    int instrEnc = (0<<26) | (stoi(tokens[1].getLexeme().substr(1))<<21) | (stoi(tokens[2].getLexeme().substr(1))<<16) | (stoi(tokens[0].getLexeme().substr(1))<<11) | lastPart;
    unsigned char c = instrEnc >> 24;
    std::cout << c;
    c = instrEnc >> 16;
    std::cout << c;
    c = instrEnc >> 8;
    std::cout << c;
    c = instrEnc;
    std::cout << c;
  } else if (instr == "beq" || instr == "bne") {
    int reg1 = stoi(tokens[0].getLexeme().substr(1));
    int reg2 = stoi(tokens[1].getLexeme().substr(1));
    int val3;
    if(tokens[2].getKind() == Token::HEXINT) {
      val3 = stoi(tokens[2].getLexeme(), nullptr, 16);
    } else { //isdecimal
      val3 = stoi(tokens[2].getLexeme());
    }
    //std::cout << reg1 << ", " << reg2 << ", " << val3 << std::endl;
    int firstPart;
    if (instr == "beq") {
      firstPart = 4;
    } else {
      firstPart = 5;
    }
    int instrEnc = (firstPart << 26) | (reg1 << 21) | (reg2 << 16) | (val3 & 0xFFFF);
    unsigned char c = instrEnc >> 24;
    std::cout << c;
    c = instrEnc >> 16;
    std::cout << c;
    c = instrEnc >> 8;
    std::cout << c;
    c = instrEnc;
    std::cout << c;
    // std::cout << "made it to beq" << std::endl;
  } else if (instr == "lw" || instr == "sw") {
    int reg1 = stoi(tokens[2].getLexeme().substr(1)); // s
    int reg2 = stoi(tokens[0].getLexeme().substr(1)); // t
    int val3;
    if(tokens[1].getKind() == Token::HEXINT) {
      val3 = stoi(tokens[1].getLexeme(), nullptr, 16);
    } else { //isdecimal
      val3 = stoi(tokens[1].getLexeme());
    }
    //std::cout << reg1 << ", " << reg2 << ", " << val3 << std::endl;
    int firstPart;
    if (instr == "lw") {
      firstPart = 35;
    } else {
      firstPart = 43;
    }
    int instrEnc = (firstPart << 26) | (reg1 << 21) | (reg2 << 16) | (val3 & 0xFFFF);
    unsigned char c = instrEnc >> 24;
    std::cout << c;
    c = instrEnc >> 16;
    std::cout << c;
    c = instrEnc >> 8;
    std::cout << c;
    c = instrEnc;
    std::cout << c;
    // std::cout << "made it to beq" << std::endl;
  } else if (instr == "word") {
    int instr = (0 << 26) | tokens[0].toNumber();
    unsigned char c = instr >> 24;
    std::cout << c;
    c = instr >> 16;
    std::cout << c;
    c = instr >> 8;
    std::cout << c;
    c = instr;
    std::cout << c;  
  }
}

void printTokenLineBin(const std::vector<Token> tokens, std::string line, const std::map<std::string, int> labelAds, const std::vector<int> lineToAddr, const int programCounter ) {
  if (tokens.size() > 0) { //otherwise we are reading comments or whitespace which we don't care about
    bool done = false;
    bool checkingWordValue = false;
    bool checkingAddValues = false;
    bool checkingSubValues = false;
    bool checkingSltValues = false;
    bool checkingSltuValues = false;
    bool checkingMultValues = false;
    bool checkingMultuValues = false;
    bool checkingDivValues = false;
    bool checkingDivuValues = false;
    bool checkingBneValues = false;
    bool checkingBeqValues = false;
    bool checkingMfhiValues = false;
    bool checkingMfloValues = false;
    bool checkingLisValues = false;
    bool checkingLwValues = false;
    bool checkingSwValues = false;
    bool checkingJrValues = false;
    bool checkingJalrValues = false;
    bool prevComma = false; //error checking for double commas
    std::vector<Token> registers;
   //std::cout << "calling print on line: " << line << "with # of tokens = " << tokens.size() << std::endl;
    bool labelOnThisLine = false; //used to prevent errors resulting from double labels or label followed by instruction
    for (auto &token : tokens) {
      Token::Kind tokenKind = token.getKind();
      // std::cout << "reading token" << token << std::endl;
    //  std::cout << "current value of done: " << done << std::endl;
      if (tokenKind == Token::WHITESPACE || tokenKind == Token::COMMENT) {
      //  std::cout << "this token is a cmment or whitespace, setting done to true..." << std::endl;
        done = true;
        break;
      // } else if (done && tokenKind != Token::LABEL) {//extra arguments leads to error
      //   std::cerr << "ERROR: Parse error in line: " << line << std::endl; 
      //   std::cerr << "Expecting end of line, but there's more stuff." << std::endl;
      // } else if(tokenKind == Token::COMMA) {
          if(prevComma) {
            std::cerr << "ERROR: Parse error in line: " << line << std::endl; 
            std::cerr << "Parse error. Cannot have two commas in a row." << std::endl; 
            return;
          }
          prevComma = true;
      } 
      
      else if (tokenKind == Token::WORD) {
        //std::cout << token << ' ';
        checkingWordValue = true;
        prevComma = false;
      } else if(tokenKind == Token::ID && token.getLexeme() == "add") {
        checkingAddValues = true;
        prevComma = false;
      } else if(tokenKind == Token::ID && token.getLexeme() == "sub") {
        checkingSubValues = true;
        prevComma = false;
      } else if(tokenKind == Token::ID && token.getLexeme() == "slt") {
        checkingSltValues = true;
        prevComma = false;
      } else if(tokenKind == Token::ID && token.getLexeme() == "sltu") {
        checkingSltuValues = true;
        prevComma = false;
      } else if(tokenKind == Token::ID && token.getLexeme() == "mult") {
        checkingMultValues = true;
        prevComma = false;
      } else if(tokenKind == Token::ID && token.getLexeme() == "multu") {
        checkingMultuValues = true;
        prevComma = false;
      } else if(tokenKind == Token::ID && token.getLexeme() == "div") {
        checkingDivValues = true;
        prevComma = false;
      } else if(tokenKind == Token::ID && token.getLexeme() == "divu") {
        checkingDivuValues = true;
        prevComma = false;
      } else if(tokenKind == Token::ID && token.getLexeme() == "mfhi") {
        checkingMfhiValues = true;
        prevComma = false;
      } else if(tokenKind == Token::ID && token.getLexeme() == "mflo") {
        checkingMfloValues = true;
        prevComma = false;
      } else if(tokenKind == Token::ID && token.getLexeme() == "lis") {
        checkingLisValues = true;
        prevComma = false;
      } else if(tokenKind == Token::ID && token.getLexeme() == "lw") {
        checkingLwValues = true;
        prevComma = false;
      } else if(tokenKind == Token::ID && token.getLexeme() == "sw") {
        checkingSwValues = true;
        prevComma = false;
      } else if(tokenKind == Token::ID && token.getLexeme() == "jr") {
        checkingJrValues = true;
        prevComma = false;
      } else if(tokenKind == Token::ID && token.getLexeme() == "jalr") {
        checkingJalrValues = true;
        prevComma = false;
      } else if(tokenKind == Token::ID && token.getLexeme() == "beq") {
        checkingBeqValues = true;
        prevComma = false;
      } else if(tokenKind == Token::ID && token.getLexeme() == "bne") {
        checkingBneValues = true;
        prevComma = false;
      } //else if(tokenKind == Token::REG && prevComma == false && registers.size() != 0) {
      //   std::cerr << "ERROR: Parse error in line: " << line << std::endl;
      //   std::cerr << "asm.ParseError: Missing comma" << std::endl;
       //} 
       else if(tokenKind == Token::REG && (checkingAddValues || checkingSubValues || checkingMfhiValues || checkingMfloValues || checkingLisValues || checkingJalrValues || checkingJrValues)) {
        registers.push_back(token);
        if((registers.size() == 3 && (checkingAddValues || checkingSubValues)) || checkingMfhiValues || checkingMfloValues || checkingLisValues || checkingJalrValues || checkingJrValues) {
          //error check registers - make sure they are all in range 1-31
          // for (auto &reg : registers) {
          //   if(reg.getLexeme().length() > 7) {
          //     std::cerr << "ERROR: Parse error in line: " << line << std::endl;
          //     std::cerr << "asm.ParseError: Constant out of range: " << std::endl;
          //     return;
          //   }
          //   long numValue = stol(reg.getLexeme().substr(1));
          //   if(numValue < 0 || numValue > 31) {
          //     std::cerr << "ERROR: Parse error in line: " << line << std::endl;
          //     std::cerr << "asm.ParseError: Constant out of range: " << numValue << std::endl;
          //     return;
          //   }
          // }
          if(checkingAddValues) {
            printTokens(registers, "add");
            checkingAddValues = false;
          } else if(checkingSubValues) {
            printTokens(registers, "sub");
            checkingSubValues = false;
          } else if(checkingMfhiValues) {
            printTokens(registers, "mfhi");
            checkingMfhiValues = false;
          } else if(checkingMfloValues) {
            printTokens(registers, "mflo");
            checkingMfloValues = false;
          } else if(checkingLisValues) {
            printTokens(registers, "lis");
            checkingLisValues = false;
          } else if(checkingJalrValues) {
            printTokens(registers, "jalr");
            checkingJalrValues = false;
          } else if(checkingJrValues) {
            printTokens(registers, "jr");
            checkingJrValues = false;
          }
          done = true;
        }
        prevComma = false;
      } else if(tokenKind == Token::REG && (checkingSltValues || checkingSltuValues)) {
        registers.push_back(token);
        if(registers.size() == 3) {
          //error check registers - make sure they are all in range 1-31
          for (auto &reg : registers) {
            if(reg.getLexeme().length() > 7) {
              std::cerr << "ERROR: Parse error in line: " << line << std::endl;
              std::cerr << "asm.ParseError: Constant out of range: " << std::endl;
              return;
            }
            long numValue = stol(reg.getLexeme().substr(1));
            if(numValue < 0 || numValue > 31) {
              std::cerr << "ERROR: Parse error in line: " << line << std::endl;
              std::cerr << "asm.ParseError: Constant out of range: " << numValue << std::endl;
              return;
            }
          }
          if(checkingSltValues) {
            printTokens(registers, "slt");
            checkingSltValues = false;
          } else {
            printTokens(registers, "sltu");
            checkingSltuValues = false;
          }
          done = true;
        }
        prevComma = false;
      }  else if(tokenKind == Token::REG && (checkingMultValues || checkingMultuValues || checkingDivValues || checkingDivuValues)) {
        registers.push_back(token);
        if(registers.size() == 2) {
          //error check registers - make sure they are all in range 1-31
          for (auto &reg : registers) {
            if(reg.getLexeme().length() > 7) {
              std::cerr << "ERROR: Parse error in line: " << line << std::endl;
              std::cerr << "asm.ParseError: Constant out of range: " << std::endl;
              return;
            }
            long numValue = stol(reg.getLexeme().substr(1));
            if(numValue < 0 || numValue > 31) {
              std::cerr << "ERROR: Parse error in line: " << line << std::endl;
              std::cerr << "asm.ParseError: Constant out of range: " << numValue << std::endl;
              return;
            }
          }
          if(checkingMultValues) {
            printTokens(registers, "mult");
            checkingMultValues = false;
          } else if(checkingMultuValues) {
            printTokens(registers, "multu");
            checkingMultuValues = false;
          } else if(checkingDivValues) {
            printTokens(registers, "div");
            checkingDivValues = false;
          } else if(checkingDivuValues) {
            printTokens(registers, "divu");
            checkingDivuValues = false;
          }
          done = true;
        }
        prevComma = false;
      } else if(tokenKind == Token::REG && (checkingBeqValues || checkingBneValues)) {
          registers.push_back(token);
          prevComma = false;
      } else if(tokenKind == Token::RPAREN && (checkingLwValues || checkingSwValues)) {
        done = true;
      }
        else if((tokenKind == Token::INT || tokenKind == Token::HEXINT || tokenKind == Token::REG) && (checkingLwValues || checkingSwValues)) {
        registers.push_back(token);
        if(registers.size() == 3) {
          if(checkingLwValues) {
            printTokens(registers, "lw");
            checkingLwValues = false;
          } else {
            printTokens(registers, "sw");
            checkingSwValues = false;
          }
        }
        prevComma = false;
      }
        else if((tokenKind == Token::INT || tokenKind == Token::HEXINT) && (checkingBeqValues || checkingBneValues)) {
          if(registers.size() == 2) {
            //error check registers - make sure they are all in range 1-31
            for (auto &reg : registers) {
              if(reg.getLexeme().length() > 7) {
                  std::cerr << "ERROR: Parse error in line: " << line << std::endl;
                  std::cerr << "asm.ParseError: Constant out of range: " << std::endl;
                  return;
                }
              long numValue = stol(reg.getLexeme().substr(1));
              if(numValue < 0 || numValue > 31) {
                std::cerr << "ERROR: Parse error in line: " << line << std::endl;
                std::cerr << "asm.ParseError: Constant out of range: " << numValue << std::endl;
                return;
              }
            }
            if(tokenKind == Token::INT) {
              if(token.getLexeme().length() > 7) {
                  std::cerr << "ERROR: Parse error in line: " << line << std::endl;
                  std::cerr << "asm.ParseError: Constant out of range: " << std::endl;
                  return;
                }
              long numValue = stol(token.getLexeme());
                if (numValue < -32768 || numValue > 32767) {
                std::cerr << "ERROR: Parse error in line: " << line << std::endl;
                std::cerr << "asm.ParseError: Constant out of range: " << numValue << std::endl;
                return;
              }
            } else if (tokenKind == Token::HEXINT) { //convert to decimal to compute error
                if(token.getLexeme().length() > 8) {
                  std::cerr << "ERROR: Parse error in line: " << line << std::endl;
                  std::cerr << "asm.ParseError: Constant out of range: " << std::endl;
                  return;
                }
                long numValue = stol(token.getLexeme(), nullptr, 16);
                if (numValue > 0xffff) {
                  std::cerr << "ERROR: Parse error in line: " << line << std::endl;
                  std::cerr << "asm.ParseError: Constant out of range: " << numValue << std::endl;
                  return;
                }
            }
            registers.push_back(token);
            if(checkingBneValues) {
              printTokens(registers, "bne");
              checkingBneValues = false;
            } else {
              printTokens(registers, "beq");
              checkingBeqValues = false;
            }            
            done = true;
           }// else {
          //   //dont have two registers - invalid format return error
          //   std::cerr << "ERROR: Parse error in line: " << line << std::endl;
          //   std::cerr << "asm.ParseError: Expecting register but got: " << token << std::endl;
          //   return;
          // }
          prevComma = false;
      } else if(tokenKind == Token::ID && (checkingBeqValues || checkingBneValues)) {
          //logic for label reference in beq 
          if(registers.size() == 2) {
            //error check registers - make sure they are all in range 1-31
            for (auto &reg : registers) {
              if(reg.getLexeme().length() > 7) {
                std::cerr << "ERROR: Parse error in line: " << line << std::endl;
                std::cerr << "asm.ParseError: Constant out of range: " << std::endl;
                return;
              }
              long numValue = stol(reg.getLexeme().substr(1));
              if(numValue < 0 || numValue > 31) {
                std::cerr << "ERROR: Parse error in line: " << line << std::endl;
                std::cerr << "asm.ParseError: Constant out of range: " << numValue << std::endl;
                return;
              }
            }
            std::string searchString = token.getLexeme() + ":";
            auto it = labelAds.find(searchString);
            if(it == labelAds.end()){//label not found, throw error
              std::cerr << "ERROR: Parse error in line: " << line << std::endl; 
              std::cerr << "Label used but not declared: " << token.getLexeme() << std::endl;
            }
            int labelAddress = it->second;
            //std::cout << "found label " << it->first << " at address " << labelAddress << std::endl;
          // std::cout << "current PC address is " << lineToAddr[programCounter+1] << std::endl;
            int offset = labelAddress/4 - lineToAddr[programCounter+1]/4; //gets the address difference between where label is declared and where it is used
            //std::cout << "COMPUTED LABEL OFFSET IS: " << offset << std::endl;
            //std::cout << "handling label logic" << std::endl;
            int reg1 = stoi(registers[0].getLexeme().substr(1));
            int reg2 = stoi(registers[1].getLexeme().substr(1));
            //std::cout << reg1 << ", " << reg2 << ", " << offset << std::endl;
            int firstPart = 4;
            if (checkingBneValues) {
              firstPart = 5;
            }
            int instrEnc = (firstPart << 26) | (reg1 << 21) | (reg2 << 16) | (offset & 0xFFFF);
            unsigned char c = instrEnc >> 24;
            std::cout << c;
            c = instrEnc >> 16;
            std::cout << c;
            c = instrEnc >> 8;
            std::cout << c;
            c = instrEnc;
            std::cout << c;
            if(checkingBneValues) {
              checkingBneValues = false;
            } else if(checkingBeqValues) {
              checkingBeqValues = false;
            }
            done = true;
          }
        prevComma = false;
      }
        else if (checkingWordValue) {
          if(tokenKind == Token::INT || tokenKind == Token::HEXINT) { //token is an int or hexint
            //printWord(token);
            //ERROR CHECKING: RANGE OF WORD ie -2^32...
            if (token.toNumber() > 4294967295 || token.toNumber() < -2147483648) {
              std::cerr << "ERROR: Parse error in line: " << line << std::endl;
              std::cerr << "asm.ParseError: Constant out of range: " << token.toNumber() << std::endl;
            }
            std::vector<Token> singleToken;
            singleToken.push_back(token);
            printTokens(singleToken, "word");
          } else if(tokenKind == Token::ID) { //token is a label
            //search for label in stack
            //throw error if label not found
            std::string searchString = token.getLexeme() + ":";
            auto it = labelAds.find(searchString);
            if(it == labelAds.end()){//label not found, throw error
              std::cerr << "ERROR: Parse error in line: " << line << std::endl; 
              std::cerr << "Label used but not declared: " << token.getLexeme() << std::endl;
            }
            int labelAddress = it->second;
        //    std::cout << "found label " << it->first << " at address " << labelAddress << std::endl;
            int instr = (0 << 26) | labelAddress;
            unsigned char c = instr >> 24;
            std::cout << c;
            c = instr >> 16;
            std::cout << c;
            c = instr >> 8;
            std::cout << c;
            c = instr;
            std::cout << c; 
          } else { //token is invalid type - throw error
            std::cerr << "ERROR: Parse error in line: " << line << std::endl; 
            std::cerr << "Invalid argument for .word " << token.getLexeme() << std::endl;
          }
        checkingWordValue = false;
        prevComma = false;
        // std::cout << "setting done to true" << std::endl;
        done = true;
      } else if(tokenKind == Token::LABEL){
     //   std::cout << "this is a label line" << token.getKind();
        labelOnThisLine = true;
      } else if(tokenKind == Token::LPAREN) {

      }
      //   else {
      //   std::cout << "unknown kind" << tokenKind;
      // }
    }
    // if (!done && !labelOnThisLine) {//extra arguments leads to error
    //   std::cerr << "ERROR: Parse error in line: " << line << std::endl; 
    //   std::cerr << "Expecting end of line, but there's more stuff." << std::endl;
    // }
  }
}

int parseLabels(std::string line, std::map<std::string, int> &labelAds, int &progCounter, std::vector<int> &lineToAddr, bool &incremented) {
  bool pcIncremented = false; //only increment at most once per line
  bool seenNotLabels = false; //ensures labels are the first things on the line we see
  std::map<char, int>::iterator it; //map iterator
  std::vector<Token> tokenLine = scan(line);
  lineToAddr.push_back(progCounter);
  for(auto &token : tokenLine) {
   //   std::cout << token << std::endl;
      if(token.getKind() == Token::LABEL) {
        auto it = labelAds.find(token.getLexeme());
        if(!seenNotLabels) {//label is before anything else(other than maybe another label) and isnt duplicate
         if(it == labelAds.end()) {
            std::string tokenString = token.getLexeme();
            labelAds.insert(std::pair<std::string, int>(tokenString, progCounter));
          } else {//label is a duplicate
            std::cerr << "ERROR: Parse error in line: " << line << std::endl; 
            std::cerr << "Label definition already exists." << std::endl;
            return 0;
          }
        } else {//seen other stuff before this label - throw error
          std::cerr << "ERROR: Parse error in line: " << line << std::endl; 
          std::cerr << "Label appeared following another instruction on the same line." << token.getLexeme() << std::endl;
          return 0;
        }
      } else if(token.getKind() == Token::ID || token.getKind() == Token::WORD) {
          if(!pcIncremented) {
            progCounter+=4;
            incremented = true;
          }
          pcIncremented = true;
          seenNotLabels = true;
      }
  }
  // if(lineToAddr.size() == 0) {
  //   lineToAddr.push_back(0);
  // }
  // else if(pcIncremented) {
  //     lineToAddr.push_back(lineToAddr[lineToAddr.size()-1]+1); // current line gets asigned an address - can be duplicate of previous if just comment/labels
  // } else {
  //   lineToAddr.push_back(lineToAddr[lineToAddr.size()-1]); 
  // }
  return 1;
}
/*
 * C++ Starter code for CS241 A3
 * All code requires C++14, so if you're getting compile errors make sure to
 * use -std=c++14.
 *
 * This file contains the main function of your program. By default, it just
 * prints the scanned list of tokens back to standard output.
 */
int main() {
  std::string line;
  try {
    std::vector<std::string> lines;
    std::map<std::string, int> labelAds;
    int programCounter = 0;
    std::vector<int> lineToAddr; //this keeps track of every line address which will be used for beq offset calculations
    bool incremented = false;
    //std::cout << "ROUND1" << std::endl;
    while (getline(std::cin, line)) {
      lines.push_back(line);
      int success = parseLabels(line, labelAds, programCounter, lineToAddr, incremented);
      if(!success) {
        return 0;
      }
    }
    if(incremented) {
      lineToAddr.push_back(lineToAddr[lineToAddr.size()-1]+4);
    } else {
      lineToAddr.push_back(lineToAddr[lineToAddr.size()-1]);
    }

    
    // add another element at the end with the same address as the last element so that we dont get array out of bounds error
    //for a case where .word label is the last line and the PC is one over

    //print debugging
    // std::cout << "program counter: " << programCounter << std::endl;
    // std::cout << "LABELS DEBUGGING: " << std::endl;
    // for(auto it = labelAds.cbegin(); it != labelAds.cend(); ++it)
    // {
    //     auto first = it->first;
    //     auto second = it->second;
    //     std::cout << first << " " << second << "\n";
    // }
    // std::cout << "LINE NUMBER DEBUGGING: " << std::endl;
    // for (long unsigned int i = 0; i < lineToAddr.size(); i++) {
    //   if(i == lineToAddr.size() - 1) {
    //     std::cout << "this is a fake line to be used to prevent array ot of bounds" << std::endl;
    //   }
    //   std::cout << "line " << i << "has address: " << lineToAddr[i] << std::endl;
    // }
    //end print debugging
   //std::cout << "ROUND2" << std::endl;


    programCounter = 0;
    for (auto &line : lines) {
      std::vector<Token> tokenLine = scan(line);
      //Prints binary representation of token line
      printTokenLineBin(tokenLine, line, labelAds, lineToAddr, programCounter);
      programCounter++;
    }

    // for (int i = 5; i >= 0; i--) {
    //   std::string test = "line" + i;
    //   test = test + ":";
    //   auto it = labelAds.find(test);
    //   if(it == labelAds.end()){//label not found, throw error
    //     std::cout << test << "not found"<< std::endl;
    //   }else {
    //     std::cout << "found"<< std::endl;
    //   }
    // }

    // for(int i = 0; i < 20000; i++) {
    //   std::cout << "line" << i << ": .word line" << i << std::endl;
    // }
  } catch (ScanningFailure &f) {
    std::cerr << f.what() << std::endl;
    return 1;
  }
  // You can add your own catch clause(s) for other kinds of errors.
  // Throwing exceptions and catching them is the recommended way to
  // handle errors and terminate the program cleanly in C++. Do not
  // use the std::exit function, which may leak memory.
  
  return 0;
}

