#include <iostream>
#include <iomanip>
#include <ctime>
#include "version.h"
#include "ParameterViewSyn.h"
#include "BackgroundEstimation.h"

int main(int argc, char ** argv)
{
	std::cout.setf(std::ios::fixed);
	std::cout << "This is DepthBackground Process" << std::endl;
	std::cout << std::setprecision(1) << "Version : " << VERSION << std::endl << std::endl;

#ifdef OUTPUT_COMPUTATIONAL_TIME
	clock_t start, finish, first;
	first = start = clock();
#endif // OUTPUT_COMPUTATIONAL_TIME

	ParameterViewSyn cParameter;
	CBackgroundEstimation cBackgroundEstimation;
	FILE *fin;

	if (cParameter.Init(argc, argv) != 1) return 0;
	std::cout << std::endl;

	if (!cBackgroundEstimation.Init(cParameter)) return 2;

	if (fopen_s(&fin, cParameter.getVideoName().c_str(), "rb")) {
		std::cout << "Can't Open Input Video File(s)" << std::endl;
		return 3;
	}

#ifdef OUTPUT_COMPUTATIONAL_TIME
	finish = clock();
	std::cout << std::setprecision(4) << "Initialization : " << (double)(finish - start) / CLOCKS_PER_SEC << 's' << std::endl;
#endif // OUTPUT_COMPUTATIONAL_TIME

	//>将深度图信息读入数组
	for (unsigned int n = cParameter.getStartFrame(); n < cParameter.getStartFrame() + cParameter.getTotalFrame(); n++) {
		std::cout << "Frame number = " << n << std::endl;

		if (!cBackgroundEstimation.getFrameBuffer()->ReadOneFrame(fin, n)) {
			std::cout << "Set Frame Head Failure!" << std::endl;
			break;
		}
		std::cout << "Set Frame Head Success!" << std::endl;

		//>中间过程的保存及显示
		/*cBackgroundEstimation.getFrameBuffer()->getData_inBGR(cBackgroundEstimation.getBgrBuffer());

		if (!cBackgroundEstimation.writeCurrentImage()) {
			std::cout << "Write Current Image Failure!" << std::endl;
			break;
		}
		std::cout << "Write Current Image Success!" << std::endl;

		cBackgroundEstimation.showCurrentImage();*/

		cBackgroundEstimation.writeOneDepthMap(cBackgroundEstimation.getFrameBuffer(), n);

		//>检测(x,y)=(256,144)宏块的深度值保存
		cBackgroundEstimation.showDepthMapTestMemory(144, 256, 15, n);

		system("pause");
	}

	return 1;
}