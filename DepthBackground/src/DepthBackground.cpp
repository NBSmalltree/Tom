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

	//>�����ͼ��Ϣ��������
	for (int n = cParameter.getStartFrame(); n < cParameter.getStartFrame() + cParameter.getTotalFrame(); n++) {
		std::cout << "Frame number = " << n << std::endl;
		std::cout << "Start Processing:";

		if (!cBackgroundEstimation.getFrameBuffer()->ReadOneFrame(fin, n)) {
			std::cout << "Set Frame Head Failure!" << std::endl;
			break;
		}
		std::cout << '.';

		//>��Test��:�м���̵ı��漰��ʾ
		/*cBackgroundEstimation.getFrameBuffer()->getData_inBGR(cBackgroundEstimation.getBgrBuffer());

		if (!cBackgroundEstimation.writeCurrentImage()) {
			std::cout << "Write Current Image Failure!" << std::endl;
			break;
		}
		std::cout << "Write Current Image Success!" << std::endl;

		cBackgroundEstimation.showCurrentImage();*/

		//>����ǰ֡������Ϣ���浽���ͼ���ݽṹ��
		if (!cBackgroundEstimation.writeOneDepthMap(cBackgroundEstimation.getFrameBuffer(), n)) {
			std::cout << "Current Frame Depth Map Save Failure!" << std::endl;
			break;
		}
		std::cout << '.';

		//>��Test��:(x,y)=(256,144)�������ֵ����
		//cBackgroundEstimation.showDepthMapTestMemory(144, 256, 15, n);

		//>��Test��:д��һ���ض�������ֵ���ļ�
		//cBackgroundEstimation.writeOnePixeltoFile(304, 160, n);

		std::cout << std::endl;
		//system("pause");
	}//>������ͼ��Ϣ����

	std::cout << std::endl;
#ifdef OUTPUT_COMPUTATIONAL_TIME
	finish = clock();
	std::cout << std::setprecision(4) << "Store All The Depth Map Duration : " << (double)(finish - start) / CLOCKS_PER_SEC << 's' << std::endl;
	start = clock();
#endif // OUTPUT_COMPUTATIONAL_TIME

#if SAVE_CERTAIN_PIXEL_HIST_IMAGE
	int pixel_x = 208;
	int pixel_y = 16;

	//>��ȡ�ض����ص����ȣ��Ը��²���Ϊ���ȣ����һ������Ϊ��ʼ����֡
	cBackgroundEstimation.extractCertainPixelDepthMap(pixel_x, pixel_y, 0);

	//>����ֱ��ͼͳ��ģ��(size,scale,updatestep)
	CPixelHist cPixelHist(256, 1, 100);

	//>����ֱ��ͼģ�����������
	//cPixelHist.printInputData(cBackgroundEstimation.getCertainPixelDepthMapBuffer());

	//>���㵱ǰ������ص��ֱ��ͼ
	cPixelHist.calcHist(cBackgroundEstimation.getCertainPixelDepthMapBuffer());

	//>�������ֵ��Сֵ
	cPixelHist.findMaxandMin();

	//>��ʾ��ǰ������ص��ֱ��ͼ
	cPixelHist.showHist();

	//>���浱ǰ������ص��ֱ��ͼ
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