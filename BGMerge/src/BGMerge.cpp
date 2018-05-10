#include <iostream>
#include <iomanip>
#include "version.h"
#include "ParameterViewSyn.h"
#include "Process.h"

int main(int argc, char ** argv) {

	std::cout.setf(std::ios::fixed);
	std::cout << "This is Background Merge Process" << std::endl;
	std::cout << "Author : Tom Rubin" << std::endl;
	std::cout << std::setprecision(1) << "Version : " << VERSION << std::endl << std::endl;

	ParameterViewSyn cParameter;

	if (cParameter.Init(argc, argv) != 1) {
		std::cout << "Initialize File Parameters Failure!" << std::endl;
		return 1;
	}

	return 0;
}