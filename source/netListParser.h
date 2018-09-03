#ifndef __PARSER_H__
#define __PARSER_H__
#include <string>
#include <iostream>
#include <fstream>
#include "Circuit.h"

class Parser{
public:
    Parser(std::string FileName, Circuit* cir);
    ~Parser(){};
    void Parse();
    void ParseLine(std::string line);
    void AddComponent(std::string& line);
    void addResistor(std::string* words);
    void addCapacitor(std::string w0,std::string w1,std::string w2,std::string w3);
    void addInductor(std::string w0,std::string w1,std::string w2,std::string w3);
    void AddVoltageSource(std::string w0,std::string w1,std::string w2,std::string w3);
    void addCurrentSource(std::string w0,std::string w1,std::string w2,std::string w3);
private:
    bool _debug = false;
    Circuit* _cir;
    std::string _fileName;

};

#endif
