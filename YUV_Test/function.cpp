#include "function.h"

double getPSNR(const cv::Mat& I1, const cv::Mat& I2)
{
	cv::Mat s1;
	absdiff(I1, I2, s1);       // |I1 - I2|
	s1.convertTo(s1, CV_32F);  // 不能在8位矩阵上做平方运算
	s1 = s1.mul(s1);           // |I1 - I2|^2

	cv::Scalar s = sum(s1);         // 叠加每个通道的元素;

	double sse = s.val[0] + s.val[1] + s.val[2]; // 叠加所有通道;

	if (sse <= 1e-10) // 如果值太小就直接等于0
		return 0;
	else
	{
		double  mse = sse / (double)(I1.channels() * I1.total());
		double psnr = 10.0*log10((256 * 256) / mse);
		return psnr;
	}
}

cv::Scalar getMSSIM(const cv::Mat& i1, const cv::Mat& i2)
{
	const double C1 = 6.5025, C2 = 58.5225;
	int w = 5;
	/***************************** INITS **********************************/
	int d = CV_32F;

	cv::Mat I1, I2;
	i1.convertTo(I1, d);           // 不能在单字节像素上进行计算，范围不够;
	i2.convertTo(I2, d);

	cv::Mat I2_2 = I2.mul(I2);        // I2^2
	cv::Mat I1_2 = I1.mul(I1);        // I1^2
	cv::Mat I1_I2 = I1.mul(I2);        // I1 * I2

	/***********************初步计算 ******************************/

	cv::Mat mu1, mu2;   //
	cv::GaussianBlur(I1, mu1, cv::Size(w, w), 1.5);
	cv::GaussianBlur(I2, mu2, cv::Size(w, w), 1.5);

	cv::Mat mu1_2 = mu1.mul(mu1);
	cv::Mat mu2_2 = mu2.mul(mu2);
	cv::Mat mu1_mu2 = mu1.mul(mu2);

	cv::Mat sigma1_2, sigma2_2, sigma12;

	cv::GaussianBlur(I1_2, sigma1_2, cv::Size(w, w), 1.5);
	sigma1_2 -= mu1_2;

	cv::GaussianBlur(I2_2, sigma2_2, cv::Size(w, w), 1.5);
	sigma2_2 -= mu2_2;

	cv::GaussianBlur(I1_I2, sigma12, cv::Size(w, w), 1.5);
	sigma12 -= mu1_mu2;

	///////////////////////////////// 公式 ////////////////////////////////
	cv::Mat t1, t2, t3;

	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

	cv::Mat ssim_map;
	divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

	cv::Scalar mssim = mean(ssim_map); // mssim = ssim_map的平均值
	return mssim;
}