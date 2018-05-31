#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/photo/photo.hpp>
#include <yuv.h>
#include "function.h"

using namespace std;

#define BACKGROUND_AVERAGE 0
#define BACKGROUND_GMM 0
#define HOLEFILL 0
#define PSNR_AND_MSSIM 0
#define YUV_READ_AND_WRITE 0
#define Y_CHANNEL_INPAINT 0
#define YUV2MAT2YUV 1

int main(int argc, char ** argv)
{
	//д������;
	vector<int>compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	//�������Mat���Ϳռ�;
	//cv::Mat bgrImage(768, 1024, CV_8UC3);
	//��ȡ�ļ���;
	//CIYuv yuvBuffer;
	//FILE *fin_view_l_ori, *fin_view_l_forward;
	//ԭʼ����;syn_left
	//if ((fin_view_l_ori = fopen("bookarrival_6.yuv", "rb")) == NULL || 
	//	(fin_view_l_forward = fopen("syn_left.yuv", "rb")) == NULL) {
	//	fprintf(stderr, "Can't open input file(s)\n");
	//	return 2;
	//}

	//����YUV���в���;
	//if (!yuvBuffer.Resize(768, 1024, 420))
	//	return 2;

	//��ȡ�趨֡;
	//if (!yuvBuffer.ReadOneFrame(fin_view_l_ori, 0))
	//	return 2;

	//��YUVͼƬ��ʽת��ΪMat����;
	//yuvBuffer.getData_inBGR(&bgrImage);

	//��ʼ���ɹ�����;
	//cv::imshow("���ͼ;", bgrImage);

	//>��Test��Write Test
	//cv::cvtColor(bgrImage, bgrImage, CV_RGB2GRAY);
	//cv::imwrite("OutputImage.png", bgrImage);

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
	//fclose(fin_view_l_ori);
	//fclose(fin_view_l_forward);

	//cv::waitKey(0);

#if PSNR_AND_MSSIM

	//>��ȡ�ļ���
	FILE *fin_src1, *fin_src2;
	if ((fin_src1 = fopen(argv[1], "rb")) == NULL ||
		(fin_src2 = fopen(argv[2], "rb")) == NULL) {
		fprintf(stderr, "Can't open input file(s)\n");
		return 2;
	}
	int totalFrame = atoi(argv[3]);
	int m_iHeight = atoi(argv[4]);
	int m_iWidth = atoi(argv[5]);

	//>�������Mat���Ϳռ�
	cv::Mat srcImage1(m_iHeight, m_iWidth, CV_8UC3);
	cv::Mat srcImage2(m_iHeight, m_iWidth, CV_8UC3);

	//>Init YUV Buffer
	CIYuv yuvBuffer;
	if (!yuvBuffer.Resize(m_iHeight, m_iWidth, 420))
		return 2;

	//>Init sumValue;
	double sumOfPSNR = 0;
	cv::Scalar sumOfMSSIM = 0;

	//>Start Processing
	for (int i = 0; i < totalFrame; i++) {

		//>Read Certain Frame 1
		if (!yuvBuffer.ReadOneFrame(fin_src1, 0))
			return 2;

		//>Transfer to Mat 1
		yuvBuffer.getData_inBGR(&srcImage1);

		//>Read Certain Frame 2
		if (!yuvBuffer.ReadOneFrame(fin_src2, 0))
			return 2;

		//>Transfer to Mat 2
		yuvBuffer.getData_inBGR(&srcImage2);

		//>��Test��imshow
		//cv::imshow("���ͼ1;", srcImage1);
		//cv::imshow("���ͼ2;", srcImage2);
		//cv::waitKey(0);
		sumOfPSNR += getPSNR(srcImage1, srcImage2);
		sumOfMSSIM += getMSSIM(srcImage1, srcImage2);
	}

	std::cout << "PSNR : " << sumOfPSNR / totalFrame << std::endl;
	std::cout << "MSSIM : " << (sumOfMSSIM[0] + sumOfMSSIM[1] + sumOfMSSIM[2]) / totalFrame / 3 << std::endl;

	system("pause");

#endif // PSNR_AND_MSSIM

#if YUV_READ_AND_WRITE
	
	std::string depthVideoName = "bookarrival_9.yuv";
	std::string outVideoName = "testout.yuv";
	FILE *fin_depth, *fout_depth;
	if (fopen_s(&fin_depth, depthVideoName.c_str(), "rb")||
		fopen_s(&fout_depth,outVideoName.c_str(),"wb"))
		return false;
	
	CIYuv depthVideo(768, 1024, 420);
	CIYuv outVideo(768, 1024, 420);

	int height = depthVideo.getHeight();
	int width = depthVideo.getWidth();
	int heightUV = depthVideo.getHeightUV();
	int widthUV = depthVideo.getWidthUV();

	depthVideo.ReadOneFrame(fin_depth, 0);

	//>Y
	for (int h = 0; h < height; h++)
		for (int w = 0; w < width; w++)
			outVideo.Y[h][w] = depthVideo.Y[h][w];
	//>U��V
	for (int h = 0; h < heightUV; h++)
		for (int w = 0; w < widthUV; w++) {
			outVideo.U[h][w] = depthVideo.U[h][w];
			outVideo.V[h][w] = depthVideo.V[h][w];
		}

	outVideo.WriteOneFrame(fout_depth);

	fclose(fin_depth);
	fclose(fout_depth);

#endif // YUV_READ_AND_WRITE

#if Y_CHANNEL_INPAINT

	std::string inVideoName = "DepthBackground.yuv";
	std::string outVideoName = "TargetBackground.yuv";

	FILE *fin, *fout;
	if (fopen_s(&fin, inVideoName.c_str(), "rb")||
		fopen_s(&fout,outVideoName.c_str(),"wb"))
		return false;

	CIYuv inVideo(768, 1024, 420);
	CIYuv outVideo(768, 1024, 420);
	int height = inVideo.getHeight();
	int width = inVideo.getWidth();

	inVideo.ReadOneFrame(fin, 0);

	cv::Mat backgroundImage(height, width, CV_8UC1, inVideo.Y[0]);

	cv::Mat inpaintMask;
	inpaintMask = cv::Mat::zeros(backgroundImage.size(), CV_8UC1);

	for (int h = 0; h < height; h++) {
		uchar* backgroundLine = backgroundImage.ptr<uchar>(h);
		uchar* inpaintMaskLine = inpaintMask.ptr<uchar>(h);
		for (int w = 0; w < width; w++) {
			if (backgroundLine[w] == 0)
				inpaintMaskLine[w] = 255;
		}
	}

	//>Process Inpaint Algorithm
	cv::Mat inpaintedImage;
	inpaint(backgroundImage, inpaintMask, inpaintedImage, 3, cv::INPAINT_TELEA); //�˴���5������������ INPAINT_NS ���� INPAINT_TELEA;
	//cv::imshow(">�޲����Ч��ͼ", inpaintedImage);
	//cv::waitKey(0);

	for (int h = 0; h < height; h++) {
		uchar* matLine = inpaintedImage.ptr<uchar>(h);
		for (int w = 0; w < width; w++)	{
			outVideo.Y[h][w] = matLine[w];
		}
	}

	outVideo.WriteOneFrame(fout);

	fclose(fin);
	fclose(fout);

#endif // Y_CHANNEL_INPAINT

#if YUV2MAT2YUV
	FILE *fin, *fout;
	if (fopen_s(&fin, argv[1], "rb") ||
		fopen_s(&fout, argv[2], "wb"))
		return 1;

	CIYuv yuvBuffer;
	yuvBuffer.Resize(atoi(argv[3]), atoi(argv[4]), 420);
	CIYuv outBuffer;
	outBuffer.Resize(atoi(argv[3]), atoi(argv[4]), 420);

	for (int n = 0; n < atoi(argv[5]); n++) {
		yuvBuffer.ReadOneFrame(fin, n);

		cv::Mat MatBuffer(atoi(argv[3]), atoi(argv[4]), CV_8UC3);
		yuvBuffer.getData_inBGR(&MatBuffer);

		outBuffer.setDataFromImgBGR(&MatBuffer);

		outBuffer.WriteOneFrame(fout);
	}

	fclose(fin);
	fclose(fout);


#endif // YUV2MAT2YUV

	return 0;
}