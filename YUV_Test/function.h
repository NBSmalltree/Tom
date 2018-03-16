#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

double getPSNR(const cv::Mat& I1, const cv::Mat& I2);
cv::Scalar getMSSIM(const cv::Mat& i1, const cv::Mat& i2);
