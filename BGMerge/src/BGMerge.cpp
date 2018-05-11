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

#ifdef OUTPUT_COMPUTATIONAL_TIME
	clock_t start, finish, first;
	start = clock();
#endif // OUTPUT_COMPUTATIONAL_TIME

	ParameterViewSyn cParameter;

	if (cParameter.Init(argc, argv) != 1) {
		std::cout << "Initialize File Parameters Failure!" << std::endl;
		return 1;
	}

	CMerge cMerge;

	//>初始化Process基本参数
	cMerge.Init(cParameter);

	//>初始化CUsingDeeperDepth Class参数
	if (!cMerge.initUsingDeeperDepth()) {
		std::cout << "Initialize CUsingDeeperDepth Class Failure!" << std::endl;
		return 2;
	}

#ifdef OUTPUT_COMPUTATIONAL_TIME
	finish = clock();
	std::cout << std::setprecision(2) << "Initialization : " << (double)(finish - start) / CLOCKS_PER_SEC << 's' << std::endl << std::endl;
	first = start = clock();
#endif // OUTPUT_COMPUTATIONAL_TIME

	//>Start proceiing
	if (!cMerge.doMerge()) {
		std::cout << "Merge Processing Failure!" << std::endl;
		return 3;
	}

#ifdef OUTPUT_COMPUTATIONAL_TIME
	finish = clock();
	std::cout << std::setprecision(2) << "Processing Time : " << (double)(finish - start) / CLOCKS_PER_SEC << 's' << std::endl << std::endl;
	first = start = clock();
#endif // OUTPUT_COMPUTATIONAL_TIME

	cMerge.releaseUsingDeeperDepth();

	return 0;
}