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
1���ж��Ƿ�Ϊ��ɫͼ�еĿն�
*/
bool CUsingBackground::isHoleinColor(CIYuv *yuvBuffer, int h, int w)
{
	//>�ж�����������Yͨ����UVͨ��
	//if (yuvBuffer->Y[h][w] == 0 &&
	//	(yuvBuffer->U[h / 2][w / 2] >= 120 && yuvBuffer->U[h / 2][w / 2] <= 135) &&
	//	(yuvBuffer->V[h / 2][w / 2] >= 120 && yuvBuffer->V[h / 2][w / 2] <= 135))
	//>�ж�����ֻ������Yͨ��
	if (yuvBuffer->Y[h][w] == 0)
		return true;
	else
		return false;
}

/*
1�������ÿһ֡���ͼ�ն���ȷ����ǰ�ն���ߵ�һ����Ϊ0ֵ��������ұߵ�һ����Ϊ0ֵ������
2�������ǰ�ն����ڻ�������ߣ������ұߵ�һ����Ϊ0ֵ���긳ֵ��Ƭ����
3�������ǰ�ն���������������ұߵ㣬������ߵ�һ����Ϊ0ֵ���긳ֵ��Ƭ����
*/
void CUsingBackground::fillDepthStreamHole(CIYuv * yuvBuffer)
{
	for (int h = 0; h < m_iHeight; h++) {
		for (int w = 0; w < m_iWidth; w++) {
			//>�����ǰ�����ǿն���
			if (yuvBuffer->Y[h][w] == 0) {
				//>�����ǰ��������ߵ�һ����
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
					//>�����ǰ�ն���������������ұߵ�
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

void CUsingBackground::doOneFrame()
{
	fillDepthStreamHole(m_pcInDepth);

	int depthTh = 5;

	for (int h = 0; h < m_iHeight; h++) {
		for (int w = 0; w < m_iWidth; w++) {
			//>�����������ص�Ϊ��ɫ�ն�
			if (isHoleinColor(m_pcInVideo, h, w)) {
				//>����õ�����ֵ�ڱ������ֵ��Χ�� & �������ֵ��Ϊ0
				if ((m_pcInDepth->Y[h][w] >= (m_pcBGDepth->Y[h][w] - depthTh)) && (m_pcInDepth->Y[h][w] <= (m_pcBGDepth->Y[h][w] + depthTh))
				     && (m_pcBGColor->Y[h][w] != 0)) {
					m_pcOutVideo->Y[h][w] = m_pcBGColor->Y[h][w];
					m_pcOutVideo->U[h / 2][w / 2] = m_pcBGColor->U[h / 2][w / 2];
					m_pcOutVideo->V[h / 2][w / 2] = m_pcBGColor->V[h / 2][w / 2];
				}
				//>�Ժ�ɫ������������,����Ҫ��,��Ȼ���д������
				else {
					m_pcOutVideo->Y[h][w] = 0;
					m_pcOutVideo->U[h / 2][w / 2] = 128;
					m_pcOutVideo->V[h / 2][w / 2] = 128;
				}
			}
			//>����������ǲ�ɫ��
			else {
				//>�Ժ�ɫ������������
				//m_pcOutVideo->Y[h][w] = 76;
				//m_pcOutVideo->U[h / 2][w / 2] = 84;
				//m_pcOutVideo->V[h / 2][w / 2] = 255;
				m_pcOutVideo->Y[h][w] = m_pcInVideo->Y[h][w];
				m_pcOutVideo->U[h / 2][w / 2] = m_pcInVideo->U[h / 2][w / 2];
				m_pcOutVideo->V[h / 2][w / 2] = m_pcInVideo->V[h / 2][w / 2];
			}
		}
	}

	inpaint(m_pcOutVideo);
}
