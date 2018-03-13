#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <yuv.h>

using namespace std;

#define BACKGROUND_AVERAGE 0
#define BACKGROUND_GMM 0
#define HOLEFILL 1

int main()
{
	//д������;
	vector<int>compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	//�������Mat���Ϳռ�;
	cv::Mat bgrImage(768, 1024, CV_8UC3);
	//��ȡ�ļ���;
	CIYuv yuvBuffer;
	FILE *fin_view_l_ori, *fin_view_l_forward;
	//ԭʼ����;syn_left
	if ((fin_view_l_ori = fopen("bookarrival_6.yuv", "rb")) == NULL || 
		(fin_view_l_forward = fopen("syn_left.yuv", "rb")) == NULL) {
		fprintf(stderr, "Can't open input file(s)\n");
		return 2;
	}

	//����YUV���в���;
	if (!yuvBuffer.Resize(768, 1024, 420))
		return 2;

	//��ȡ�趨֡;
	//if (!yuvBuffer.ReadOneFrame(fin_yuv, 0))
	//	return 2;

	//��YUVͼƬ��ʽת��ΪMat����;
	//yuvBuffer.getData_inBGR(&bgr);

	//��ʼ���ɹ�����;
	//imshow("���ͼ;", bgr);

	//д�뵽YUV�ļ�����;
	//yuvBuffer.setDataFromImgBGR(&bgr);
	
//�󱳾�(���ȡƽ��);
#if BACKGROUND_AVERAGE
	cv::Mat backGround(768, 1024, CV_32FC3, cv::Scalar(0, 0, 0));

	for (int i = 0; i < 50; i++) {
		yuvBuffer.ReadOneFrame(fin_view_l_ori, i);
		yuvBuffer.getData_inBGR(&bgrImage);
		
		//��CV_8UC3��ʽת��ΪCV_32FC3����Ȼ�޷��ۼ�;
		cv::Mat matimg;
		matimg = cv::Mat(bgrImage);
		matimg.convertTo(matimg, CV_32FC3, 1.0 / 255, 0);

		backGround += matimg;
	}

	backGround /= 50;
	backGround.convertTo(bgrImage, CV_8UC3, 255, 0);
	cv::imshow("���ͼ;", bgrImage);
	//imwrite("ARV_5.bmp", bg);
#endif // BACKGROUND_AVERAGE	

//�󱳾�(GMM);
#if BACKGROUND_GMM
	cv::BackgroundSubtractorMOG2 mog;
	cv::Mat foreground,bg;

	for (int i = 0; i < 50; i++) {
		yuvBuffer.ReadOneFrame(fin_view_l_ori, i);
		yuvBuffer.getData_inBGR(&bgrImage);

		mog(bgrImage, foreground, 0.01);
	}

	mog.getBackgroundImage(bg);
	cv::imshow("��Ĥͼ;", bg);
	//imwrite("GMM_5.bmp", bg);
#endif // BACKGROUND_AVERAGE

//��������䵥�ӵ�ն�(��Ե��ӵ��˶������Ե�ն��޸�);
#if HOLEFILL
	cv::Mat bg = cv::imread("GMM_20.bmp", 2 | 4);

	for (int frame = 0; frame < 50; frame++) {
		yuvBuffer.ReadOneFrame(fin_view_l_forward, frame);
		yuvBuffer.getData_inBGR(&bgrImage);

		cv::Mat src;
		bgrImage.copyTo(src);

		for (int i = 0; i < bgrImage.rows; i++) {
			uchar* data1 = bgrImage.ptr<uchar>(i);
			uchar* data2 = bg.ptr<uchar>(i);
			for (int j = 0; j < bgrImage.cols; j++) {
				if (data1[j * 3] <= 10 && data1[j * 3 + 1] <= 10 && data1[j * 3 + 2] <= 10) {
					data1[j * 3] = data2[j * 3];
					data1[j * 3 + 1] = data2[j * 3 + 1];
					data1[j * 3 + 2] = data2[j * 3 + 2];

					//�ð�ɫ�����;
					//data1[j * 3] = 255;
					//data1[j * 3 + 1] = 255;
					//data1[j * 3 + 2] = 255;
				}
			}
		}
		//Mat dst;
		//mergeImg(dst, src, matimg);

		//imshow("�Ա�ͼ;", dst);

		cv::imshow("ԭʼδ����;", src);
		cv::imshow("�����;", bgrImage);
		cv::waitKey();
	}
#endif // HOLEFILL	

	//�ر���;
	fclose(fin_view_l_ori);
	fclose(fin_view_l_forward);

	cv::waitKey(0);

	return 0;
}