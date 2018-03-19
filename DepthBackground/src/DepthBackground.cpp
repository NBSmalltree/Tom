#include <iostream>
#include <iomanip>
#include <ctime>
#include "version.h"
#include "ParameterViewSyn.h"
#include "BackgroundEstimation.h"
#include "PixelHist.h"

#define SAVE_CERTAIN_PIXEL_HIST_IMAGE 0
#define GET_DEPTH_BACKGROUND_USING_HISTOGRAM_SEGMENTATION 1

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
	int pixel_x = 208;
	int pixel_y = 16;

	//>提取特定像素点的深度，以更新步长为长度，最后一个参数为起始更新帧(转换深度表格式)
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
	//cPixelHist.showHist();

	//>保存当前深度像素点的直方图
	//cPixelHist.writeHistImageToFile(pixel_x, pixel_y);

	std::cout << std::endl;
#ifdef OUTPUT_COMPUTATIONAL_TIME
	finish = clock();
	std::cout << std::setprecision(4) << "The Process Of One Pixel : " << (double)(finish - start) / CLOCKS_PER_SEC << 's' << std::endl;
	start = clock();
#endif // OUTPUT_COMPUTATIONAL_TIME

#endif // SAVE_CERTAIN_PIXEL_HIST_IMAGE

#ifdef GET_DEPTH_BACKGROUND_USING_HISTOGRAM_SEGMENTATION

	//>声明深度背景图空间
	cv::Mat backgroundImage(cParameter.getSourceHeight(), cParameter.getSourceWidth(), CV_8UC1);

	//>开始遍历全图像
	for (int h = 0; h < cParameter.getSourceHeight(); h++) {
		//>定义深度图Mat类行指针
		uchar* p_bgLine = backgroundImage.ptr<uchar>(h);
		for (int w = 0; w < cParameter.getSourceWidth(); w++) {

			//>提取特定像素点的深度，以更新步长为长度，最后一个参数为起始更新帧(转换深度表格式)
			cBackgroundEstimation.extractCertainPixelDepthMap(w, h, 0);

			//>进入直方图统计模块(size,scale,updatestep)
			CPixelHist cPixelHist(256, 1, cParameter.getUpdateStep());

			//>计算当前深度像素点的直方图
			cPixelHist.calcHist(cBackgroundEstimation.getCertainPixelDepthMapBuffer());

			//>判断是否只有一个峰，如果有且只有一个峰，则此像素点的背景深度值即为该峰对应的深度值
			if (cPixelHist.isOnlyOneBar()) {
				p_bgLine[w] = cPixelHist.getDepthValueTag();
				continue;
			}

			//>测试最大值最小值
			cPixelHist.findMaxandMin();

			//如果最高峰频次（高度）小于N/5，则此像素点的背景深度值赋值为0
			if (cPixelHist.getMaxBarValue() < cParameter.getUpdateStep()) {
				p_bgLine[w] = 0;
				continue;
			}

			//>从直方图最左边开始向右边遍历，记录第一个峰值大于或等于最高峰频次1/4的峰，作为深度值最低的主峰
			p_bgLine[w] = cPixelHist.getBackgroundMainPeek();
		}
	}

	//>测试提取的深度背景图
	cv::imshow("背景深度图;", backgroundImage);
	cv::waitKey(0);

#endif // GET_DEPTH_BACKGROUND_USING_HISTOGRAM_SEGMENTATION
	return 1;
}