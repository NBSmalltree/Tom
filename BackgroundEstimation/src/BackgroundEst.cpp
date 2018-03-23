#include <iostream>
#include <iomanip>
#include <ctime>
#include "version.h"
#include "ParameterViewSyn.h"
#include "Process.h"


#define SAVE_CERTAIN_PIXEL_HIST_IMAGE 0
#define GET_DEPTH_BACKGROUND_USING_HISTOGRAM_SEGMENTATION 0
#define GET_COLOR_BACKGROUND 0

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

	if (cParameter.Init(argc, argv) != 1) return 0;
	std::cout << std::endl;

	//>��ʼ��Process��������
	cBackgroundEstimation.Init(cParameter);

#ifdef OUTPUT_COMPUTATIONAL_TIME
	finish = clock();
	std::cout << std::setprecision(4) << "Initialization : " << (double)(finish - start) / CLOCKS_PER_SEC << 's' << std::endl;
	start = clock();
#endif // OUTPUT_COMPUTATIONAL_TIME
	
	//>���빹����ȱ���ͼ����
	if (!cBackgroundEstimation.initDepthBackground()) {
		std::cout << "Initialize CDepthBackground Class Failure!" << std::endl;
		return 3;
	}

	//>�����ͼ��Ϣ��������
	if (!cBackgroundEstimation.buildDepthMap()) {
		std::cout << "Depth Map Build Failure!" << std::endl;
		return 4;
	}
	
	//>�����ԡ�����ĳһ�ض����ص��ֱ��ͼ
	//cBackgroundEstimation.calcOnePixelHist(336, 128);

	//>����ȫ����ֱ��ͼ�����ɱ������ͼ
	cBackgroundEstimation.calcHist();
	
	//>���빹����ɫ����ͼ����
	if (!cBackgroundEstimation.initColorBackground()) {
		std::cout << "Initialize CColorBackground Class Failure!" << std::endl;
		return 5;
	}

	//>�����ɫ����ͼ
	cBackgroundEstimation.calcColorBackground();

#if GET_COLOR_BACKGROUND



#endif // GET_COLOR_BACKGROUND
	return 1;
}