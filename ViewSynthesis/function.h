#include <iostream>
#include <cstring>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;

void ForwardWarp(cv::Mat& Color, cv::Mat& Disp, int LEFTVIEW, cv::Mat& Warped);
void Merge(cv::Mat& LWarp, cv::Mat& RWarp, cv::Mat& VirtualView);
void FillHoles(cv::Mat& VirtualView);