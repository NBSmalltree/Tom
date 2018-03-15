#include <iostream>
#include <iomanip>
#include <ctime>
#include "version.h"
#include "ParameterViewSyn.h"
#include "BackgroundEstimation.h"
#include "PixelHist.h"

#define SAVE_CERTAIN_PIXEL_HIST_IMAGE 1

int main(int argc, char ** argv)
{
	std::cout.setf(std::ios::fixed);
	std::cout << "This is DepthBackground Process" << std::endl;
	std::cout << "Author : Tom Rubin" << std::endl;
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
	start = clock();
#endif // OUTPUT_COMPUTATIONAL_TIME

	//>将深度图信息读入数组
	for (int n = cParameter.getStartFrame(); n < cParameter.getStartFrame() + cParameter.getTotalFrame(); n++) {
		std::cout << "Frame number = " << n << std::endl;
		std::cout << "Start Processing:";

		if (!cBackgroundEstimation.getFrameBuffer()->ReadOneFrame(fin, n)) {
			std::cout << "Set Frame Head Failure!" << std::endl;
			break;
		}
		std::cout << '.';

		//>【Test】:中间过程的保存及显示
		/*cBackgroundEstimation.getFrameBuffer()->getData_inBGR(cBackgroundEstimation.getBgrBuffer());

		if (!cBackgroundEstimation.writeCurrentImage()) {
			std::cout << "Write Current Image Failure!" << std::endl;
			break;
		}
		std::cout << "Write Current Image Success!" << std::endl;

		cBackgroundEstimation.showCurrentImage();*/

		//>将当前帧亮度信息保存到深度图数据结构中
		if (!cBackgroundEstimation.writeOneDepthMap(cBackgroundEstimation.getFrameBuffer(), n)) {
			std::cout << "Current Frame Depth Map Save Failure!" << std::endl;
			break;
		}
		std::cout << '.';

		//>【Test】:(x,y)=(256,144)宏块的深度值保存
		//cBackgroundEstimation.showDepthMapTestMemory(144, 256, 15, n);

		//>【Test】:写入一个特定的像素值到文件
		//cBackgroundEstimation.writeOnePixeltoFile(304, 160, n);

		std::cout << std::endl;
		//system("pause");
	}//>完成深度图信息保存

	std::cout << std::endl;
#ifdef OUTPUT_COMPUTATIONAL_TIME
	finish = clock();
	std::cout << std::setprecision(4) << "Store All The Depth Map Duration : " << (double)(finish - start) / CLOCKS_PER_SEC << 's' << std::endl;
	start = clock();
#endif // OUTPUT_COMPUTATIONAL_TIME

#if SAVE_CERTAIN_PIXEL_HIST_IMAGE
	int pixel_x = 304;
	int pixel_y = 160;

	//>提取特定像素点的深度，以更新步长为长度，最后一个参数为起始更新帧
	cBackgroundEstimation.extractCertainPixelDepthMap(pixel_x, pixel_y, 0);

	//>进入直方图统计模块(size,scale,updatestep)
	CPixelHist cPixelHist(256, 1, 100);

	//>测试直方图模块的输入数据
	//cPixelHist.printInputData(cBackgroundEstimation.getCertainPixelDepthMapBuffer());

	//>计算当前深度像素点的直方图
	cPixelHist.calcHist(cBackgroundEstimation.getCertainPixelDepthMapBuffer());

	//>测试最大值最小值
	cPixelHist.findMaxandMin();

	//>显示当前深度像素点的直方图
	cPixelHist.showHist();

	//>保存当前深度像素点的直方图
	cPixelHist.writeHistImageToFile(pixel_x, pixel_y);

	std::cout << std::endl;
#ifdef OUTPUT_COMPUTATIONAL_TIME
	finish = clock();
	std::cout << std::setprecision(4) << "The Process Of One Pixel : " << (double)(finish - start) / CLOCKS_PER_SEC << 's' << std::endl;
	start = clock();
#endif // OUTPUT_COMPUTATIONAL_TIME

#endif // SAVE_CERTAIN_PIXEL_HIST_IMAGE

	return 1;
}