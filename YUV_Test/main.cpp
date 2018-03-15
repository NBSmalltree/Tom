#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <yuv.h>

using namespace std;

#define BACKGROUND_AVERAGE 0
#define BACKGROUND_GMM 0
#define HOLEFILL 1

int main()
{
	//写入配置;
	vector<int>compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	//申明存放Mat类型空间;
	cv::Mat bgrImage(768, 1024, CV_8UC3);
	//读取文件流;
	CIYuv yuvBuffer;
	FILE *fin_view_l_ori, *fin_view_l_forward;
	//原始序列;syn_left
	if ((fin_view_l_ori = fopen("bookarrival_6.yuv", "rb")) == NULL || 
		(fin_view_l_forward = fopen("syn_left.yuv", "rb")) == NULL) {
		fprintf(stderr, "Can't open input file(s)\n");
		return 2;
	}

	//设置YUV序列参数;
	if (!yuvBuffer.Resize(768, 1024, 420))
		return 2;

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
	cv::BackgroundSubtractorMOG2 mog;
	cv::Mat foreground,bg;

	for (int i = 0; i < 50; i++) {
		yuvBuffer.ReadOneFrame(fin_view_l_ori, i);
		yuvBuffer.getData_inBGR(&bgrImage);

		mog(bgrImage, foreground, 0.01);
	}

	mog.getBackgroundImage(bg);
	cv::imshow("掩膜图;", bg);
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
	fclose(fin_view_l_ori);
	fclose(fin_view_l_forward);

	cv::waitKey(0);

	return 0;
}