#include "function.h"

int main(int argc, char ** argv) {

	//д������;
	vector<int>compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	//���������ļ���;
	vector<string> allPicname;
	for (int i = 1; i < argc; ++i) {
		allPicname.push_back(argv[i]);
	}

	//�����;
	string outpicName = "001";
	outpicName += ".bmp";


	//�����ļ�����ָ����ļ�;
	cv::Mat lColor = cv::imread(allPicname[0], 2 | 4);
	cv::Mat rColor = cv::imread(allPicname[1], 2 | 4);
	cv::Mat lDisp = cv::imread(allPicname[2], 2 | 4);
	cv::Mat rDisp = cv::imread(allPicname[3], 2 | 4);

	cv::imshow("���ӵ�ͼ;", lColor);
	cv::imshow("���ӵ�ͼ;", rColor);

	//����Ե�����Ӳ�ת��;
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

	cv::imshow("���ӵ�ƫ��ͼ;", lwarped);
	cv::imshow("���ӵ�ƫ��ͼ;", rwarped);

	//cv::imwrite("���ӵ�ƫ��ͼ.png", lwarped, compression_params);
	//cv::imwrite("���ӵ�ƫ��ͼ.png", rwarped, compression_params);
	
	//Merge
	cv::Mat virtualView;
	virtualView.create(lColor.rows, lColor.cols, CV_8UC3);
	Merge(lwarped, rwarped, virtualView);
	
	cv::imshow("�ں�ͼ;", virtualView);

	//FillHoles
	FillHoles(virtualView);
	
	

	//cv::imwrite(outpicName, virtualView, compression_params);

	cv::imshow("�ն��ͼ;", virtualView);
	cv::waitKey(0);

	return 1;
}