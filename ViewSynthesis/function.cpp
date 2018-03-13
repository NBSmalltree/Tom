#include "function.h"

void ForwardWarp(cv::Mat& Color, cv::Mat& Disp, int LEFTVIEW, cv::Mat& Warped)
{
	//z-buffer;
	cv::Mat WarpedLocal, WarpedLocal_Value;
	WarpedLocal.create(Disp.rows, Disp.cols, CV_16UC1);
	WarpedLocal_Value.create(Disp.rows, Disp.cols, CV_8UC1);
	//init to 0;
	for (int i = 0; i < WarpedLocal.rows; i++) {
		ushort* data = WarpedLocal.ptr<ushort>(i);//��ָ��;
		for (int j = 0; j < WarpedLocal.cols; j++)
			data[j] = 0;
	}
	for (int i = 0; i < WarpedLocal_Value.rows; i++) {
		uchar* data = WarpedLocal_Value.ptr<uchar>(i);//��ָ��;
		for (int j = 0; j < WarpedLocal_Value.cols; j++)
			data[j] = 0;
	}
	//����λ����ģ;
	for (int i = 0; i < Disp.rows; i++) {
		uchar* data1 = Disp.ptr<uchar>(i);//���ӵ��Ӳ�ͼ;
		ushort* data2 = WarpedLocal.ptr<ushort>(i);//λ��;
		uchar* data3 = WarpedLocal_Value.ptr<uchar>(i);//ӳ���ȥ���Ӳ�ֵ;
		for (int j = 0, k = 0; j < Disp.cols; j++) {//k����ӳ����Ŀ�����ص�;
			if(LEFTVIEW)
				k = j - data1[j];
			else
				k = j + data1[j];
			if (k >= 0 && k < Disp.cols) {
				if (data3[k] < data1[j]) {//Ŀ�����ص�洢���Ӳ�ֵ<ԭʼ���ص��Ӳ�ֵ;
					data2[k] = j;//�洢λ��;
					data3[k] = data1[j];//�洢������Ӳ�ֵ;
				}
			}
		}
	}
	//ӳ�����;
	for (int i = 0; i < Disp.rows; i++) {
		uchar* data1 = Color.ptr<uchar>(i);
		ushort* data2 = WarpedLocal.ptr<ushort>(i);
		uchar* data3 = Warped.ptr<uchar>(i);
		for (int j = 0; j < Disp.cols; j++) {
			if (data2[j] == 0) {
				data3[j * 3] = 0;
				data3[j * 3 + 1] = 0;
				data3[j * 3 + 2] = 0;
			}
			else {
				data3[j * 3] = data1[data2[j] * 3];
				data3[j * 3 + 1] = data1[data2[j] * 3 + 1];
				data3[j * 3 + 2] = data1[data2[j] * 3 + 2];
			}
		}
	}
}

void Merge(cv::Mat& LWarp, cv::Mat& RWarp, cv::Mat& VirtualView)
{
	for (int i = 0; i < VirtualView.rows; i++) {
		uchar* data1 = LWarp.ptr<uchar>(i);
		uchar* data2 = RWarp.ptr<uchar>(i);
		uchar* data3 = VirtualView.ptr<uchar>(i);
		for (int j = 0; j < VirtualView.cols; j++) {
			if (data1[j * 3] == 0 && data1[j * 3 + 1] == 0 && data1[j * 3 + 2] == 0) {
				if (data2[j * 3] == 0 && data2[j * 3 + 1] == 0 && data2[j * 3 + 2] == 0) {
					data3[j * 3] = 0; data3[j * 3 + 1] = 0; data3[j * 3 + 2] = 0;
				}
				else {
					data3[j * 3] = data2[j * 3]; data3[j * 3 + 1] = data2[j * 3 + 1]; data3[j * 3 + 2] = data2[j * 3 + 2];
				}
			}
			else {
				if (data2[j * 3] == 0 && data2[j * 3 + 1] == 0 && data2[j * 3 + 2] == 0) {
					data3[j * 3] = data1[j * 3]; data3[j * 3 + 1] = data1[j * 3 + 1]; data3[j * 3 + 2] = data1[j * 3 + 2];
				}
				else {
					data3[j * 3] = (data1[j * 3] + data2[j * 3]) / 2;
					data3[j * 3 + 1] = (data1[j * 3 + 1] + data2[j * 3 + 1]) / 2;
					data3[j * 3 + 2] = (data1[j * 3 + 2] + data2[j * 3 + 2]) / 2;
				}
			}
		}
	}
}

void FillHoles(cv::Mat& VirtualView)
{
	for (int i = 0; i < VirtualView.rows; i++) {
		uchar* data = VirtualView.ptr<uchar>(i);
		for (int j = 1; j < VirtualView.cols - 1; j++) {
			if (data[j * 3] == 0 && data[j * 3 + 1] == 0 && data[j * 3 + 2] == 0) {
				if ((data[(j - 1) * 3] != 0 || data[(j - 1) * 3 + 1] != 0 || data[(j - 1) * 3 + 2] != 0) &&
					(data[(j + 1) * 3] != 0 || data[(j + 1) * 3 + 1] != 0 || data[(j + 1) * 3 + 2] != 0)) {//ֻ��һ���ն���;
					data[j * 3] = (data[(j - 1) * 3] + data[(j + 1) * 3]) / 2;
					data[j * 3 + 1] = (data[(j - 1) * 3 + 1] + data[(j + 1) * 3 + 1]) / 2;
					data[j * 3 + 2] = (data[(j - 1) * 3 + 2] + data[(j + 1) * 3 + 2]) / 2;
				}
				if ((data[(j - 1) * 3] != 0 || data[(j - 1) * 3 + 1] != 0 || data[(j - 1) * 3 + 2] != 0) &&
					(data[(j + 1) * 3] == 0 || data[(j + 1) * 3 + 1] == 0 || data[(j + 1) * 3 + 2] == 0) &&
					(data[(j + 2) * 3] != 0 || data[(j + 2) * 3 + 1] != 0 || data[(j + 2) * 3 + 2] != 0)) {//ֻ�������ն���;
					data[j * 3] = data[(j + 1) * 3] = (data[(j - 1) * 3] + data[(j + 2) * 3]) / 2;
					data[j * 3 + 1] = data[(j + 1) * 3 + 1] = (data[(j - 1) * 3 + 1] + data[(j + 2) * 3 + 1]) / 2;
					data[j * 3 + 2] = data[(j + 1) * 3 + 2] = (data[(j - 1) * 3 + 2] + data[(j + 2) * 3 + 2]) / 2;
				}
			}
		}
	}
}