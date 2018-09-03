#include "netListParser.h"

Parser::Parser(std::string FileName, Circuit* cir){
    // std::ifstream in(f);
    _cir = cir;
    _fileName = FileName;
    if(getenv("DEBUG"))
        _debug = true;
};

void Parser::Parse(){
    std::ifstream file;
    std::string sLine = "";
    if(_debug)
        std::cout <<  "netlist: "
        << _fileName
        << std::endl;
    file.open(_fileName);
    while (!file.eof()) {
        getline(file, sLine);
        ParseLine(sLine);
    }
};

void Parser::ParseLine(std::string line){
    if(line.empty() || *(line.begin()) == '*')
        return;
    std::string component = "RrLlCcVvIi";
    if(component.find(line.front()) != std::string::npos){
        AddComponent(line);
    }

    //TODO: add pwl waveform
};

// add a RLC or Source component from netlist
void Parser::AddComponent(std::string& line){
    size_t last = 0;
    size_t next = 0;
    int index = 0;
    std::string words[4] = {};
    std::string delimiter = " "; // split the line by blank
    if(getenv("DEBUG"))
        std::cout<<line<<std::endl;
    while ((next = line.find(delimiter, last)) != std::string::npos) {
        // std::cout << line.substr(last, next-last) << std::endl;
        words[index] = line.substr(last, next - last);
        last = next + 1;
        index++;
    }
    // std::cout << line.substr(last) << std::endl;
    words[index] = line.substr(last); // each element in words represents componentX node1 node2 value
    std::string resistorType = "Rr";
    std::string currentType = "Ii";
    if (resistorType.find(words[0][0]) != std::string::npos)
        _cir->addResistor(words);
    else if(currentType.find(words[0][0]) != std::string::npos)
        _cir->addCurrentSource(words);
    // else if(words[0][0] == 'C')
    //     _cir.addCapacitor(words[0],words[1],words[2],words[3]);
    // else if(words[0][0] == 'L')
    //     _cir.addInductor(words[0],words[1],words[2],words[3]);
    // else if(words[0][0] == 'V')
    //     _cir.AddVoltageSource(words[0],words[1],words[2],words[3]);
    // else if(words[0][0] == 'i')
    //     _cir.addCurrentSource(words[0],words[1],words[2],words[3]);
};
