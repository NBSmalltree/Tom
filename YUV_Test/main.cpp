#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/photo/photo.hpp>
#include <yuv.h>
#include "function.h"
#include <stdio.h>

using namespace std;

#define BACKGROUND_AVERAGE 0
#define BACKGROUND_GMM 0
#define HOLEFILL 0
#define PSNR_AND_MSSIM 0
#define YUV_READ_AND_WRITE 0
#define Y_CHANNEL_INPAINT 0
#define YUV2MAT2YUV 0
#define YUV2PNG 0
#define YUV2PNG_WITH_WHITEHOLE 0
#define YUV2BMP 0
#define PSNR_AND_SSIM_IMAGE_SERIES 0
#define PSNR_AND_SSIM_IMAGE_SERIES_BMP 0
#define MAT2YUV 1

int main(int argc, char ** argv)
{
	//写入配置;
	vector<int>compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	//申明存放Mat类型空间;
	//cv::Mat bgrImage(768, 1024, CV_8UC3);
	//读取文件流;
	//CIYuv yuvBuffer;
	//FILE *fin_view_l_ori, *fin_view_l_forward;
	//原始序列;syn_left
	//if ((fin_view_l_ori = fopen("bookarrival_6.yuv", "rb")) == NULL || 
	//	(fin_view_l_forward = fopen("syn_left.yuv", "rb")) == NULL) {
	//	fprintf(stderr, "Can't open input file(s)\n");
	//	return 2;
	//}

	//设置YUV序列参数;
	//if (!yuvBuffer.Resize(768, 1024, 420))
	//	return 2;

	//读取设定帧;
	//if (!yuvBuffer.ReadOneFrame(fin_view_l_ori, 0))
	//	return 2;

	//将YUV图片格式转换为Mat类型;
	//yuvBuffer.getData_inBGR(&bgrImage);

	//初始化成功测试;
	//cv::imshow("输出图;", bgrImage);

	//>【Test】Write Test
	//cv::cvtColor(bgrImage, bgrImage, CV_RGB2GRAY);
	//cv::imwrite("OutputImage.png", bgrImage);

	//写入到YUV文件测试;
	//yuvBuffer.setDataFromImgBGR(&bgr);
	
//求背景(求和取平均);
#if BACKGROUND_AVERAGE
	cv::Mat backGround(768, 1024, CV_32FC3, cv::Scalar(0, 0, 0));

	for (int i = 0; i < 50; i++) {
		yuvBuffer.ReadOneFrame(fin_view_l_ori, i);
		yuvBuffer.getData_inBGR(&bgrImage);
		
		//把CV_8UC3格式转换为CV_32FC3，不然无法累加;
		cv::Mat matimg;
		matimg = cv::Mat(bgrImage);
		matimg.convertTo(matimg, CV_32FC3, 1.0 / 255, 0);

		backGround += matimg;
	}

	backGround /= 50;
	backGround.convertTo(bgrImage, CV_8UC3, 255, 0);
	cv::imshow("输出图;", bgrImage);
	//imwrite("ARV_5.bmp", bg);
#endif // BACKGROUND_AVERAGE	

//求背景(GMM);
#if BACKGROUND_GMM

	FILE *fin, *fout;
	if (fopen_s(&fin, argv[1], "rb")||
		fopen_s(&fout,argv[2], "wb"))
		return 1;

	CIYuv yuvBuffer;
	yuvBuffer.Resize(atoi(argv[3]), atoi(argv[4]), 420);

	cv::Mat bgrImage(atoi(argv[3]), atoi(argv[4]), CV_8UC3);

	cv::BackgroundSubtractorMOG2 mog;
	cv::Mat foreground,bg;

	for (int i = 0; i < atoi(argv[5]); i++) {
		yuvBuffer.ReadOneFrame(fin, i);
		yuvBuffer.getData_inBGR(&bgrImage);

		mog(bgrImage, foreground, 0.01);
	}

	mog.getBackgroundImage(bg);

	yuvBuffer.setDataFromImgBGR(&bg);

	yuvBuffer.WriteOneFrame(fout);

	fclose(fin);
	fclose(fout);

	//cv::imshow("掩膜图;", bg);
	//cv::waitKey(0);
	//imwrite("GMM_5.bmp", bg);
#endif // BACKGROUND_AVERAGE

//按背景填充单视点空洞(针对单视点运动物体边缘空洞修复);
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

					//用白色块测试;
					//data1[j * 3] = 255;
					//data1[j * 3 + 1] = 255;
					//data1[j * 3 + 2] = 255;
				}
			}
		}
		//Mat dst;
		//mergeImg(dst, src, matimg);

		//imshow("对比图;", dst);

		cv::imshow("原始未处理;", src);
		cv::imshow("处理后;", bgrImage);
		cv::waitKey();
	}
#endif // HOLEFILL	

	//关闭流;
	//fclose(fin_view_l_ori);
	//fclose(fin_view_l_forward);

	//cv::waitKey(0);

#if PSNR_AND_MSSIM

	//>读取文件流
	FILE *fin_src1, *fin_src2, *fout1, *fout2;
	if ((fin_src1 = fopen(argv[1], "rb")) == NULL ||
		(fin_src2 = fopen(argv[2], "rb")) == NULL
		//(fout1 = fopen("PSNR.txt", "w")) == NULL ||
		//(fout2 = fopen("SSIM.txt", "w")) == NULL) 
	){
		fprintf(stderr, "Can't open input file(s)\n");
		return 2;
	}
	int totalFrame = atoi(argv[3]);
	int m_iHeight = atoi(argv[4]);
	int m_iWidth = atoi(argv[5]);

	//>申明存放Mat类型空间
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
		if (!yuvBuffer.ReadOneFrame(fin_src1, i))
			return 2;

		//>Transfer to Mat 1
		yuvBuffer.getData_inBGR(&srcImage1);

		//>Read Certain Frame 2
		if (!yuvBuffer.ReadOneFrame(fin_src2, i))
			return 2;

		//>Transfer to Mat 2
		yuvBuffer.getData_inBGR(&srcImage2);

		//>【Test】imshow
		//cv::imshow("输出图1;", srcImage1);
		//cv::imshow("输出图2;", srcImage2);
		//cv::waitKey(0);
		sumOfPSNR += getPSNR(srcImage1, srcImage2);
		sumOfMSSIM += getMSSIM(srcImage1, srcImage2);

		//>Write each data to txt file
		//double psnr = getPSNR(srcImage1, srcImage2);
		//cv::Scalar ssim = getMSSIM(srcImage1, srcImage2);
		//double ssim_out = (ssim[0] + ssim[1] + ssim[2]) / 3;

		//char s_psnr[7], s_ssim[6];
		//sprintf(s_psnr, "%.4f", psnr);
		//sprintf(s_ssim, "%.4f", ssim_out);

		//char space = ' ';
		//fwrite(s_psnr, sizeof(s_psnr), 1, fout1);
		//fwrite(&space, sizeof(char), 1, fout1);
		//fwrite(s_ssim, sizeof(s_ssim), 1, fout2);
		//fwrite(&space, sizeof(char), 1, fout2);
	}

	std::cout << "PSNR : " << sumOfPSNR / totalFrame << std::endl;
	std::cout << "MSSIM : " << (sumOfMSSIM[0] + sumOfMSSIM[1] + sumOfMSSIM[2]) / totalFrame / 3 << std::endl;

	//system("pause");

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
	//>U、V
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
	inpaint(backgroundImage, inpaintMask, inpaintedImage, 3, cv::INPAINT_TELEA); //此处第5个参数可以是 INPAINT_NS 或者 INPAINT_TELEA;
	//cv::imshow(">修补后的效果图", inpaintedImage);
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

#if YUV2PNG

	FILE *fin;
	if (fopen_s(&fin, argv[1], "rb"))
		return 1;
	CIYuv yuvBuffer;
	yuvBuffer.Resize(atoi(argv[2]), atoi(argv[3]), 420);

	for (int n = 0; n < atoi(argv[4]); n++) {
		yuvBuffer.ReadOneFrame(fin, n);

		cv::Mat MatBuffer(atoi(argv[2]), atoi(argv[3]), CV_8UC3);
		yuvBuffer.getData_inBGR(&MatBuffer);

		std::string outname = argv[5];
		char num[3];
		_itoa(n, num, 10);
		outname += num;
		outname += ".png";
		cv::imwrite(outname, MatBuffer);
	}

	fclose(fin);

#endif // YUV2PNG

#if YUV2PNG_WITH_WHITEHOLE

	FILE *fin;
	if (fopen_s(&fin, argv[1], "rb"))
		return 1;
	CIYuv yuvBuffer;
	yuvBuffer.Resize(atoi(argv[2]), atoi(argv[3]), 420);

	for (int n = 0; n < atoi(argv[4]); n++) {
		yuvBuffer.ReadOneFrame(fin, n);

		cv::Mat MatBuffer(atoi(argv[2]), atoi(argv[3]), CV_8UC3);
		yuvBuffer.getData_inBGR(&MatBuffer);

		//>Transfer Black Hole into White
		for (int h = 0; h < MatBuffer.rows; h++) {
			uchar* srcLine = MatBuffer.ptr<uchar>(h);
			for (int w = 0; w < MatBuffer.cols; w++) {
				if (srcLine[3 * w] <= 3 && srcLine[3 * w + 1] <= 3 && srcLine[3 * w + 2] <= 3) {
					srcLine[3 * w] = 255;
					srcLine[3 * w + 1] = 255;
					srcLine[3 * w + 2] = 255;
				}
			}
		}

		std::string outname = argv[5];
		char num[3];
		_itoa(n, num, 10);
		outname += num;
		outname += ".png";
		cv::imwrite(outname, MatBuffer);
	}

	fclose(fin);

#endif // YUV2PNG_WITH_WHITEHOLE

#if YUV2BMP

	FILE *fin;
	if (fopen_s(&fin, argv[1], "rb"))
		return 1;
	CIYuv yuvBuffer;
	yuvBuffer.Resize(atoi(argv[2]), atoi(argv[3]), 420);

	for (int n = 0; n < atoi(argv[4]); n++) {
		yuvBuffer.ReadOneFrame(fin, n);

		cv::Mat MatBuffer(atoi(argv[2]), atoi(argv[3]), CV_8UC3);
		yuvBuffer.getData_inBGR(&MatBuffer);

		std::string outname = argv[5];
		char num[3];
		_itoa(n, num, 10);
		outname += num;
		outname += ".bmp";
		cv::imwrite(outname, MatBuffer);
	}

	fclose(fin);

#endif // YUV2BMP

#if PSNR_AND_SSIM_IMAGE_SERIES

	//>输出文件名
	FILE *fout1, *fout2;
	if ((fout1 = fopen("PSNR.txt", "w")) == NULL ||
		(fout2 = fopen("SSIM.txt", "w")) == NULL) {
		fprintf(stderr, "Can't open input file(s)\n");
		return 2;
	}

	//>读取文件夹的路径及公共名字
	std::string filePath1 = argv[1];
	std::string filePath2 = argv[2];

	int totalFrame = atoi(argv[3]);
	int m_iHeight = atoi(argv[4]);
	int m_iWidth = atoi(argv[5]);

	//>申明存放Mat类型空间
	cv::Mat srcImage1(m_iHeight, m_iWidth, CV_8UC3);
	cv::Mat srcImage2(m_iHeight, m_iWidth, CV_8UC3);

	//>Init sumValue;
	double sumOfPSNR = 0;
	cv::Scalar sumOfMSSIM = 0;

	//>Start Processing
	for (int i = 0; i < totalFrame; i++) {

		char num[4];
		_itoa(i, num, 10);
		std::string name1 = filePath1 + num;
		std::string name2 = filePath2 + num;
		name1 += ".png";
		name2 += ".png";
		//>Read Certain Frame
		srcImage1 = cv::imread(name1, 2 | 4);
		srcImage2 = cv::imread(name2, 2 | 4);

		//>【Test】imshow
		//cv::imshow("输出图1;", srcImage1);
		//cv::imshow("输出图2;", srcImage2);
		//cv::waitKey(0);
		//sumOfPSNR += getPSNR(srcImage1, srcImage2);
		//sumOfMSSIM += getMSSIM(srcImage1, srcImage2);

		//>Write each data to txt file
		double psnr = getPSNR(srcImage1, srcImage2);
		cv::Scalar ssim = getMSSIM(srcImage1, srcImage2);
		double ssim_out = (ssim[0] + ssim[1] + ssim[2]) / 3;

		char s_psnr[7], s_ssim[6];
		sprintf(s_psnr, "%.4f", psnr);
		sprintf(s_ssim, "%.4f", ssim_out);

		char space = ' ';
		fwrite(s_psnr, sizeof(s_psnr), 1, fout1);
		fwrite(&space, sizeof(char), 1, fout1);
		fwrite(s_ssim, sizeof(s_ssim), 1, fout2);
		fwrite(&space, sizeof(char), 1, fout2);

	}

	//std::cout << "PSNR : " << sumOfPSNR / totalFrame << std::endl;
	//std::cout << "MSSIM : " << (sumOfMSSIM[0] + sumOfMSSIM[1] + sumOfMSSIM[2]) / totalFrame / 3 << std::endl;

#endif // PSNR_AND_SSIM_IMAGE_SERIES

#if PSNR_AND_SSIM_IMAGE_SERIES_BMP

	//>输出文件名
	FILE *fout1, *fout2;
	if ((fout1 = fopen("PSNR.txt", "w")) == NULL ||
		(fout2 = fopen("SSIM.txt", "w")) == NULL) {
		fprintf(stderr, "Can't open input file(s)\n");
		return 2;
	}

	//>读取文件夹的路径及公共名字
	std::string filePath1 = argv[1];
	std::string filePath2 = argv[2];

	int totalFrame = atoi(argv[3]);
	int m_iHeight = atoi(argv[4]);
	int m_iWidth = atoi(argv[5]);

	//>申明存放Mat类型空间
	cv::Mat srcImage1(m_iHeight, m_iWidth, CV_8UC3);
	cv::Mat srcImage2(m_iHeight, m_iWidth, CV_8UC3);

	//>Init sumValue;
	double sumOfPSNR = 0;
	cv::Scalar sumOfMSSIM = 0;

	//>Start Processing
	for (int i = 0; i < totalFrame; i++) {

		char num[4];
		_itoa(i, num, 10);
		std::string name1 = filePath1 + num;
		std::string name2 = filePath2 + num;
		name1 += ".bmp";
		name2 += ".png";
		//>Read Certain Frame
		srcImage1 = cv::imread(name1, 2 | 4);
		srcImage2 = cv::imread(name2, 2 | 4);

		//>【Test】imshow
		//cv::imshow("输出图1;", srcImage1);
		//cv::imshow("输出图2;", srcImage2);
		//cv::waitKey(0);
		//sumOfPSNR += getPSNR(srcImage1, srcImage2);
		//sumOfMSSIM += getMSSIM(srcImage1, srcImage2);

		//>Write each data to txt file
		double psnr = getPSNR(srcImage1, srcImage2);
		cv::Scalar ssim = getMSSIM(srcImage1, srcImage2);
		double ssim_out = (ssim[0] + ssim[1] + ssim[2]) / 3;

		char s_psnr[7], s_ssim[6];
		sprintf(s_psnr, "%.4f", psnr);
		sprintf(s_ssim, "%.4f", ssim_out);

		char space = ' ';
		fwrite(s_psnr, sizeof(s_psnr), 1, fout1);
		fwrite(&space, sizeof(char), 1, fout1);
		fwrite(s_ssim, sizeof(s_ssim), 1, fout2);
		fwrite(&space, sizeof(char), 1, fout2);
	}

	//std::cout << "PSNR : " << sumOfPSNR / totalFrame << std::endl;
	//std::cout << "MSSIM : " << (sumOfMSSIM[0] + sumOfMSSIM[1] + sumOfMSSIM[2]) / totalFrame / 3 << std::endl;

#endif // PSNR_AND_SSIM_IMAGE_SERIES_BMP

#if MAT2YUV
	FILE *fout;
	if (fopen_s(&fout, argv[1], "wb"))
		return 1;

	char imgName[10];
	string iName;
	cv::Mat bgrImage(atoi(argv[2]), atoi(argv[3]), CV_8UC3);

	CIYuv outBuffer;
	outBuffer.Resize(atoi(argv[3]), atoi(argv[2]), 420);

	for (int n = 0; n < atoi(argv[4]); n++) {
		_itoa(n, imgName, 10);
		iName = imgName;
		iName += ".png";
		bgrImage = cv::imread(iName, 2 | 4);

		//cv::imshow("temp", bgrImage);
		//cv::waitKey(0);

		outBuffer.setDataFromImgBGR(&bgrImage);

		outBuffer.WriteOneFrame(fout);
	}

	if (fout != NULL)
		fclose(fout);


#endif // YUV2MAT2YUV

	return 0;
}