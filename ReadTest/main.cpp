#include "ParameterViewSyn.h"

int main(int argc, char ** argv) {

	ParameterViewSyn cParameter;
	cParameter.Init(argc, argv);
	cParameter.showParameter();

	system("pause");
	return 1;
}