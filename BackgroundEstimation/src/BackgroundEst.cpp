#include <iostream>
#include <iomanip>
#include <ctime>
#include "version.h"
#include "ParameterViewSyn.h"
#include "Process.h"


int main(int argc, char ** argv)
{
	std::cout.setf(std::ios::fixed);
	std::cout << "This is DepthBackground Process" << std::endl;
	std::cout << "Author : Tom Rubin" << std::endl;
	std::cout << std::setprecision(1) << "Version : " << VERSION << std::endl << std::endl;

#ifdef OUTPUT_COMPUTATIONAL_TIME
	clock_t start, finish, first;
	start = clock();
#endif // OUTPUT_COMPUTATIONAL_TIME

	ParameterViewSyn cParameter;
	CBackgroundEstimation cBackgroundEstimation;

	if (cParameter.Init(argc, argv) != 1) {
		std::cout << "Initialize File Parameters Failure!" << std::endl;
		return 2;
	}

	//>��ʼ��Process��������
	cBackgroundEstimation.Init(cParameter);

#ifdef OUTPUT_COMPUTATIONAL_TIME
	finish = clock();
	std::cout << std::setprecision(2) << "Initialization : " << (double)(finish - start) / CLOCKS_PER_SEC << 's' << std::endl << std::endl;
	first = start = clock();
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
	//cBackgroundEstimation.calcOnePixelHist(1824, 448);
	
	//>����ȫ����ֱ��ͼ�����ɱ������ͼ
	cBackgroundEstimation.calcHist();

#ifdef OUTPUT_COMPUTATIONAL_TIME
	finish = clock();
	std::cout << std::setprecision(2) << "Depth Segmentation Time : " << (double)(finish - start) / CLOCKS_PER_SEC << 's' << std::endl << std::endl;
	start = clock();
#endif // OUTPUT_COMPUTATIONAL_TIME

	//>���빹����ɫ����ͼ����
	if (!cBackgroundEstimation.initColorBackground()) {
		std::cout << "Initialize CColorBackground Class Failure!" << std::endl;
		return 5;
	}

	//>�����ɫ����ͼ
	if (!cBackgroundEstimation.calcColorBackground()) {
		std::cout << "Process Color Reconstruction Failure!" << std::endl;
		return 6;
	}

#ifdef OUTPUT_COMPUTATIONAL_TIME
	finish = clock();
	std::cout << std::setprecision(2) << "Color Reconstruction Time : " << (double)(finish - start) / CLOCKS_PER_SEC << 's' << std::endl << std::endl;
	std::cout << std::setprecision(2) << "Total Processing Time : " << (double)(finish - first) / CLOCKS_PER_SEC << 's' << std::endl << std::endl;
#endif // OUTPUT_COMPUTATIONAL_TIME

	return 1;
}