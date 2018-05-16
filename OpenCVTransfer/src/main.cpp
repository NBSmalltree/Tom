#include "iostream"
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char ** argv) {
	
	cv::Mat srcImage = cv::imread("1.jpg", 2 | 4);
	cv::imshow("SrcImage", srcImage);
	cv::waitKey(0);

	return 0;
}