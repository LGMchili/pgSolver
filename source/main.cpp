#include "netListParser.h"
void printMatrix(Eigen::MatrixXd){

}

int main(){
    Circuit cir;
    std::string file;

    if(getenv("DEBUG"))
        file = "3x3.net";
    else{
        std::cout << "Input File:" << std::endl;
        std::cin >> file;
    }
    Parser netListParser(file, &cir);
    netListParser.Parse();
    cir.solve();
    cir.printNodalVoltage();
}
