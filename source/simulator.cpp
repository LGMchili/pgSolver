#include "simulator.h"

Simulator::Simulator(string fileName){
    _parser = new Parser();
    _parser->parse(fileName);
    resNum = (_parser->getResistors())->size();
    indNum = (_parser->getInductors())->size();
    capNum = (_parser->getCapacitors())->size();
    currNum = (_parser->getCurrentSource())->size();
    volNum = (_parser->getVoltageSource())->size();
    nodeNode = (_parser->getNodeMap())->size();
    // _resistors = _parser->getResistors();
    // _inductors = _parser->getInductors();
    // _capacitors = _parser->getCapacitors();
    // _currentSource = _parser->getCurrentSource();
    // _voltageSource = _parser->getVoltageSource();
    // _nodeMap = _parser->getNodeMap();
}

Simulator::~Simulator(){
    delete _parser;
}

void Simulator::simulate(){

}

void Simulator::buildMatrixG(){

}

int main(){
    Simulator simulator("../test/tstCir.sp");
    clock_t mstart = clock();
    // simulator.simulate();
    clock_t mend = clock();
    cout << "total time: " << (mend - mstart) / (double) CLOCKS_PER_SEC << "s" << endl;
}
