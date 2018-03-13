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

//--------------------------ȫ�ֺ�������;--------------------------------

# define HISTOGRAM 0

# define SIFT_AND_BFMATCHER 0
# define SURF_AND_BFMATCHER 0
# define SURF_AND_FLANN 0
# define SURF_AND_FLANN_AND_OBJECT_DETECTION 0
# define ORB_AND_FLANN 0

# define ERODE 0
# define DILATE 0
# define ADVANCED_MORPHOLOGY 0 //�߼���̬ѧ����;
# define RESIZE 0
# define PYRAMID 0
# define BINARYZATION 0
# define MEAN_FILTERING 0
# define CANNY_EDGE_DETECTION 0


# define YUV_CLASS 0

# define WATERSHED 0
# define INPAINT 0

//--------------------------ȫ�ֺ�����������;----------------------------
static void on_ContrastAndBright(int, void *);

//--------------------------ȫ�ֱ�����������;----------------------------
int g_nContrastValue; //�Աȶ�ֵ;
int g_nBrightValue; //����ֵ;
Mat g_srcImage, g_dstImage;

//--------------------------ȫ�ֺ���ʵ�ֲ���;----------------------------
//�������ı�ͼ��ԱȶȺ�����ֵ�Ļص�����;
static void on_ContrastAndBright(int, void *) {

	//��������;
	namedWindow("��ԭʼͼ���ڡ�;", 1);

	//����forѭ����ִ������g_dstImage(i,j) = a*g_srcImage(i,j) + b;
	for (int y = 0; y < g_srcImage.rows; y++) {
		for (int x = 0; x < g_srcImage.cols; x++) {
			for (int c = 0; c < 3; c++) {
				//g_dstImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>((g_nContrastValue*0.01)*(g_srcImage.at<Vec3b>(y, x)[c]) + g_nBrightValue);
				g_dstImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>((g_srcImage.at<Vec3b>)(y, x)[c] + g_nBrightValue - 255);
			}
		}
	}

	imshow("��ԭʼͼ���ڡ�;", g_srcImage);
	imshow("��Ч��ͼ���ڡ�;", g_dstImage);
}

//������������ʾ����ͼ��;
void mergeImg(Mat &dst, Mat &src1, Mat &src2) {
	int rows = max(src1.rows, src2.rows);
	int cols = src1.cols + src2.cols;
	dst.create(rows, cols, src1.type());
	src1.copyTo(dst(Rect(0, 0, src1.cols, src1.rows)));
	src2.copyTo(dst(Rect(src1.cols, 0, src2.cols, src2.rows)));
}

//���������������ɫ����;
Vec3b RandomColor(int value)
{
	value = value % 255;  //����0~255�������  
	RNG rng;
	int aa = rng.uniform(0, value);
	int bb = rng.uniform(0, value);
	int cc = rng.uniform(0, value);
	return Vec3b(aa, bb, cc);
}

//��������Ӧ�����Ϣ�Ļص�����;
Mat srcImage1, inpaintMask;
Point previousPoint(-1, -1); //ԭ���ĵ�����;
static void On_Mouse(int event, int x, int y, int flags, void*) {
	//������������Ϣ;
	if (event == EVENT_LBUTTONUP || !(flags & EVENT_FLAG_LBUTTON))
		previousPoint = Point(-1, -1);
	//������������Ϣ;
	else if (event == EVENT_LBUTTONDOWN)
		previousPoint = Point(x, y);
	//��갴�²��ƶ������л���;
	else if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON)) {
		Point pt(x, y);
		if (previousPoint.x < 0)
			previousPoint = pt;
		//���ư�ɫ����;
		line(inpaintMask, previousPoint, pt, Scalar::all(255), 5, 8, 0);
		line(srcImage1, previousPoint, pt, Scalar::all(255), 5, 8, 0);
		previousPoint = pt;
		imshow("ԭʼͼ;", srcImage1);
	}
}

int main(int argc, char ** argv) {

	//д������;
	vector<int>compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

//--------------------------ֱ��ͼ��main����;--------------------------------------
#if HISTOGRAM
	//--------------------------���ȡ��Աȶȵ���main����;------------------------------
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

	namedWindow("��Ч��ͼ���ڡ�;", 1);

	//�����켣��;
	createTrackbar("�Աȶȣ�;", "��Ч��ͼ���ڡ�;", &g_nContrastValue, 300, on_ContrastAndBright);
	createTrackbar("���ȣ�;", "��Ч��ͼ���ڡ�;", &g_nBrightValue, 512, on_ContrastAndBright);

	//init;
	on_ContrastAndBright(g_nContrastValue, 0);
	on_ContrastAndBright(g_nBrightValue, 0);

	while (char(waitKey(1)) != 'q') {}
	*/

	//--------------------------ֱ��ͼ���⻯(�Ҷ�)main����;----------------------------
	/*
	Mat srcImage, dstImage;
	srcImage = imread("1.jpg", 1);
	if (!srcImage.data) { printf("��ȡͼƬ������ȷ��Ŀ¼���Ƿ���imread����ָ��ͼƬ����~��; \n"); return false; }

	cvtColor(srcImage, srcImage, CV_BGR2GRAY);
	imshow("ԭʼͼ;", srcImage);

	equalizeHist(srcImage, dstImage);

	imshow("����ֱ��ͼ���⻯���ͼ;", dstImage);
	*/

	//--------------------------ֱ��ͼ���⻯(��ɫ)main����;----------------------------
	
	Mat srcImage, dstImage, dstImageBlue, dstImageGreen, dstImageRed;
	srcImage = imread("1.jpg", 1);
	if (!srcImage.data) { printf("��ȡͼƬ������ȷ��Ŀ¼���Ƿ���imread����ָ��ͼƬ����~��; \n"); return false; }

	imshow("ԭʼͼ;", srcImage);
	
	vector<Mat> channels;
	Mat imageBlueChannel;
	Mat imageGreenChannel;
	Mat imageRedChannel;

	split(srcImage, channels);
	imageBlueChannel = channels.at(0);
	imageGreenChannel = channels.at(1);
	imageRedChannel = channels.at(2);

	imshow("ԭʼ��ͨ��;", imageBlueChannel);
	imshow("ԭʼ��ͨ��;", imageGreenChannel);
	imshow("ԭʼ��ͨ��;", imageRedChannel);

	equalizeHist(imageBlueChannel, imageBlueChannel);
	equalizeHist(imageGreenChannel, imageGreenChannel);
	equalizeHist(imageRedChannel, imageRedChannel);

	merge(channels, dstImage);

	imshow("������ͨ��;", imageBlueChannel);
	imshow("������ͨ��;", imageGreenChannel);
	imshow("�����ͨ��;", imageRedChannel);

	imshow("����ֱ��ͼ���⻯���ͼ;", dstImage);
	
	imwrite("out2.png", dstImage);
	

	//--------------------------����H-Sֱ��ͼmain����;---------------------------------
	/*
	Mat srcImage, hsvImage;
	srcImage = imread("1.jpg");
	cvtColor(srcImage, hsvImage, COLOR_BGR2HSV);

	//parameter
	//��ɫ������Ϊ30���ȼ��������Ͷ�����Ϊ32���ȼ�;
	int hueBinNum = 30;//ɫ��ֱ��ͼֱ������;
	int saturationBinNum = 32;//���Ͷ�ֱ��ͼֱ������;
	int histSize[] = { hueBinNum,saturationBinNum };
	float hueRanges[] = { 0, 180 };//����ɫ���ı仯��ΧΪ0��179;
	float saturationRanges[] = { 0,256 };//���履�Ͷȵı仯��ΧΪ0(�ڡ��ס���)��255(��������ɫ);
	const float* ranges[] = { hueRanges,saturationRanges };
	MatND dstHist;

	//����׼��,calcHist�����н������0ͨ���͵�1ͨ����ֱ��ͼ;
	int channels[] = { 0,1 };
	calcHist(&hsvImage,//���������;
		1,//�������Ϊ1;
		channels,//ͨ������;
		Mat(),//��ʹ����Ĥ;
		dstHist,//�����Ŀ��ֱ��ͼ;
		2,//��Ҫ�����ֱ��ͼ��ά��Ϊ2;
		histSize,//���ÿ��ά�ȵ�ֱ��ͼ�ߴ������;
		ranges,//ÿһά��ֵ��ȡֵ��Χ����;
		true,//ָʾֱ��ͼ�Ƿ���ȵı�ʶ����true��ʾ���ȵ�ֱ��ͼ;
		false//�ۼƱ�ʶ����false��ʾֱ��ͼ�����ý׶λᱻ����;
	);

	//����ֱ��ͼ׼������;
	double maxValue = 0;//���ֵ;
	minMaxLoc(dstHist, 0, &maxValue, 0, 0);//����������������ȫ����Сֵ�����ֵ����maxValue��;
	int scale = 10;
	Mat histImg = Mat::zeros(saturationBinNum * scale, hueBinNum * 10, CV_8UC3);

	//˫��ѭ��������ֱ��ͼ����;
	for (int hue = 0; hue < hueBinNum; hue++){
		for (int saturation = 0; saturation < saturationBinNum; saturation++) {
			float binValue = dstHist.at<float>(hue, saturation);//ֱ��ͼֱ����ֵ;
			int intensity = cvRound(binValue * 255 / maxValue);//ǿ��;

			//��ʽ���л���;
			rectangle(histImg, Point(hue*scale, saturation*scale), Point((hue + 1)*scale - 1, (saturation + 1)*scale - 1), Scalar::all(intensity),7);
		}
	}

	//��ʾЧ��ͼ;
	imshow("�ز�ͼ;", srcImage);
	imshow("H-Sֱ��ͼ;", histImg);
	*/

	//--------------------------����һάֱ��ͼmain����;--------------------------------
	/*
	Mat srcImage = imread("1.jpg", 0);
	imshow("ԭͼ;", srcImage);
	if (!srcImage.data) { cout << "Fail to Load Image" << endl; return 0; }

	//�������;
	MatND dstHist;
	int dims = 1;
	float hranges[] = { 0,255 };
	const float *range[] = { hranges };//������Ҫconst����;
	int size = 256;
	int channels = 0;

	//����ͼ���ֱ��ͼ;
	calcHist(&srcImage, 1, &channels, Mat(), dstHist, dims, &size, range);
	int scale = 1;

	Mat dstImage(size * scale, size, CV_8U, Scalar(0));

	//��ȡ���ֵ����Сֵ;
	double minValue = 0;
	double maxValue = 0;
	minMaxLoc(dstHist, &minValue, &maxValue, 0, 0);

	//����ֱ��ͼ;
	int hpt = saturate_cast<int>(0.9 * size);
	for (int i = 0; i < 256; i++) {
		float binValue = dstHist.at<float>(i);
		int realValue = saturate_cast<int>(binValue * hpt / maxValue);
		rectangle(dstImage, Point(i * scale, size - 1), Point((i + 1)*scale - 1, size - realValue), Scalar(255));
	}

	imshow("һάֱ��ͼ;", dstImage);
	*/

	//--------------------------����RGB��ɫֱ��ͼmain����;-----------------------------
	/*
	Mat srcImage = imread("1.jpg");
	imshow("ԭͼ;", srcImage);

	//����׼��;
	int bins = 256;
	int hist_size[] = { bins };
	float range[] = { 0,256 };
	const float* ranges[] = { range };
	MatND redHist, greenHist, blueHist;

	//ֱ��ͼ����;
	int channels_r[] = { 0 };
	calcHist(&srcImage, 1, channels_r, Mat(),//��ʹ����Ĥ;
		redHist, 1, hist_size, ranges, true, false);

	int channels_g[] = { 1 };
	calcHist(&srcImage, 1, channels_g, Mat(),//��ʹ����Ĥ;
		greenHist, 1, hist_size, ranges, true, false);

	int channels_b[] = { 1 };
	calcHist(&srcImage, 1, channels_b, Mat(),//��ʹ����Ĥ;
		blueHist, 1, hist_size, ranges, true, false);


	//������ɫֱ��ͼ;
	//����׼��;
	double maxValue_red, maxValue_green, maxValue_blue;
	minMaxLoc(redHist, 0, &maxValue_red, 0, 0);
	minMaxLoc(greenHist, 0, &maxValue_green, 0, 0);
	minMaxLoc(blueHist, 0, &maxValue_blue, 0, 0);
	int scale = 1;
	int histHeight = 256;
	Mat histImage = Mat::zeros(histHeight, bins * 3, CV_8UC3);

	//��ʼ����;
	for (int i = 0; i < bins; i++) {
		//����׼��;
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

	imshow("ͼ���RGBֱ��ͼ;", histImage);
	*/

	//--------------------------ֱ��ͼ�Ա�main����;------------------------------------
	/*
	//�����洢��׼ͼ����������ŶԱ�ͼ��ľ���(RGB��HSV);
	Mat srcImage_base, hsvImage_base;
	Mat srcImage_test1, hsvImage_test1;
	Mat srcImage_test2, hsvImage_test2;
	Mat hsvImage_halfDown;

	srcImage_base = imread("c_1.jpg", 1);
	srcImage_test1 = imread("c_2.jpg", 1);
	srcImage_test2 = imread("c_3.jpg", 1);

	imshow("��׼ͼ��;", srcImage_base);
	imshow("����ͼ��1;", srcImage_test1);
	imshow("����ͼ��2;", srcImage_test2);

	//RGBתHSV;
	cvtColor(srcImage_base, hsvImage_base, COLOR_BGR2HSV);
	cvtColor(srcImage_test1, hsvImage_test1, COLOR_BGR2HSV);
	cvtColor(srcImage_test2, hsvImage_test2, COLOR_BGR2HSV);

	//����������׼ͼ���°벿�İ���ͼ��(HSV��ʽ);
	hsvImage_halfDown = hsvImage_base(Range(hsvImage_base.rows / 2, hsvImage_base.rows - 1), Range(0, hsvImage_base.cols - 1));

	//��ʼ��ֱ��ͼ��ʵ��;
	//��hueͨ��ʹ��30��bin����saturationͨ��ʹ��32��bin;
	int h_bins = 30; int s_bins = 32;
	int histSize[] = { h_bins,s_bins };
	float h_ranges[] = { 0,256 };
	float s_ranges[] = { 0,180 };
	const float* ranges[] = { h_ranges,s_ranges };
	//ʹ�õ�0�͵�1ͨ��;
	int channels[] = { 0,1 };

	//����ֱ��ͼ��ʵ��;
	MatND baseHist;
	MatND halfDownHist;
	MatND testHist1;
	MatND testHist2;

	//����ֱ��ͼ;
	calcHist(&hsvImage_base, 1, channels, Mat(), baseHist, 2, histSize, ranges, true, false);
	normalize(baseHist, baseHist, 0, 1, NORM_MINMAX, -1, Mat());

	calcHist(&hsvImage_halfDown, 1, channels, Mat(), halfDownHist, 2, histSize, ranges, true, false);
	normalize(halfDownHist, halfDownHist, 0, 1, NORM_MINMAX, -1, Mat());

	calcHist(&hsvImage_test1, 1, channels, Mat(), testHist1, 2, histSize, ranges, true, false);
	normalize(testHist1, testHist1, 0, 1, NORM_MINMAX, -1, Mat());

	calcHist(&hsvImage_test2, 1, channels, Mat(), testHist2, 2, histSize, ranges, true, false);
	normalize(testHist2, testHist2, 0, 1, NORM_MINMAX, -1, Mat());

	//��˳��ʹ��4�ֶԱȱ�׼����׼ͼ���ֱ��ͼ�������ֱ��ͼ���жԱ�;
	for (int i = 0; i < 4; i++) {
		int compare_method = i;
		double base_base = compareHist(baseHist, baseHist, compare_method);
		double base_half = compareHist(baseHist, halfDownHist, compare_method);
		double base_test1 = compareHist(baseHist, testHist1, compare_method);
		double base_test2 = compareHist(baseHist, testHist2, compare_method);

		cout << "����[" << compare_method << "]��ƥ�������£�" << endl << endl;
		cout << "����׼ͼ-��׼ͼ����" << base_base << "������׼ͼ-����ͼ����" << base_half
			<< "������׼ͼ-����ͼ1����" << base_test1 << "������׼ͼ-����ͼ2����" << base_test2 << endl;

		cout << "-----------------------------------------------------------------------------" << endl;
	}
	*/
#endif // HISTOGRAM

//--------------------------SIFT��main����;----------------------------------------
#if SIFT_AND_BFMATCHER	
	Mat srcImage1 = imread("ukbench00000.jpg", 2 | 4);
	Mat srcImage2 = imread("ukbench00001.jpg", 2 | 4);

	//SIFT�������;
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

//--------------------------SURF��main����;----------------------------------------
#if SURF_AND_BFMATCHER	
	//�ı�console������ɫ;
	system("color 2F");

	Mat srcImage1 = imread("Bookarrival_Original_view1.bmp", 2 | 4);
	Mat srcImage2 = imread("Bookarrival_Original_view4.bmp", 2 | 4);
	if (!srcImage1.data || !srcImage2.data)	{
		cout << "Fail to read images" << endl;
		return false;
	}
	imshow("ԭʼͼ1;", srcImage1);
	imshow("ԭʼͼ2;", srcImage2);

	//������Ҫ�õ��ı�������;
	int minHessian = 700;//����SURF��hessian��ֵ������������;
	SurfFeatureDetector detctor(minHessian);//������������;
	std::vector<KeyPoint> keypoints_1, keypoints_2;//vectorģ�����ܹ�����������͵Ķ�̬���飬�ܹ����Ӻ�ѹ������;

	//����detect��������SURF�����ؼ��㣬������vector������;
	detctor.detect(srcImage1, keypoints_1);
	detctor.detect(srcImage2, keypoints_2);

	//���ƹؼ�������;
	Mat img_keypoints_1, img_keypoints_2;
	drawKeypoints(srcImage1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	drawKeypoints(srcImage2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

	//��ʾЧ��ͼ;
	imshow("��������Ч��ͼ1;", img_keypoints_1);
	imshow("��������Ч��ͼ2;", img_keypoints_2);

	//����������(��������);
	SurfDescriptorExtractor extractor;
	Mat descriptors1, descriptors2;
	extractor.compute(srcImage1, keypoints_1, descriptors1);
	extractor.compute(srcImage2, keypoints_2, descriptors2);

	//ʹ��BruteForce����ƥ��;
	BruteForceMatcher<L2<float>> matcher;//ʵ����;
	std::vector<DMatch> matches;
	matcher.match(descriptors1, descriptors2, matches);//ƥ������ͼ�е�������;

	//���ƴ�����ͼ����ƥ��Ĺؼ���;
	Mat imgMatches;
	drawMatches(srcImage1, keypoints_1, srcImage2, keypoints_2, matches, imgMatches);

	//��ʾЧ��ͼ;
	imshow("ƥ��ͼ;", imgMatches);
#endif // SURF_AND_BFMATCHER

//--------------------------FLANN��main����;---------------------------------------
#if SURF_AND_FLANN
	//�ı�console������ɫ;
	system("color 2F");

	Mat srcImage1 = imread("ukbench00000.jpg", 2 | 4);
	Mat srcImage2 = imread("ukbench00001.jpg", 2 | 4);
	if (!srcImage1.data || !srcImage2.data) {
		cout << "Fail to read images" << endl;
		return false;
	}
	imshow("ԭʼͼ1;", srcImage1);
	imshow("ԭʼͼ2;", srcImage2);

	//������Ҫ�õ��ı�������;
	int minHessian = 700;//����SURF��hessian��ֵ������������;
	SurfFeatureDetector detctor(minHessian);//������������;
	std::vector<KeyPoint> keypoints_1, keypoints_2;//vectorģ�����ܹ�����������͵Ķ�̬���飬�ܹ����Ӻ�ѹ������;

	//����detect��������SURF�����ؼ��㣬������vector������;
	detctor.detect(srcImage1, keypoints_1);
	detctor.detect(srcImage2, keypoints_2);

	//����������(��������);
	SurfDescriptorExtractor extractor;
	Mat descriptors1, descriptors2;
	extractor.compute(srcImage1, keypoints_1, descriptors1);
	extractor.compute(srcImage2, keypoints_2, descriptors2);

	//����FLANN�㷨ƥ����������;
	FlannBasedMatcher matcher;
	std::vector<DMatch> matches;
	matcher.match(descriptors1, descriptors2, matches);

	//���ټ���ؼ���֮���������С����;
	double max_dist = 0;
	double min_dist = 100;
	for (int i = 0; i < descriptors1.rows; i++) {
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	//���������Ϣ;
	cout << "������(Max dist);" << max_dist << endl;
	cout << "��С����(Min dist);" << min_dist << endl;

	//���·���������ƥ����(������С��2*min_dist��)��ʹ��radiusMatchͬ������;
	std::vector<DMatch> good_matches;
	for (int i = 0; i < descriptors1.rows; i++) {
		if (matches[i].distance < 2 * min_dist)
			good_matches.push_back(matches[i]);
	}

	//���ƴ�����ͼ����ƥ��Ĺؼ���;
	Mat imgMatches;
	drawMatches(srcImage1, keypoints_1, srcImage2, keypoints_2, good_matches, imgMatches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//�������ƥ�����Ϣ;
	for (int i = 0; i < good_matches.size(); i++) {
		cout << ">����������ƥ���;" << i << "������1;" << good_matches[i].queryIdx << " --������2;" << good_matches[i].trainIdx << endl;
	}

	//��ʾЧ��ͼ;
	imshow("ƥ��ͼ;", imgMatches);
	//imwrite("ƥ��ͼ.png", imgMatches);
#endif // SURF_AND_FLANN

//--------------------------����FLANN��Ѱ����֪����main����;-----------------------
#if SURF_AND_FLANN_AND_OBJECT_DETECTION
	//�ı�console������ɫ;
	system("color 2F");

	Mat srcImage1 = imread("Bookarrival_Original_view1.bmp", 2 | 4);
	Mat srcImage2 = imread("Bookarrival_Original_view4.bmp", 2 | 4);
	if (!srcImage1.data || !srcImage2.data) {
		cout << "Fail to read images" << endl;
		return false;
	}
	imshow("ԭʼͼ1;", srcImage1);
	imshow("ԭʼͼ2;", srcImage2);

	//ʹ��SURF���Ӽ��ؼ���;
	int minHessian = 400;//SURF�㷨�е�hessian��ֵ;
	SurfFeatureDetector detector(minHessian);//����һ��SurfFeatureDetector������������;
	vector<KeyPoint> keypoints_object, keypoints_scene;//vectorģ���࣬���Դ���������͵Ķ�̬����;

	//����detect��������SURF�����ؼ��㣬������vector������;
	detector.detect(srcImage1, keypoints_object);
	detector.detect(srcImage2, keypoints_scene);

	//����������(��������);
	SurfDescriptorExtractor extractor;
	Mat descriptors_object, descriptors_scene;
	extractor.compute(srcImage1, keypoints_object, descriptors_object);
	extractor.compute(srcImage2, keypoints_scene, descriptors_scene);

	//ʹ��FLANNƥ�����ӽ���ƥ��;
	FlannBasedMatcher matcher;
	vector<DMatch> matches;
	matcher.match(descriptors_object, descriptors_scene, matches);

	//���ټ���ؼ���֮���������С����;
	double max_dist = 0;
	double min_dist = 100;
	for (int i = 0; i < descriptors_object.rows; i++) {
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	//���������Ϣ;
	cout << "������(Max dist);" << max_dist << endl;
	cout << "��С����(Min dist);" << min_dist << endl;

	//����ƥ�����С��3*min_dist�ĵ��;
	std::vector<DMatch> good_matches;
	for (int i = 0; i < descriptors_object.rows; i++) {
		if (matches[i].distance < 3 * min_dist)
			good_matches.push_back(matches[i]);
	}

	//���ƴ�����ͼ����ƥ��Ĺؼ���;
	Mat imgMatches;
	drawMatches(srcImage1, keypoints_object, srcImage2, keypoints_scene, good_matches, imgMatches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//��ƥ��ɹ���ƥ����л�ȡ�ؼ���;
	vector<Point2f> obj, scene;
	for (unsigned int i = 0; i < good_matches.size(); i++) {
		obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}

	//����͸�ӱ任����;
	Mat H = findHomography(obj, scene, CV_RANSAC);

	//�Ӵ���ͼƬ�л�ȡ�ǵ�;
	vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0);
	obj_corners[1] = cvPoint(srcImage1.cols, 0);
	obj_corners[2] = cvPoint(srcImage1.cols, srcImage1.rows);
	obj_corners[3] = cvPoint(0, srcImage1.rows);

	//����͸�ӱ任;
	vector<Point2f> scene_corners(4);
	perspectiveTransform(obj_corners, scene_corners, H);

	//����ǵ�֮���ֱ��;
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

	//��ʾ���ս��;
	imshow("Good Matches & Object Detection", imgMatches);
#endif // SURF_AND_FLANN_AND_OBJECT_DETECTION

//--------------------------����FLANN��Ѱ����֪����main����;-----------------------
#if ORB_AND_FLANN
	//�ı�console������ɫ;
	system("color 2F");

	Mat srcImage1 = imread("ukbench00000.jpg", 2 | 4);
	Mat srcImage2 = imread("ukbench00001.jpg", 2 | 4);
	if (!srcImage1.data || !srcImage2.data) {
		cout << "Fail to read images" << endl;
		return false;
	}
	imshow("ԭʼͼ1;", srcImage1);
	imshow("ԭʼͼ2;", srcImage2);

	Mat gryImage1, gryImage2;
	cvtColor(srcImage1, gryImage1, CV_BGR2GRAY);
	cvtColor(srcImage2, gryImage2, CV_BGR2GRAY);

	//��������;
	OrbFeatureDetector featureDetector;
	vector<KeyPoint> KeyPoints1, KeyPoints2;
	Mat descriptors1, descriptors2;

	//����detect������������ؼ��㣬������vector������;
	featureDetector.detect(gryImage1, KeyPoints1);
	featureDetector.detect(gryImage2, KeyPoints2);

	//����������(��������);
	OrbDescriptorExtractor featureExtractor;
	featureExtractor.compute(gryImage1, KeyPoints1, descriptors1);
	featureExtractor.compute(gryImage2, KeyPoints2, descriptors2);

	//����FLANN������������ƥ��;
	flann::Index flannIndex(descriptors1, flann::LshIndexParams(12, 20, 2), cvflann::FLANN_DIST_HAMMING);
	Mat matchIndex(descriptors2.rows, 2, CV_32SC1);
	Mat matchDistance(descriptors2.rows, 2, CV_32FC1);
	flannIndex.knnSearch(descriptors2, matchIndex, matchDistance, 2, flann::SearchParams());//����K�����㷨;

	//���������㷨;(Lowe's algorithm)ѡ�������ƥ��;
	vector<DMatch> goodMatches;
	for (int i = 0; i < matchDistance.rows; i++) {
		if (matchDistance.at<float>(i, 0) < 0.6 * matchDistance.at<float>(i, 1)) {
			DMatch dmatches(i, matchIndex.at<int>(i, 0), matchDistance.at<float>(i, 0));
			goodMatches.push_back(dmatches);
		}
	}

	//���Ʋ���ʾƥ�䴰��;
	Mat resultImage;
	drawMatches(srcImage2, KeyPoints2, srcImage1, KeyPoints1, goodMatches, resultImage);
	imshow("ƥ�䴰��;", resultImage);
	imwrite("ƥ��ͼ.png", resultImage);

#endif // ORB_AND_FLANN

//--------------------------��ʴ������main����;------------------------------------
#if ERODE
	Mat src = imread("1.jpg", 2 | 4);
	//getStructuringElement�������ص���ָ����״�ͳߴ�ĽṹԪ��;
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	Mat dst;
	erode(src, dst, element);
	Mat merge;
	mergeImg(merge, src, dst);
	imshow("��ʴЧ��ͼ;", merge);
#endif // ERODE

//--------------------------���Ͳ�����main����;------------------------------------
#if DILATE
	Mat src = imread("1.jpg", 2 | 4);
	//getStructuringElement�������ص���ָ����״�ͳߴ�ĽṹԪ��;
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	Mat dst;
	dilate(src, dst, element);
	Mat merge;
	mergeImg(merge, src, dst);
	imshow("����Ч��ͼ;", merge);
#endif // DILATE

//--------------------------�߼���̬ѧ�����main����;------------------------------
#if ADVANCED_MORPHOLOGY
	Mat src = imread("1.jpg", 2 | 4);
	//getStructuringElement�������ص���ָ����״�ͳߴ�ĽṹԪ��;
	Mat element = getStructuringElement(MORPH_RECT, Size(7, 7));
	Mat dst;

	//��3�������޸����ִ���;
	//MORPH_OPEN ������;
	//MORPH_CLOSE ������;
	//MORPH_GRADIENT ��̬ѧ�ݶ�;
	//MORPH_TOPHAT ��ñ;
	//MORPH_BLACKHAT ��ñ;
	//MORPH_ERODE ��ʴ��������;
	//MORPH_DILATE ���ͣ�������;
	morphologyEx(src, dst, MORPH_DILATE, element);
	Mat merge;
	mergeImg(merge, src, dst);
	imshow("�߼���̬ѧ����Ч��ͼ;", merge);
#endif // ADVANCED_MORPHOLOGY

//--------------------------ͼ�����ŵ�main����;------------------------------------
#if RESIZE
	Mat src = imread("1.jpg", 2 | 4);
	imshow("ԭʼͼ;", src);

	// ����1 �̶���С;
	Mat dst1 = Mat::zeros(512, 512, CV_8UC3);
	resize(src, dst1, dst1.size());
	imshow("�����ߴ��1;", dst1);

	// ����2 ������;
	Mat dst2;
	resize(src, dst2, Size(), 0.5, 0.5);
	imshow("�����ߴ��2;", dst2);

#endif // RESIZE

//--------------------------ͼ���������main����;----------------------------------
#if PYRAMID

	Mat src = imread("1.jpg", 2 | 4);
	imshow("ԭʼͼ;", src);

	Mat dst1, dst2;
	pyrUp(src, dst1, Size(src.cols * 2, src.rows * 2)); // �Ŵ�һ��;
	pyrDown(src, dst2, Size(src.cols*0.5, src.rows*0.5)); // ��Сһ��;

	imshow("�ߴ�Ŵ�֮��;", dst1);
	imshow("�ߴ���С֮��;", dst2);

#endif // PYRAMID

//--------------------------YUV��ص�main����;-------------------------------------
#if YUV_CLASS
	//���������ļ���;
	vector<string> allPicname;
	for (int i = 1; i < argc - 1; ++i) {
		allPicname.push_back(argv[i]);
	}

	//�����;
	string outpicName = allPicname[0];
	for (int i = 0; i < 4; i++) {
		outpicName.pop_back();
	}
	outpicName += ".bmp";
	
	//��ȡ�ļ���;
	CIYuv yuvBuffer;
	FILE *fin_yuv;
	if ((fin_yuv = fopen(allPicname[0].c_str(), "rb")) == NULL) {
		fprintf(stderr, "Can't open input file(s)\n");
		return 3;
	}

	//����YUV���в���;
	if (!yuvBuffer.Resize(768, 1024, 420))
		return 2;

	//��ȡ�趨֡;
	/*
	int num = atoi(argv[2]);
	if (!yuvBuffer.ReadOneFrame(fin_yuv, num)){
		return 2;
	}

	IplImage *bgr = cvCreateImage(cvSize(1024, 768), 8, 3);
	yuvBuffer.getData_inBGR(bgr);

	cv::Mat matimg;
	matimg = cv::Mat(bgr);
	imshow("���ͼ;", matimg);
	//imwrite(outpicName, matimg);
	*/

	//�󱳾�(���ȡƽ��);
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
		//imshow("���ͼ;", matimg);
		//waitKey(30);
	}

	bg /= 5;
	bg.convertTo(bg, CV_8UC3, 255, 0);
	imshow("���ͼ;", bg);
	imwrite("ARV_5.bmp", bg);
	*/

	//�󱳾�(GMM);
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
	cv::imshow("��Ĥͼ;", bg);
	imwrite("GMM_5.bmp", bg);
	*/

	//��������䵥�ӵ�ն�(��Ե��ӵ��˶������Ե�ն��޸�);
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

		//imshow("�Ա�ͼ;", dst);

		imshow("ԭʼδ����;", src);
		imshow("�����;", matimg);
		waitKey();
	}

	//�ر���;
	fclose(fin_yuv);
#endif // YUV_CLASS

//--------------------------Mat���main����;---------------------------------------
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

//--------------------------��ֵ����main����;--------------------------------------
#if BINARYZATION
	Mat image = imread("1.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	//ȫ�ֶ�ֵ��;
	int th = 25;
	Mat global;
	threshold(image, global, th, 255, CV_THRESH_BINARY_INV);

	//�ֲ���ֵ��;
	int blockSize = 25;
	int constValue = 10;
	Mat local;
	adaptiveThreshold(image, local, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, blockSize, constValue);

	imshow("ȫ�ֶ�ֵ��;", global);
	imshow("�ֲ���ֵ��;", local);
#endif // BINARYZATION

//--------------------------��ֵ�˲���main����;------------------------------------
#if MEAN_FILTERING
	Mat src = imread("1.jpg", 2 | 4);
	Mat dst;
	blur(src, dst, Size(7, 7));

	Mat merge;
	mergeImg(merge, src, dst);

	imshow("��ֵģ��Ч��ͼ;", merge);

#endif // MEAN_FILTERING

//--------------------------Canny��Ե����main����;-------------------------------
#if CANNY_EDGE_DETECTION
	Mat src = imread("1.jpg", 2 | 4);

	Mat dst, edge, grayImage;
	cvtColor(src, grayImage, COLOR_BGR2GRAY);

	//��ʹ��3*3�ں�������;
	blur(grayImage, edge, Size(3, 3));

	//����canny����;
	Canny(edge, dst, 30, 90);
	imshow("��Ե��ȡЧ��ͼ;", dst);

#endif // CANNY_EDGE_DETECTION

//--------------------------��ˮ��ָ��main����;----------------------------------
#if WATERSHED
	Mat image = imread("test.jpg", 2 | 4);
	imshow("Source Image", image);

	//�ҶȻ����˲���Canny��Ե���;
	Mat imageGray;
	cvtColor(image, imageGray, CV_RGB2GRAY);//�Ҷ�ת��;
	
	GaussianBlur(imageGray, imageGray, Size(5, 5), 2);//��˹�˲�;

	imshow("Gray Image", imageGray);

	//Canny(imageGray, imageGray, 80, 150);
	Canny(imageGray, imageGray, 60, 140);

	imshow("Canny Image", imageGray);

	//��������;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(imageGray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());
	Mat imageContours = Mat::zeros(image.size(), CV_8UC1);  //����;
	Mat marks(image.size(), CV_32S);   //Opencv��ˮ��ڶ����������;
	marks = Scalar::all(0);
	int index = 0;
	int compCount = 0;
	for (; index >= 0; index = hierarchy[index][0], compCount++){
		//��marks���б�ǣ��Բ�ͬ������������б�ţ��൱������עˮ�㣬�ж������������ж���עˮ��  
		drawContours(marks, contours, index, Scalar::all(compCount + 1), 1, 8, hierarchy);
		drawContours(imageContours, contours, index, Scalar(255), 1, 8, hierarchy);
	}

	//��������һ�´���ľ���marks����ʲô����;
	Mat marksShows;
	convertScaleAbs(marks, marksShows);
	imshow("marksShow", marksShows);
	imshow("����;", imageContours);
	watershed(image, marks);

	//����������һ�·�ˮ���㷨֮��ľ���marks����ʲô����;
	Mat afterWatershed;
	convertScaleAbs(marks, afterWatershed);
	imshow("After Watershed", afterWatershed);

	//��ÿһ�����������ɫ���;
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

	//�ָ�����ɫ�Ľ����ԭʼͼ���ں�;
	Mat wshed;
	addWeighted(image, 0.6, PerspectiveImage, 0.4, 0, wshed);
	imshow("AddWeighted Image", wshed);

#endif // WATERSHED

//--------------------------ͼ���޸���main����;------------------------------------
#if INPAINT
	Mat srcImage = imread("1.jpg", 2 | 4);
	srcImage1 = srcImage.clone();
	inpaintMask = Mat::zeros(srcImage.size(), CV_8U);

	imshow("ԭʼͼ;", srcImage1);

	//�������ص���Ϣ;
	setMouseCallback("ԭʼͼ;", On_Mouse, 0);

	//��ѯ���������ݲ�ͬ�İ������д���;
	while (1) {
		//��ȡ������ֵ;
		char c = (char)waitKey();

		//��ֵΪESC�������˳�;
		if (c == 27)
			break;

		//��ֵΪ2���ָ���ԭʼͼ��;
		if (c == '2') {
			inpaintMask = Scalar::all(0);
			srcImage.copyTo(srcImage1);
			imshow("ԭʼͼ;", srcImage1);
		}

		//��ֵΪ1���߿ո񣬽���ͼ���޲�����;
		if (c == '1' || c == ' ') {
			Mat inpaintedImage;
			inpaint(srcImage1, inpaintMask, inpaintedImage, 3, INPAINT_TELEA); //�˴���5������������ INPAINT_NS ���� INPAINT_TELEA;
			imshow("�޲����Ч��ͼ;", inpaintedImage);
		}
	}
	
#endif // INPAINT

	Mat src = imread("1.jpg", 2 | 4);

	Msrcr msrcr;

	vector<double> sigema;
	vector<double> weight;
	for (int i = 0; i < 3; i++)
		weight.push_back(1. / 3);
	// ����MSRCR.h�ж����˺�USE_EXTRA_SIGMA�����Դ˴���vector<double> sigema��û��ʲô����
	sigema.push_back(30);
	sigema.push_back(150);
	sigema.push_back(300);


	Mat dst;
	msrcr.MultiScaleRetinexCR(src, dst, weight,sigema,100,100);
	cv::imshow("dst", dst);

	waitKey(0);
	return 1;
}

