#include "UsingBackground.h"



CUsingBackground::CUsingBackground()
{
	m_iWidth = 1024;
	m_iHeight = 768;
	m_iStartFrame = 0;
	m_iTotalFrame = 10;

	m_pcInVideo = nullptr;
	m_pcInDepth = nullptr;
	m_pcBGColor = nullptr;
	m_pcBGDepth = nullptr;
	m_pcOutVideo = nullptr;

	m_pcMatBuffer = nullptr;
}


CUsingBackground::~CUsingBackground()
{
	if (m_pcInVideo !=nullptr){
		delete m_pcInVideo;
		m_pcInVideo = nullptr;
	}
	if (m_pcInDepth != nullptr) {
		delete m_pcInDepth;
		m_pcInDepth = nullptr;
	}
	if (m_pcBGColor != nullptr) {
		delete m_pcBGColor;
		m_pcBGColor = nullptr;
	}
	if (m_pcBGDepth != nullptr) {
		delete m_pcBGDepth;
		m_pcBGDepth = nullptr;
	}
	if (m_pcOutVideo != nullptr) {
		delete m_pcOutVideo;
		m_pcOutVideo = nullptr;
	}

	if (m_pcMatBuffer != nullptr) {
		delete m_pcMatBuffer;
		m_pcMatBuffer = nullptr;
	}
}

bool CUsingBackground::allocateMem()
{
	m_pcInVideo = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcInDepth = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcBGColor = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcBGDepth = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcOutVideo = new CIYuv(m_iHeight, m_iWidth, 420);

	m_pcMatBuffer = new cv::Mat(m_iHeight, m_iWidth, CV_8UC3, cv::Scalar::all(0));

	return true;
}

void CUsingBackground::releaseMem()
{
	if (m_pcInVideo != nullptr) {
		delete m_pcInVideo;
		m_pcInVideo = nullptr;
	}
	if (m_pcInDepth != nullptr) {
		delete m_pcInDepth;
		m_pcInDepth = nullptr;
	}
	if (m_pcBGColor != nullptr) {
		delete m_pcBGColor;
		m_pcBGColor = nullptr;
	}
	if (m_pcBGDepth != nullptr) {
		delete m_pcBGDepth;
		m_pcBGDepth = nullptr;
	}
	if (m_pcOutVideo != nullptr) {
		delete m_pcOutVideo;
		m_pcOutVideo = nullptr;
	}

	if (m_pcMatBuffer != nullptr) {
		delete m_pcMatBuffer;
		m_pcMatBuffer = nullptr;
	}
}

/*
1、判断是否为彩色图中的空洞
*/
bool CUsingBackground::isHoleinColor(CIYuv *yuvBuffer, int h, int w)
{
	//>判定条件考虑了Y通道和UV通道
	//if (yuvBuffer->Y[h][w] == 0 &&
	//	(yuvBuffer->U[h / 2][w / 2] >= 120 && yuvBuffer->U[h / 2][w / 2] <= 135) &&
	//	(yuvBuffer->V[h / 2][w / 2] >= 120 && yuvBuffer->V[h / 2][w / 2] <= 135))
	//>判定条件只考虑了Y通道
	if (yuvBuffer->Y[h][w] == 0)
		return true;
	else
		return false;
}

/*
1、按行填补每一帧深度图空洞，确定当前空洞左边第一个不为0值的坐标和右边第一个不为0值的坐标
2、如果当前空洞点在画面最左边，则以右边第一个不为0值坐标赋值该片区域
3、如果当前空洞区域包含画幅最右边点，则以左边第一个不为0值坐标赋值该片区域
*/
void CUsingBackground::fillDepthStreamHole(CIYuv * yuvBuffer)
{
	for (int h = 0; h < m_iHeight; h++) {
		for (int w = 0; w < m_iWidth; w++) {
			//>如果当前像素是空洞点
			if (yuvBuffer->Y[h][w] == 0) {
				//>如果当前像素是左边第一个点
				if (w == 0) {
					int len = 1;
					while (yuvBuffer->Y[h][len] == 0) {
						len++;
						if (len >= m_iWidth)
							break;
					}
					for (int j = 0; j < len; j++)
						yuvBuffer->Y[h][j] = yuvBuffer->Y[h][len];
					w = len;
				}
				else {
					int leftValue = yuvBuffer->Y[h][w - 1];
					int len = 1;
					while (yuvBuffer->Y[h][w + len] == 0) {
						len++;
						if (len >= m_iWidth - w)
							break;
					}
					//>如果当前空洞区域包含画幅最右边点
					if (w + len >= m_iWidth) {
						for (int j = w; j < w + len; j++)
							yuvBuffer->Y[h][j] = leftValue;
						w = m_iWidth - 1;
					}
					else {
						int rghtValue = yuvBuffer->Y[h][w + len];
						int minValue = leftValue < rghtValue ? leftValue : rghtValue;
						for (int j = w; j < w + len; j++)
							yuvBuffer->Y[h][j] = minValue;
						w += len;
					}
				}
			}
		}
	}
}

void CUsingBackground::inpaint(CIYuv * yuvBuffer)
{
	yuvBuffer->setData_inBGR(m_pcMatBuffer);

	cv::Mat Mask(m_iHeight, m_iWidth, CV_8UC1, cv::Scalar::all(0));

	for (int h = 0; h < Mask.rows; h++) {
		uchar* mskLine = Mask.ptr<uchar>(h);
		for (int w = 0; w < Mask.cols; w++)
			if (yuvBuffer->Y[h][w] == 0)
				mskLine[w] = 255;
	}

	cv::Mat inpaintedImage;
	cv::inpaint(*m_pcMatBuffer, Mask, inpaintedImage, 3, cv::INPAINT_NS);

	yuvBuffer->setDataFromImgBGR(&inpaintedImage);
}

/*
1、按行填补每一帧彩色图空洞，确定当前空洞左边第一个不为0值的坐标和右边第一个不为0值的坐标
2、如果当前空洞点在画面最左边，则以右边第一个不为0值坐标赋值该片区域
3、如果当前空洞区域包含画幅最右边点，则以左边第一个不为0值坐标赋值该片区域
4、如果当前空洞区域左右均有值，则考虑以左右第一个不为空洞点的深度值较小者赋值该片区域
*/
void CUsingBackground::VSRSInpaint(CIYuv * yuvBuffer, CIYuv * depthBuffer)
{
	for (int h = 0; h < m_iHeight; h++) {
		for (int w = 0; w < m_iWidth; w++) {
			//>如果当前像素是空洞点，此处只判定了Y通道
			if (yuvBuffer->Y[h][w] == 0) {
				//>如果当前像素是左边第一个点
				if (w == 0) {
					int len = 1;
					while (yuvBuffer->Y[h][len] == 0) {
						len++;
						if (len >= m_iWidth)
							break;
					}
					for (int j = 0; j < len; j++) {
						yuvBuffer->Y[h][j] = yuvBuffer->Y[h][len];
						yuvBuffer->U[h / 2][j / 2] = yuvBuffer->U[h / 2][len / 2];
						yuvBuffer->V[h / 2][j / 2] = yuvBuffer->V[h / 2][len / 2];
					}
					w = len;
				}
				else {
					int leftDepth = depthBuffer->Y[h][w - 1];
					int leftValueY = yuvBuffer->Y[h][w - 1];
					int leftValueU = yuvBuffer->U[h / 2][(w - 1) / 2];
					int leftValueV = yuvBuffer->V[h / 2][(w - 1) / 2];
					int len = 1;
					while (yuvBuffer->Y[h][w + len] == 0) {
						len++;
						if (len >= m_iWidth - w)
							break;
					}
					//>如果当前空洞区域包含画幅最右边点
					if (w + len >= m_iWidth) {
						for (int j = w; j < w + len; j++) {
							yuvBuffer->Y[h][j] = leftValueY;
							yuvBuffer->U[h / 2][j / 2] = leftValueU;
							yuvBuffer->V[h / 2][j / 2] = leftValueV;
						}
						w = m_iWidth - 1;
					}
					else {
						int rghtDepth = depthBuffer->Y[h][w + len];
						int rghtValueY = yuvBuffer->Y[h][w + len];
						int rghtValueU = yuvBuffer->U[h / 2][(w + len) / 2];
						int rghtValueV = yuvBuffer->V[h / 2][(w + len) / 2];

						if (leftDepth <= rghtDepth) {
							for (int j = w; j < w + len; j++) {
								yuvBuffer->Y[h][j] = leftValueY;
								yuvBuffer->U[h / 2][j / 2] = leftValueU;
								yuvBuffer->V[h / 2][j / 2] = leftValueV;
							}
						}
						else {
							for (int j = w; j < w + len; j++) {
								yuvBuffer->Y[h][j] = rghtValueY;
								yuvBuffer->U[h / 2][j / 2] = rghtValueU;
								yuvBuffer->V[h / 2][j / 2] = rghtValueV;
							}
						}

						w += len;
					}
				}
			}
		}
	}
}

void CUsingBackground::doOneFrame()
{
	fillDepthStreamHole(m_pcInDepth);

	int depthTh = 5;

	for (int h = 0; h < m_iHeight; h++) {
		for (int w = 0; w < m_iWidth; w++) {
			//>如果输入的像素点为黑色空洞
			if (isHoleinColor(m_pcInVideo, h, w)) {
				//>如果该点的深度值在背景深度值范围内 & 背景深度值不为0
				if ((m_pcInDepth->Y[h][w] >= (m_pcBGDepth->Y[h][w] - depthTh)) && (m_pcInDepth->Y[h][w] <= (m_pcBGDepth->Y[h][w] + depthTh))
				     && (m_pcBGColor->Y[h][w] != 0)) {
					m_pcOutVideo->Y[h][w] = m_pcBGColor->Y[h][w];
					m_pcOutVideo->U[h / 2][w / 2] = m_pcBGColor->U[h / 2][w / 2];
					m_pcOutVideo->V[h / 2][w / 2] = m_pcBGColor->V[h / 2][w / 2];

					//>以红色来看填充的区域
					m_pcOutVideo->Y[h][w] = 76;
					m_pcOutVideo->U[h / 2][w / 2] = 84;
					m_pcOutVideo->V[h / 2][w / 2] = 255;

				}
				//>以黑色来看填充的区域,必须要有,不然会有错误填充
				else {
					m_pcOutVideo->Y[h][w] = 0;
					m_pcOutVideo->U[h / 2][w / 2] = 128;
					m_pcOutVideo->V[h / 2][w / 2] = 128;
				}
			}
			//>否则输入的是彩色点
			else {
				//>以红色来看填充的区域
				//m_pcOutVideo->Y[h][w] = 76;
				//m_pcOutVideo->U[h / 2][w / 2] = 84;
				//m_pcOutVideo->V[h / 2][w / 2] = 255;
				m_pcOutVideo->Y[h][w] = m_pcInVideo->Y[h][w];
				m_pcOutVideo->U[h / 2][w / 2] = m_pcInVideo->U[h / 2][w / 2];
				m_pcOutVideo->V[h / 2][w / 2] = m_pcInVideo->V[h / 2][w / 2];
			}
		}
	}

	//VSRSInpaint(m_pcOutVideo, m_pcInDepth);
	//inpaint(m_pcOutVideo);
}
