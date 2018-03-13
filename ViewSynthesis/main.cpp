#include "function.h"

int main(int argc, char ** argv) {

	//写入配置;
	vector<int>compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	//输入所有文件名;
	vector<string> allPicname;
	for (int i = 1; i < argc; ++i) {
		allPicname.push_back(argv[i]);
	}

	//输出名;
	string outpicName = "001";
	outpicName += ".bmp";


	//读入文件名所指向的文件;
	cv::Mat lColor = cv::imread(allPicname[0], 2 | 4);
	cv::Mat rColor = cv::imread(allPicname[1], 2 | 4);
	cv::Mat lDisp = cv::imread(allPicname[2], 2 | 4);
	cv::Mat rDisp = cv::imread(allPicname[3], 2 | 4);

	cv::imshow("左视点图;", lColor);
	cv::imshow("右视点图;", rColor);

	//针对性的深度视差转换;
	for (int i = 0; i < lDisp.rows; i++) {
		uchar* data1 = lDisp.ptr<uchar>(i);
		uchar* data2 = rDisp.ptr<uchar>(i);
		for (int j = 0; j < lDisp.cols; j++) {
			//data1[j] = 0.0771697 * data1[j] + 14.75868187;
			//data2[j] = 0.0771697 * data2[j] + 14.75868187;
			data1[j] /= 2;
			data2[j] /= 2;
		}
	}

	//ForwardWarp
	cv::Mat lwarped, rwarped;
	lwarped.create(lColor.rows, lColor.cols, CV_8UC3);
	rwarped.create(lColor.rows, lColor.cols, CV_8UC3);
	ForwardWarp(lColor, lDisp, 1, lwarped);
	ForwardWarp(rColor, rDisp, 0, rwarped);

	cv::imshow("左视点偏移图;", lwarped);
	cv::imshow("右视点偏移图;", rwarped);

	//cv::imwrite("左视点偏移图.png", lwarped, compression_params);
	//cv::imwrite("右视点偏移图.png", rwarped, compression_params);
	
	//Merge
	cv::Mat virtualView;
	virtualView.create(lColor.rows, lColor.cols, CV_8UC3);
	Merge(lwarped, rwarped, virtualView);
	
	cv::imshow("融合图;", virtualView);

	//FillHoles
	FillHoles(virtualView);
	
	

	//cv::imwrite(outpicName, virtualView, compression_params);

	cv::imshow("空洞填补图;", virtualView);
	cv::waitKey(0);

	return 1;
}