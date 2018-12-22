#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/photo/photo.hpp>
#include <iostream>
#include <yuv.h>
#include <msrcr.h>

using namespace std;
using namespace cv;

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

//--------------------------全局函数开关;--------------------------------

# define HISTOGRAM 0

# define SIFT_AND_BFMATCHER 0
# define SURF_AND_BFMATCHER 0
# define SURF_AND_FLANN 1
# define SURF_AND_FLANN_AND_OBJECT_DETECTION 0
# define ORB_AND_FLANN 0

# define ERODE 0
# define DILATE 0
# define ADVANCED_MORPHOLOGY 0 //高级形态学处理;
# define RESIZE 0
# define PYRAMID 0
# define BINARYZATION 0
# define MEAN_FILTERING 0
# define CANNY_EDGE_DETECTION 0


# define YUV_CLASS 0

# define WATERSHED 0
# define INPAINT 0
# define MYINPAINT 0

//--------------------------全局函数申明部分;----------------------------
static void on_ContrastAndBright(int, void *);

//--------------------------全局变量申明部分;----------------------------
int g_nContrastValue; //对比度值;
int g_nBrightValue; //亮度值;
Mat g_srcImage, g_dstImage;

//--------------------------全局函数实现部分;----------------------------
//描述：改变图像对比度和亮度值的回调函数;
static void on_ContrastAndBright(int, void *) {

	//创建窗口;
	namedWindow("【原始图窗口】;", 1);

	//三个for循环，执行运算g_dstImage(i,j) = a*g_srcImage(i,j) + b;
	for (int y = 0; y < g_srcImage.rows; y++) {
		for (int x = 0; x < g_srcImage.cols; x++) {
			for (int c = 0; c < 3; c++) {
				//g_dstImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>((g_nContrastValue*0.01)*(g_srcImage.at<Vec3b>(y, x)[c]) + g_nBrightValue);
				g_dstImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>((g_srcImage.at<Vec3b>)(y, x)[c] + g_nBrightValue - 255);
			}
		}
	}

	imshow("【原始图窗口】;", g_srcImage);
	imshow("【效果图窗口】;", g_dstImage);
}

//描述：并排显示两张图像;
void mergeImg(Mat &dst, Mat &src1, Mat &src2) {
	int rows = max(src1.rows, src2.rows);
	int cols = src1.cols + src2.cols;
	dst.create(rows, cols, src1.type());
	src1.copyTo(dst(Rect(0, 0, src1.cols, src1.rows)));
	src2.copyTo(dst(Rect(src1.cols, 0, src2.cols, src2.rows)));
}

//描述：生成随机颜色函数;
Vec3b RandomColor(int value)
{
	value = value % 255;  //生成0~255的随机数  
	RNG rng;
	int aa = rng.uniform(0, value);
	int bb = rng.uniform(0, value);
	int cc = rng.uniform(0, value);
	return Vec3b(aa, bb, cc);
}

//描述：响应鼠标消息的回调函数;
Mat srcImage1, inpaintMask;
Point previousPoint(-1, -1); //原来的点坐标;
static void On_Mouse(int event, int x, int y, int flags, void*) {
	//鼠标左键弹起消息;
	if (event == EVENT_LBUTTONUP || !(flags & EVENT_FLAG_LBUTTON))
		previousPoint = Point(-1, -1);
	//鼠标左键按下消息;
	else if (event == EVENT_LBUTTONDOWN)
		previousPoint = Point(x, y);
	//鼠标按下并移动，进行绘制;
	else if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON)) {
		Point pt(x, y);
		if (previousPoint.x < 0)
			previousPoint = pt;
		//绘制白色线条;
		line(inpaintMask, previousPoint, pt, Scalar::all(255), 5, 8, 0);
		line(srcImage1, previousPoint, pt, Scalar::all(255), 5, 8, 0);
		previousPoint = pt;
		imshow("原始图;", srcImage1);
	}
}

int main(int argc, char ** argv) {

	//写入配置;
	vector<int>compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

//--------------------------直方图的main调用;--------------------------------------
#if HISTOGRAM
	//--------------------------亮度、对比度调整main调用;------------------------------
	/*
	g_srcImage = imread("opencv.png");
	if (!g_srcImage.data){
		printf("Image Read Failure!!\n");
		return false;
	}
	g_dstImage = Mat::zeros(g_srcImage.size(), g_srcImage.type());

	//init;
	g_nContrastValue = 80;
	g_nBrightValue = 255;

	namedWindow("【效果图窗口】;", 1);

	//创建轨迹条;
	createTrackbar("对比度：;", "【效果图窗口】;", &g_nContrastValue, 300, on_ContrastAndBright);
	createTrackbar("亮度：;", "【效果图窗口】;", &g_nBrightValue, 512, on_ContrastAndBright);

	//init;
	on_ContrastAndBright(g_nContrastValue, 0);
	on_ContrastAndBright(g_nBrightValue, 0);

	while (char(waitKey(1)) != 'q') {}
	*/

	//--------------------------直方图均衡化(灰度)main调用;----------------------------
	/*
	Mat srcImage, dstImage;
	srcImage = imread("1.jpg", 1);
	if (!srcImage.data) { printf("读取图片错误，请确定目录下是否有imread函数指定图片存在~！; \n"); return false; }

	cvtColor(srcImage, srcImage, CV_BGR2GRAY);
	imshow("原始图;", srcImage);

	equalizeHist(srcImage, dstImage);

	imshow("经过直方图均衡化后的图;", dstImage);
	*/

	//--------------------------直方图均衡化(彩色)main调用;----------------------------
	/*
	Mat srcImage, dstImage, dstImageBlue, dstImageGreen, dstImageRed;
	srcImage = imread("znn.jpg", 1);
	if (!srcImage.data) { printf("读取图片错误，请确定目录下是否有imread函数指定图片存在~！; \n"); return false; }

	imshow("原始图;", srcImage);
	
	vector<Mat> channels;
	Mat imageBlueChannel;
	Mat imageGreenChannel;
	Mat imageRedChannel;

	split(srcImage, channels);
	imageBlueChannel = channels.at(0);
	imageGreenChannel = channels.at(1);
	imageRedChannel = channels.at(2);

	imshow("原始蓝通道;", imageBlueChannel);
	imshow("原始绿通道;", imageGreenChannel);
	imshow("原始红通道;", imageRedChannel);

	equalizeHist(imageBlueChannel, imageBlueChannel);
	equalizeHist(imageGreenChannel, imageGreenChannel);
	equalizeHist(imageRedChannel, imageRedChannel);

	merge(channels, dstImage);

	imshow("处理蓝通道;", imageBlueChannel);
	imshow("处理绿通道;", imageGreenChannel);
	imshow("处理红通道;", imageRedChannel);

	imshow("经过直方图均衡化后的图;", dstImage);
	
	imwrite("out2.png", dstImage);
	*/

	//--------------------------绘制H-S直方图main调用;---------------------------------
	/*
	Mat srcImage, hsvImage;
	srcImage = imread("1.jpg");
	cvtColor(srcImage, hsvImage, COLOR_BGR2HSV);

	//parameter
	//将色调量化为30个等级，将饱和度量化为32个等级;
	int hueBinNum = 30;//色调直方图直条数量;
	int saturationBinNum = 32;//饱和度直方图直条数量;
	int histSize[] = { hueBinNum,saturationBinNum };
	float hueRanges[] = { 0, 180 };//定义色调的变化范围为0到179;
	float saturationRanges[] = { 0,256 };//定义饱和度的变化范围为0(黑、白、灰)到255(纯光谱颜色);
	const float* ranges[] = { hueRanges,saturationRanges };
	MatND dstHist;

	//参数准备,calcHist函数中将计算第0通道和第1通道的直方图;
	int channels[] = { 0,1 };
	calcHist(&hsvImage,//输入的数组;
		1,//数组个数为1;
		channels,//通道索引;
		Mat(),//不使用掩膜;
		dstHist,//输出的目标直方图;
		2,//需要计算的直方图的维度为2;
		histSize,//存放每个维度的直方图尺寸的数组;
		ranges,//每一维数值的取值范围数组;
		true,//指示直方图是否均匀的标识符，true表示均匀的直方图;
		false//累计标识符，false表示直方图在配置阶段会被清零;
	);

	//绘制直方图准备参数;
	double maxValue = 0;//最大值;
	minMaxLoc(dstHist, 0, &maxValue, 0, 0);//查找数组和子数组的全局最小值和最大值存入maxValue中;
	int scale = 10;
	Mat histImg = Mat::zeros(saturationBinNum * scale, hueBinNum * 10, CV_8UC3);

	//双层循环，进行直方图绘制;
	for (int hue = 0; hue < hueBinNum; hue++){
		for (int saturation = 0; saturation < saturationBinNum; saturation++) {
			float binValue = dstHist.at<float>(hue, saturation);//直方图直条的值;
			int intensity = cvRound(binValue * 255 / maxValue);//强度;

			//正式进行绘制;
			rectangle(histImg, Point(hue*scale, saturation*scale), Point((hue + 1)*scale - 1, (saturation + 1)*scale - 1), Scalar::all(intensity),7);
		}
	}

	//显示效果图;
	imshow("素材图;", srcImage);
	imshow("H-S直方图;", histImg);
	*/

	//--------------------------绘制一维直方图main调用;--------------------------------
	/*
	Mat srcImage = imread("1.jpg", 0);
	imshow("原图;", srcImage);
	if (!srcImage.data) { cout << "Fail to Load Image" << endl; return 0; }

	//定义变量;
	MatND dstHist;
	int dims = 1;
	float hranges[] = { 0,255 };
	const float *range[] = { hranges };//这里需要const类型;
	int size = 256;
	int channels = 0;

	//计算图像的直方图;
	calcHist(&srcImage, 1, &channels, Mat(), dstHist, dims, &size, range);
	int scale = 1;

	Mat dstImage(size * scale, size, CV_8U, Scalar(0));

	//获取最大值和最小值;
	double minValue = 0;
	double maxValue = 0;
	minMaxLoc(dstHist, &minValue, &maxValue, 0, 0);

	//绘制直方图;
	int hpt = saturate_cast<int>(0.9 * size);
	for (int i = 0; i < 256; i++) {
		float binValue = dstHist.at<float>(i);
		int realValue = saturate_cast<int>(binValue * hpt / maxValue);
		rectangle(dstImage, Point(i * scale, size - 1), Point((i + 1)*scale - 1, size - realValue), Scalar(255));
	}

	imshow("一维直方图;", dstImage);
	*/

	//--------------------------绘制RGB三色直方图main调用;-----------------------------
	/*
	Mat srcImage = imread("1.jpg");
	imshow("原图;", srcImage);

	//参数准备;
	int bins = 256;
	int hist_size[] = { bins };
	float range[] = { 0,256 };
	const float* ranges[] = { range };
	MatND redHist, greenHist, blueHist;

	//直方图计算;
	int channels_r[] = { 0 };
	calcHist(&srcImage, 1, channels_r, Mat(),//不使用掩膜;
		redHist, 1, hist_size, ranges, true, false);

	int channels_g[] = { 1 };
	calcHist(&srcImage, 1, channels_g, Mat(),//不使用掩膜;
		greenHist, 1, hist_size, ranges, true, false);

	int channels_b[] = { 1 };
	calcHist(&srcImage, 1, channels_b, Mat(),//不使用掩膜;
		blueHist, 1, hist_size, ranges, true, false);


	//绘制三色直方图;
	//参数准备;
	double maxValue_red, maxValue_green, maxValue_blue;
	minMaxLoc(redHist, 0, &maxValue_red, 0, 0);
	minMaxLoc(greenHist, 0, &maxValue_green, 0, 0);
	minMaxLoc(blueHist, 0, &maxValue_blue, 0, 0);
	int scale = 1;
	int histHeight = 256;
	Mat histImage = Mat::zeros(histHeight, bins * 3, CV_8UC3);

	//开始绘制;
	for (int i = 0; i < bins; i++) {
		//参数准备;
		float binValue_red = redHist.at<float>(i);
		float binValue_green = greenHist.at<float>(i);
		float binValue_blue = blueHist.at<float>(i);
		int intensity_red = cvRound(binValue_red * histHeight / maxValue_red);
		int intensity_green = cvRound(binValue_green * histHeight / maxValue_green);
		int intensity_blue = cvRound(binValue_blue * histHeight / maxValue_blue);

		rectangle(histImage, Point(i*scale, histHeight - 1), Point((i + 1)*scale - 1, histHeight - intensity_red), Scalar(255, 0, 0));
		rectangle(histImage, Point((i + bins)*scale, histHeight - 1), Point((i + bins + 1)*scale - 1, histHeight - intensity_green), Scalar(0, 255, 0));
		rectangle(histImage, Point((i + bins * 2)*scale, histHeight - 1), Point((i + bins * 2 + 1)*scale - 1, histHeight - intensity_blue), Scalar(0, 0, 255));
	}

	imshow("图像的RGB直方图;", histImage);
	*/

	//--------------------------直方图对比main调用;------------------------------------
	/*
	//声明存储基准图像和另外两张对比图像的矩阵(RGB和HSV);
	Mat srcImage_base, hsvImage_base;
	Mat srcImage_test1, hsvImage_test1;
	Mat srcImage_test2, hsvImage_test2;
	Mat hsvImage_halfDown;

	srcImage_base = imread("c_1.jpg", 1);
	srcImage_test1 = imread("c_2.jpg", 1);
	srcImage_test2 = imread("c_3.jpg", 1);

	imshow("基准图像;", srcImage_base);
	imshow("测试图像1;", srcImage_test1);
	imshow("测试图像2;", srcImage_test2);

	//RGB转HSV;
	cvtColor(srcImage_base, hsvImage_base, COLOR_BGR2HSV);
	cvtColor(srcImage_test1, hsvImage_test1, COLOR_BGR2HSV);
	cvtColor(srcImage_test2, hsvImage_test2, COLOR_BGR2HSV);

	//创建包含基准图像下半部的半身图像(HSV格式);
	hsvImage_halfDown = hsvImage_base(Range(hsvImage_base.rows / 2, hsvImage_base.rows - 1), Range(0, hsvImage_base.cols - 1));

	//初始化直方图的实参;
	//对hue通道使用30个bin，对saturation通道使用32个bin;
	int h_bins = 30; int s_bins = 32;
	int histSize[] = { h_bins,s_bins };
	float h_ranges[] = { 0,256 };
	float s_ranges[] = { 0,180 };
	const float* ranges[] = { h_ranges,s_ranges };
	//使用第0和第1通道;
	int channels[] = { 0,1 };

	//创建直方图类实例;
	MatND baseHist;
	MatND halfDownHist;
	MatND testHist1;
	MatND testHist2;

	//计算直方图;
	calcHist(&hsvImage_base, 1, channels, Mat(), baseHist, 2, histSize, ranges, true, false);
	normalize(baseHist, baseHist, 0, 1, NORM_MINMAX, -1, Mat());

	calcHist(&hsvImage_halfDown, 1, channels, Mat(), halfDownHist, 2, histSize, ranges, true, false);
	normalize(halfDownHist, halfDownHist, 0, 1, NORM_MINMAX, -1, Mat());

	calcHist(&hsvImage_test1, 1, channels, Mat(), testHist1, 2, histSize, ranges, true, false);
	normalize(testHist1, testHist1, 0, 1, NORM_MINMAX, -1, Mat());

	calcHist(&hsvImage_test2, 1, channels, Mat(), testHist2, 2, histSize, ranges, true, false);
	normalize(testHist2, testHist2, 0, 1, NORM_MINMAX, -1, Mat());

	//按顺序使用4种对比标准将基准图像的直方图与其余各直方图进行对比;
	for (int i = 0; i < 4; i++) {
		int compare_method = i;
		double base_base = compareHist(baseHist, baseHist, compare_method);
		double base_half = compareHist(baseHist, halfDownHist, compare_method);
		double base_test1 = compareHist(baseHist, testHist1, compare_method);
		double base_test2 = compareHist(baseHist, testHist2, compare_method);

		cout << "方法[" << compare_method << "]的匹配结果如下：" << endl << endl;
		cout << "【基准图-基准图】：" << base_base << "，【基准图-半身图】：" << base_half
			<< "，【基准图-测试图1】：" << base_test1 << "，【基准图-测试图2】：" << base_test2 << endl;

		cout << "-----------------------------------------------------------------------------" << endl;
	}
	*/
#endif // HISTOGRAM

//--------------------------SIFT的main调用;----------------------------------------
#if SIFT_AND_BFMATCHER	
	//Mat srcImage1 = imread("ukbench00000.jpg", 2 | 4);
	//Mat srcImage2 = imread("ukbench00001.jpg", 2 | 4);
	Mat srcImage1 = imread(argv[1], 2 | 4);
	Mat srcImage2 = imread(argv[2], 2 | 4);

	//SIFT特征检测;
	SiftFeatureDetector siftdtc;
	vector<KeyPoint>kp1, kp2;

	siftdtc.detect(srcImage1, kp1);
	Mat outImage1;
	drawKeypoints(srcImage1, kp1, outImage1);
	imshow("srcImage1 keypoints", outImage1);
	KeyPoint kp;

	vector<KeyPoint>::iterator itvc;
	for (itvc = kp1.begin(); itvc != kp1.end(); itvc++) 
		cout << "angle:" << itvc->angle << '\t' << itvc->class_id << '\t' << itvc->octave << "\t\t" << itvc->pt << '\t' << itvc->response << endl;
	
	siftdtc.detect(srcImage2, kp2);
	Mat outImage2;
	drawKeypoints(srcImage2, kp2, outImage2);
	imshow("srcImage2 keypoints", outImage2);

	SiftDescriptorExtractor extractor;
	Mat descriptor1, descriptor2;
	BruteForceMatcher<L2<float>> matcher;
	vector<DMatch> matches;
	Mat imgMatches;
	extractor.compute(srcImage1, kp1, descriptor1);
	extractor.compute(srcImage2, kp2, descriptor2);

	imshow("descriptor", descriptor1);
	cout << endl << descriptor1 << endl;
	matcher.match(descriptor1, descriptor2, matches);

	drawMatches(srcImage1, kp1, srcImage2, kp2, matches, imgMatches);
	imshow("matches", imgMatches);
#endif // SIFT_AND_BFMATCHER

//--------------------------SURF的main调用;----------------------------------------
#if SURF_AND_BFMATCHER	
	//改变console字体颜色;
	system("color 2F");

	//Mat srcImage1 = imread("Bookarrival_Original_view1.bmp", 2 | 4);
	//Mat srcImage2 = imread("Bookarrival_Original_view4.bmp", 2 | 4);
	Mat srcImage1 = imread(argv[1], 2 | 4);
	Mat srcImage2 = imread(argv[2], 2 | 4);

	if (!srcImage1.data || !srcImage2.data)	{
		cout << "Fail to read images" << endl;
		return false;
	}
	imshow("原始图1;", srcImage1);
	imshow("原始图2;", srcImage2);

	//定义需要用到的变量和类;
	int minHessian = 700;//定义SURF中hessian阈值特征点检测算子;
	SurfFeatureDetector detctor(minHessian);//特征检测类对象;
	std::vector<KeyPoint> keypoints_1, keypoints_2;//vector模板类能够存放任意类型的动态数组，能够增加和压缩数据;

	//调用detect函数检测出SURF特征关键点，保存在vector容器中;
	detctor.detect(srcImage1, keypoints_1);
	detctor.detect(srcImage2, keypoints_2);

	//绘制关键特征点;
	Mat img_keypoints_1, img_keypoints_2;
	drawKeypoints(srcImage1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	drawKeypoints(srcImage2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

	//显示效果图;
	imshow("特征点检测效果图1;", img_keypoints_1);
	imshow("特征点检测效果图2;", img_keypoints_2);

	//计算描述符(特征向量);
	SurfDescriptorExtractor extractor;
	Mat descriptors1, descriptors2;
	extractor.compute(srcImage1, keypoints_1, descriptors1);
	extractor.compute(srcImage2, keypoints_2, descriptors2);

	//使用BruteForce进行匹配;
	BruteForceMatcher<L2<float>> matcher;//实例化;
	std::vector<DMatch> matches;
	matcher.match(descriptors1, descriptors2, matches);//匹配两幅图中的描述子;

	//绘制从两个图像中匹配的关键点;
	Mat imgMatches;
	drawMatches(srcImage1, keypoints_1, srcImage2, keypoints_2, matches, imgMatches);

	//显示效果图;
	imshow("匹配图;", imgMatches);
#endif // SURF_AND_BFMATCHER

//--------------------------FLANN的main调用;---------------------------------------
#if SURF_AND_FLANN
	//改变console字体颜色;
	system("color 2F");

	//Mat srcImage1 = imread("ukbench00000.jpg", 2 | 4);
	//Mat srcImage2 = imread("ukbench00001.jpg", 2 | 4);

	Mat srcImage1 = imread(argv[1], 2 | 4);
	Mat srcImage2 = imread(argv[2], 2 | 4);

	if (!srcImage1.data || !srcImage2.data) {
		cout << "Fail to read images" << endl;
		return false;
	}
	imshow("原始图1;", srcImage1);
	imshow("原始图2;", srcImage2);

	//定义需要用到的变量和类;
	//int minHessian = 700;//定义SURF中hessian阈值特征点检测算子;
	int minHessian = atoi(argv[3]);

	SurfFeatureDetector detctor(minHessian);//特征检测类对象;
	std::vector<KeyPoint> keypoints_1, keypoints_2;//vector模板类能够存放任意类型的动态数组，能够增加和压缩数据;

	//调用detect函数检测出SURF特征关键点，保存在vector容器中;
	detctor.detect(srcImage1, keypoints_1);
	detctor.detect(srcImage2, keypoints_2);

	//计算描述符(特征向量);
	SurfDescriptorExtractor extractor;
	Mat descriptors1, descriptors2;
	extractor.compute(srcImage1, keypoints_1, descriptors1);
	extractor.compute(srcImage2, keypoints_2, descriptors2);

	//采用FLANN算法匹配特征向量;
	FlannBasedMatcher matcher;
	std::vector<DMatch> matches;
	matcher.match(descriptors1, descriptors2, matches);

	//快速计算关键点之间的最大和最小距离;
	double max_dist = 0;
	double min_dist = 100;
	for (int i = 0; i < descriptors1.rows; i++) {
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	//输出距离信息;
	cout << "最大距离(Max dist);" << max_dist << endl;
	cout << "最小距离(Min dist);" << min_dist << endl;

	//存下符合条件的匹配结果(即距离小于2*min_dist的)，使用radiusMatch同样可行;
	std::vector<DMatch> good_matches;
	for (int i = 0; i < descriptors1.rows; i++) {
		if (matches[i].distance < 2 * min_dist)
			good_matches.push_back(matches[i]);
	}

	//绘制从两个图像中匹配的关键点;
	Mat imgMatches;
	drawMatches(srcImage1, keypoints_1, srcImage2, keypoints_2, good_matches, imgMatches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//输出所有匹配点信息;
	for (int i = 0; i < good_matches.size(); i++) {
		cout << ">符合条件的匹配点;" << i << "特征点1;" << good_matches[i].queryIdx << " --特征点2;" << good_matches[i].trainIdx << endl;
	}

	//显示效果图;
	imshow("匹配图;", imgMatches);
	//imwrite("匹配图.png", imgMatches);
#endif // SURF_AND_FLANN

//--------------------------基于FLANN的寻找已知物体main调用;-----------------------
#if SURF_AND_FLANN_AND_OBJECT_DETECTION
	//改变console字体颜色;
	system("color 2F");

	//Mat srcImage1 = imread("Bookarrival_Original_view1.bmp", 2 | 4);
	//Mat srcImage2 = imread("Bookarrival_Original_view4.bmp", 2 | 4);

	Mat srcImage1 = imread(argv[1], 2 | 4);
	Mat srcImage2 = imread(argv[2], 2 | 4);

	if (!srcImage1.data || !srcImage2.data) {
		cout << "Fail to read images" << endl;
		return false;
	}
	imshow("原始图1;", srcImage1);
	imshow("原始图2;", srcImage2);

	//使用SURF算子检测关键点;
	int minHessian = 400;//SURF算法中的hessian阈值;
	SurfFeatureDetector detector(minHessian);//定义一个SurfFeatureDetector特征检测类对象;
	vector<KeyPoint> keypoints_object, keypoints_scene;//vector模板类，可以存放任意类型的动态数组;

	//调用detect函数检测出SURF特征关键点，保存在vector容器中;
	detector.detect(srcImage1, keypoints_object);
	detector.detect(srcImage2, keypoints_scene);

	//计算描述符(特征向量);
	SurfDescriptorExtractor extractor;
	Mat descriptors_object, descriptors_scene;
	extractor.compute(srcImage1, keypoints_object, descriptors_object);
	extractor.compute(srcImage2, keypoints_scene, descriptors_scene);

	//使用FLANN匹配算子进行匹配;
	FlannBasedMatcher matcher;
	vector<DMatch> matches;
	matcher.match(descriptors_object, descriptors_scene, matches);

	//快速计算关键点之间的最大和最小距离;
	double max_dist = 0;
	double min_dist = 100;
	for (int i = 0; i < descriptors_object.rows; i++) {
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	//输出距离信息;
	cout << "最大距离(Max dist);" << max_dist << endl;
	cout << "最小距离(Min dist);" << min_dist << endl;

	//存下匹配距离小于3*min_dist的点对;
	std::vector<DMatch> good_matches;
	for (int i = 0; i < descriptors_object.rows; i++) {
		if (matches[i].distance < 3 * min_dist)
			good_matches.push_back(matches[i]);
	}

	//绘制从两个图像中匹配的关键点;
	Mat imgMatches;
	drawMatches(srcImage1, keypoints_object, srcImage2, keypoints_scene, good_matches, imgMatches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//从匹配成功的匹配对中获取关键点;
	vector<Point2f> obj, scene;
	for (unsigned int i = 0; i < good_matches.size(); i++) {
		obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}

	//计算透视变换矩阵;
	Mat H = findHomography(obj, scene, CV_RANSAC);

	//从待测图片中获取角点;
	vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0);
	obj_corners[1] = cvPoint(srcImage1.cols, 0);
	obj_corners[2] = cvPoint(srcImage1.cols, srcImage1.rows);
	obj_corners[3] = cvPoint(0, srcImage1.rows);

	//进行透视变换;
	vector<Point2f> scene_corners(4);
	perspectiveTransform(obj_corners, scene_corners, H);

	//绘出角点之间的直线;
	line(imgMatches,
		scene_corners[0] + Point2f(static_cast<float>(srcImage1.cols), 0),
		scene_corners[1] + Point2f(static_cast<float>(srcImage1.cols), 0),
		Scalar(255, 0, 123), 4
	);
	line(imgMatches,
		scene_corners[1] + Point2f(static_cast<float>(srcImage1.cols), 0),
		scene_corners[2] + Point2f(static_cast<float>(srcImage1.cols), 0),
		Scalar(255, 0, 123), 4
	);
	line(imgMatches,
		scene_corners[2] + Point2f(static_cast<float>(srcImage1.cols), 0),
		scene_corners[3] + Point2f(static_cast<float>(srcImage1.cols), 0),
		Scalar(255, 0, 123), 4
	);
	line(imgMatches,
		scene_corners[3] + Point2f(static_cast<float>(srcImage1.cols), 0),
		scene_corners[0] + Point2f(static_cast<float>(srcImage1.cols), 0),
		Scalar(255, 0, 123), 4
	);

	//显示最终结果;
	imshow("Good Matches & Object Detection", imgMatches);
#endif // SURF_AND_FLANN_AND_OBJECT_DETECTION

//--------------------------基于FLANN的寻找已知物体main调用;-----------------------
#if ORB_AND_FLANN
	//改变console字体颜色;
	system("color 2F");

	//Mat srcImage1 = imread("ukbench00000.jpg", 2 | 4);
	//Mat srcImage2 = imread("ukbench00001.jpg", 2 | 4);

	Mat srcImage1 = imread(argv[1], 2 | 4);
	Mat srcImage2 = imread(argv[2], 2 | 4);

	if (!srcImage1.data || !srcImage2.data) {
		cout << "Fail to read images" << endl;
		return false;
	}
	imshow("原始图1;", srcImage1);
	imshow("原始图2;", srcImage2);

	Mat gryImage1, gryImage2;
	cvtColor(srcImage1, gryImage1, CV_BGR2GRAY);
	cvtColor(srcImage2, gryImage2, CV_BGR2GRAY);

	//参数定义;
	OrbFeatureDetector featureDetector;
	vector<KeyPoint> KeyPoints1, KeyPoints2;
	Mat descriptors1, descriptors2;

	//调用detect函数检测特征关键点，保存在vector容器中;
	featureDetector.detect(gryImage1, KeyPoints1);
	featureDetector.detect(gryImage2, KeyPoints2);

	//计算描述符(特征向量);
	OrbDescriptorExtractor featureExtractor;
	featureExtractor.compute(gryImage1, KeyPoints1, descriptors1);
	featureExtractor.compute(gryImage2, KeyPoints2, descriptors2);

	//基于FLANN的描述符对象匹配;
	flann::Index flannIndex(descriptors1, flann::LshIndexParams(12, 20, 2), cvflann::FLANN_DIST_HAMMING);
	Mat matchIndex(descriptors2.rows, 2, CV_32SC1);
	Mat matchDistance(descriptors2.rows, 2, CV_32FC1);
	flannIndex.knnSearch(descriptors2, matchIndex, matchDistance, 2, flann::SearchParams());//调用K近邻算法;

	//根据劳氏算法;(Lowe's algorithm)选出优秀的匹配;
	vector<DMatch> goodMatches;
	for (int i = 0; i < matchDistance.rows; i++) {
		if (matchDistance.at<float>(i, 0) < 0.6 * matchDistance.at<float>(i, 1)) {
			DMatch dmatches(i, matchIndex.at<int>(i, 0), matchDistance.at<float>(i, 0));
			goodMatches.push_back(dmatches);
		}
	}

	//绘制并显示匹配窗口;
	Mat resultImage;
	drawMatches(srcImage2, KeyPoints2, srcImage1, KeyPoints1, goodMatches, resultImage);
	imshow("匹配窗口;", resultImage);
	imwrite("匹配图.png", resultImage);

#endif // ORB_AND_FLANN

//--------------------------腐蚀操作的main调用;------------------------------------
#if ERODE
	Mat src = imread("1.jpg", 2 | 4);
	//getStructuringElement函数返回的是指定形状和尺寸的结构元素;
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	Mat dst;
	erode(src, dst, element);
	Mat merge;
	mergeImg(merge, src, dst);
	imshow("腐蚀效果图;", merge);
#endif // ERODE

//--------------------------膨胀操作的main调用;------------------------------------
#if DILATE
	Mat src = imread("1.jpg", 2 | 4);
	//getStructuringElement函数返回的是指定形状和尺寸的结构元素;
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	Mat dst;
	dilate(src, dst, element);
	Mat merge;
	mergeImg(merge, src, dst);
	imshow("膨胀效果图;", merge);
#endif // DILATE

//--------------------------高级形态学处理的main调用;------------------------------
#if ADVANCED_MORPHOLOGY
	Mat src = imread("1.jpg", 2 | 4);
	//getStructuringElement函数返回的是指定形状和尺寸的结构元素;
	Mat element = getStructuringElement(MORPH_RECT, Size(7, 7));
	Mat dst;

	//第3个参数修改哪种处理;
	//MORPH_OPEN 开运算;
	//MORPH_CLOSE 闭运算;
	//MORPH_GRADIENT 形态学梯度;
	//MORPH_TOPHAT 顶帽;
	//MORPH_BLACKHAT 黑帽;
	//MORPH_ERODE 腐蚀（更暗）;
	//MORPH_DILATE 膨胀（更亮）;
	morphologyEx(src, dst, MORPH_DILATE, element);
	Mat merge;
	mergeImg(merge, src, dst);
	imshow("高级形态学处理效果图;", merge);
#endif // ADVANCED_MORPHOLOGY

//--------------------------图像缩放的main调用;------------------------------------
#if RESIZE
	Mat src = imread("1.jpg", 2 | 4);
	imshow("原始图;", src);

	// 方法1 固定大小;
	Mat dst1 = Mat::zeros(512, 512, CV_8UC3);
	resize(src, dst1, dst1.size());
	imshow("调整尺寸后1;", dst1);

	// 方法2 按比例;
	Mat dst2;
	resize(src, dst2, Size(), 0.5, 0.5);
	imshow("调整尺寸后2;", dst2);

#endif // RESIZE

//--------------------------图像金字塔的main调用;----------------------------------
#if PYRAMID

	Mat src = imread("1.jpg", 2 | 4);
	imshow("原始图;", src);

	Mat dst1, dst2;
	pyrUp(src, dst1, Size(src.cols * 2, src.rows * 2)); // 放大一倍;
	pyrDown(src, dst2, Size(src.cols*0.5, src.rows*0.5)); // 缩小一倍;

	imshow("尺寸放大之后;", dst1);
	imshow("尺寸缩小之后;", dst2);

#endif // PYRAMID

//--------------------------YUV相关的main调用;-------------------------------------
#if YUV_CLASS
	//输入所有文件名;
	vector<string> allPicname;
	for (int i = 1; i < argc - 1; ++i) {
		allPicname.push_back(argv[i]);
	}

	//输出名;
	string outpicName = allPicname[0];
	for (int i = 0; i < 4; i++) {
		outpicName.pop_back();
	}
	outpicName += ".bmp";
	
	//读取文件流;
	CIYuv yuvBuffer;
	FILE *fin_yuv;
	if ((fin_yuv = fopen(allPicname[0].c_str(), "rb")) == NULL) {
		fprintf(stderr, "Can't open input file(s)\n");
		return 3;
	}

	//设置YUV序列参数;
	if (!yuvBuffer.Resize(768, 1024, 420))
		return 2;

	//读取设定帧;
	/*
	int num = atoi(argv[2]);
	if (!yuvBuffer.ReadOneFrame(fin_yuv, num)){
		return 2;
	}

	IplImage *bgr = cvCreateImage(cvSize(1024, 768), 8, 3);
	yuvBuffer.getData_inBGR(bgr);

	cv::Mat matimg;
	matimg = cv::Mat(bgr);
	imshow("输出图;", matimg);
	//imwrite(outpicName, matimg);
	*/

	//求背景(求和取平均);
	/*
	cv::Mat bg(768, 1024, CV_32FC3, Scalar(0, 0, 0));
	
	for (int i = 0; i < 5; i++) {
		if (!yuvBuffer.ReadOneFrame(fin_yuv, i)) {
			return 2;
		}
		IplImage *bgr = cvCreateImage(cvSize(1024, 768), 8, 3);
		yuvBuffer.getData_inBGR(bgr);

		cv::Mat matimg;
		matimg = cv::Mat(bgr);

		matimg.convertTo(matimg, CV_32FC3, 1.0 / 255, 0);

		bg += matimg;
		//imshow("输出图;", matimg);
		//waitKey(30);
	}

	bg /= 5;
	bg.convertTo(bg, CV_8UC3, 255, 0);
	imshow("输出图;", bg);
	imwrite("ARV_5.bmp", bg);
	*/

	//求背景(GMM);
	/*
	cv::BackgroundSubtractorMOG2 mog;
	cv::Mat foreground,bg;

	for (int i = 0; i < 5; i++) {
		if (!yuvBuffer.ReadOneFrame(fin_yuv, i)) {
			return 2;
		}
		IplImage *bgr = cvCreateImage(cvSize(1024, 768), 8, 3);
		yuvBuffer.getData_inBGR(bgr);

		cv::Mat matimg;
		matimg = cv::Mat(bgr);
		
		//cvtColor(matimg, matimg, CV_BGR2GRAY);
		mog(matimg, foreground, 0.01);
	}

	mog.getBackgroundImage(bg);
	cv::imshow("掩膜图;", bg);
	imwrite("GMM_5.bmp", bg);
	*/

	//按背景填充单视点空洞(针对单视点运动物体边缘空洞修复);
	Mat bg = imread("GMM_20.bmp", 2 | 4);

	for (int frame = 0; frame < 50; frame++) {
		if (!yuvBuffer.ReadOneFrame(fin_yuv, frame)) {
			return 2;
		}
		IplImage *bgr = cvCreateImage(cvSize(1024, 768), 8, 3);
		yuvBuffer.getData_inBGR(bgr);

		cv::Mat matimg;
		matimg = cv::Mat(bgr);

		Mat src;
		matimg.copyTo(src);

		for (int i = 0; i < matimg.rows; i++) {
			uchar* data1 = matimg.ptr<uchar>(i);
			uchar* data2 = bg.ptr<uchar>(i);
			for (int j = 0; j < matimg.cols; j++) {
				if (data1[j * 3] <= 10 && data1[j * 3 + 1] <= 10 && data1[j * 3 + 2] <= 10) {
					data1[j * 3] = data2[j * 3];
					data1[j * 3 + 1] = data2[j * 3 + 1];
					data1[j * 3 + 2] = data2[j * 3 + 2];
					//data1[j * 3] = 255;
					//data1[j * 3 + 1] = 255;
					//data1[j * 3 + 2] = 255;
				}
			}
		}
		//Mat dst;
		//mergeImg(dst, src, matimg);

		//imshow("对比图;", dst);

		imshow("原始未处理;", src);
		imshow("处理后;", matimg);
		waitKey();
	}

	//关闭流;
	fclose(fin_yuv);
#endif // YUV_CLASS

//--------------------------Mat类的main调用;---------------------------------------
/*
	Mat img1(2, 2, CV_8UC3, Scalar(0, 0, 255));
	cout << img1 << endl;

	Mat img2;
	img2.create(4, 4, CV_8UC3);
	cout << img2 << endl;

	Mat img3 = Mat::zeros(4, 4, CV_8UC3);
	cout << img3 << endl;

	Mat img4 = img3.clone();
	cout << img4 << endl;

	Mat img5;
	img4.copyTo(img5);
	cout << img5 << endl;

	Mat img6 = img5;
	Mat img7(img6);

	cout << "img6" << img6 << endl << "img7" << img7 << endl;

	img5 = Mat::ones(4, 4, CV_8UC3);

	cout << "img6" << img6 << endl << "img7" << img7 << endl;
*/

//--------------------------二值化的main调用;--------------------------------------
#if BINARYZATION
	Mat image = imread("1.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	//全局二值化;
	int th = 25;
	Mat global;
	threshold(image, global, th, 255, CV_THRESH_BINARY_INV);

	//局部二值化;
	int blockSize = 25;
	int constValue = 10;
	Mat local;
	adaptiveThreshold(image, local, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, blockSize, constValue);

	imshow("全局二值化;", global);
	imshow("局部二值化;", local);
#endif // BINARYZATION

//--------------------------均值滤波的main调用;------------------------------------
#if MEAN_FILTERING
	Mat src = imread("1.jpg", 2 | 4);
	Mat dst;
	blur(src, dst, Size(7, 7));

	Mat merge;
	mergeImg(merge, src, dst);

	imshow("均值模糊效果图;", merge);

#endif // MEAN_FILTERING

//--------------------------Canny边缘检测的main调用;-------------------------------
#if CANNY_EDGE_DETECTION
	Mat src = imread("1.jpg", 2 | 4);

	Mat dst, edge, grayImage;
	cvtColor(src, grayImage, COLOR_BGR2GRAY);

	//先使用3*3内核来降噪;
	blur(grayImage, edge, Size(3, 3));

	//运行canny算子;
	Canny(edge, dst, 30, 90);
	imshow("边缘提取效果图;", dst);

#endif // CANNY_EDGE_DETECTION

//--------------------------分水岭分割的main调用;----------------------------------
#if WATERSHED
	Mat image = imread("test.jpg", 2 | 4);
	imshow("Source Image", image);

	//灰度化，滤波，Canny边缘检测;
	Mat imageGray;
	cvtColor(image, imageGray, CV_RGB2GRAY);//灰度转换;
	
	GaussianBlur(imageGray, imageGray, Size(5, 5), 2);//高斯滤波;

	imshow("Gray Image", imageGray);

	//Canny(imageGray, imageGray, 80, 150);
	Canny(imageGray, imageGray, 60, 140);

	imshow("Canny Image", imageGray);

	//查找轮廓;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(imageGray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());
	Mat imageContours = Mat::zeros(image.size(), CV_8UC1);  //轮廓;
	Mat marks(image.size(), CV_32S);   //Opencv分水岭第二个矩阵参数;
	marks = Scalar::all(0);
	int index = 0;
	int compCount = 0;
	for (; index >= 0; index = hierarchy[index][0], compCount++){
		//对marks进行标记，对不同区域的轮廓进行编号，相当于设置注水点，有多少轮廓，就有多少注水点  
		drawContours(marks, contours, index, Scalar::all(compCount + 1), 1, 8, hierarchy);
		drawContours(imageContours, contours, index, Scalar(255), 1, 8, hierarchy);
	}

	//我们来看一下传入的矩阵marks里是什么东西;
	Mat marksShows;
	convertScaleAbs(marks, marksShows);
	imshow("marksShow", marksShows);
	imshow("轮廓;", imageContours);
	watershed(image, marks);

	//我们再来看一下分水岭算法之后的矩阵marks里是什么东西;
	Mat afterWatershed;
	convertScaleAbs(marks, afterWatershed);
	imshow("After Watershed", afterWatershed);

	//对每一个区域进行颜色填充;
	Mat PerspectiveImage = Mat::zeros(image.size(), CV_8UC3);
	for (int i = 0; i < marks.rows; i++){
		for (int j = 0; j < marks.cols; j++){
			int index = marks.at<int>(i, j);
			if (marks.at<int>(i, j) == -1)
				PerspectiveImage.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
			else
				PerspectiveImage.at<Vec3b>(i, j) = RandomColor(index);
		}
	}
	imshow("After ColorFill", PerspectiveImage);

	//分割并填充颜色的结果跟原始图像融合;
	Mat wshed;
	addWeighted(image, 0.6, PerspectiveImage, 0.4, 0, wshed);
	imshow("AddWeighted Image", wshed);

#endif // WATERSHED

//--------------------------图像修复的main调用;------------------------------------
#if INPAINT
	Mat srcImage = imread("1.jpg", 2 | 4);
	srcImage1 = srcImage.clone();
	inpaintMask = Mat::zeros(srcImage.size(), CV_8U);

	imshow("原始图;", srcImage1);

	//设置鼠标回调消息;
	setMouseCallback("原始图;", On_Mouse, 0);

	//轮询按键，根据不同的按键进行处理;
	while (1) {
		//获取按键键值;
		char c = (char)waitKey();

		//键值为ESC，程序退出;
		if (c == 27)
			break;

		//键值为2，恢复成原始图像;
		if (c == '2') {
			inpaintMask = Scalar::all(0);
			srcImage.copyTo(srcImage1);
			imshow("原始图;", srcImage1);
		}

		//键值为1或者空格，进行图像修补操作;
		if (c == '1' || c == ' ') {
			Mat inpaintedImage;
			inpaint(srcImage1, inpaintMask, inpaintedImage, 3, INPAINT_TELEA); //此处第5个参数可以是 INPAINT_NS 或者 INPAINT_TELEA;
			imshow("修补后的效果图;", inpaintedImage);
		}
	}
	
#endif // INPAINT
	
#if MYINPAINT
	cv::Mat srcImage = cv::imread(argv[1], 2 | 4);
	//cv::imshow("srcImage", srcImage);

	cv::Mat inpaintMask = cv::imread(argv[2], 2 | 4);
	cv::Mat inpaintMask1D = cv::Mat::zeros(srcImage.size(), CV_8UC1);
	for (int h = 0; h < inpaintMask.rows; h++) {
		uchar* srcLine = inpaintMask.ptr<uchar>(h);
		uchar* mskLine = inpaintMask1D.ptr<uchar>(h);
		for (int w = 0; w < inpaintMask.cols; w++)
			if (srcLine[3 * w] >= 250 && srcLine[3 * w + 1] == 255 && srcLine[3 * w + 2] == 255)
				mskLine[w] = 255;
	}
	/*cv::Mat inpaintMask = cv::Mat::zeros(srcImage.size(), CV_8UC1);

	for (int h = 0; h < srcImage.rows;h++) {
		uchar* srcLine = srcImage.ptr<uchar>(h);
		uchar* mskLine = inpaintMask.ptr<uchar>(h);
		for (int w = 0; w < srcImage.cols; w++)
			if (srcLine[3 * w] == 2 && srcLine[3 * w + 1] == 0 && srcLine[3 * w + 2] == 1)
				mskLine[w] = 255;
	}*/

	//cv::imshow("Mask", inpaintMask);

	cv::Mat inpaintedImage;
	inpaint(srcImage, inpaintMask1D, inpaintedImage, 10, INPAINT_NS);
	cv::imwrite("TARGET1.bmp", inpaintedImage);
	//cv::imshow("Output", inpaintedImage);

#endif // MYINPAINT

/*
	Mat src = imread("1.jpg", 2 | 4);

	Msrcr msrcr;

	vector<double> sigema;
	vector<double> weight;
	for (int i = 0; i < 3; i++)
		weight.push_back(1. / 3);
	// 由于MSRCR.h中定义了宏USE_EXTRA_SIGMA，所以此处的vector<double> sigema并没有什么意义
	sigema.push_back(30);
	sigema.push_back(150);
	sigema.push_back(300);


	Mat dst;
	msrcr.MultiScaleRetinexCR(src, dst, weight,sigema,100,100);
	cv::imshow("dst", dst);
*/
	waitKey(0);
	return 1;
}

