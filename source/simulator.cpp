#include "simulator.h"

Simulator::Simulator(){

}

void Simulator::simulate(string fileName){
    _parser.parse(fileName);
}


int main(){
    Simulator simulator;
    simulator.simulate("../test/tstCir.sp");
}
