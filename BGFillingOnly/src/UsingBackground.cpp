#include "UsingBackground.h"



CUsingBackground::CUsingBackground()
{
	m_iWidth = 1024;
	m_iHeight = 768;
	m_iStartFrame = 0;
	m_iTotalFrame = 10;

	m_pcInVideo = nullptr;
	m_pcBGColor = nullptr;
	m_pcOutVideo = nullptr;
}


CUsingBackground::~CUsingBackground()
{
	if (m_pcInVideo !=nullptr){
		delete m_pcInVideo;
		m_pcInVideo = nullptr;
	}
	if (m_pcBGColor != nullptr) {
		delete m_pcBGColor;
		m_pcBGColor = nullptr;
	}
	if (m_pcOutVideo != nullptr) {
		delete m_pcOutVideo;
		m_pcOutVideo = nullptr;
	}
}

bool CUsingBackground::allocateMem()
{
	m_pcInVideo = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcBGColor = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcOutVideo = new CIYuv(m_iHeight, m_iWidth, 420);

	return true;
}

void CUsingBackground::releaseMem()
{
	if (m_pcInVideo != nullptr) {
		delete m_pcInVideo;
		m_pcInVideo = nullptr;
	}
	if (m_pcBGColor != nullptr) {
		delete m_pcBGColor;
		m_pcBGColor = nullptr;
	}
	if (m_pcOutVideo != nullptr) {
		delete m_pcOutVideo;
		m_pcOutVideo = nullptr;
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

void CUsingBackground::doOneFrame()
{
	for (int h = 0; h < m_iHeight; h++) {
		for (int w = 0; w < m_iWidth; w++) {
			//>如果输入的像素点为黑色空洞
			if (isHoleinColor(m_pcInVideo, h, w)) {			
				m_pcOutVideo->Y[h][w] = m_pcBGColor->Y[h][w];
				m_pcOutVideo->U[h / 2][w / 2] = m_pcBGColor->U[h / 2][w / 2];
				m_pcOutVideo->V[h / 2][w / 2] = m_pcBGColor->V[h / 2][w / 2];
			}
			else {
				m_pcOutVideo->Y[h][w] = m_pcInVideo->Y[h][w];
				m_pcOutVideo->U[h / 2][w / 2] = m_pcInVideo->U[h / 2][w / 2];
				m_pcOutVideo->V[h / 2][w / 2] = m_pcInVideo->V[h / 2][w / 2];
			}
		}
	}
}
