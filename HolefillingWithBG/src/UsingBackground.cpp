#include "UsingBackground.h"



CUsingBackground::CUsingBackground()
{
	m_iWidth = 1024;
	m_iHeight = 768;
	m_iStartFrame = 0;
	m_iTotalFrame = 10;

	m_pcInVideo = nullptr;
	m_pcBackground = nullptr;
	m_pcOutVideo = nullptr;
}


CUsingBackground::~CUsingBackground()
{
	if (m_pcInVideo !=nullptr){
		delete m_pcInVideo;
		m_pcInVideo = nullptr;
	}
	if (m_pcBackground != nullptr) {
		delete m_pcBackground;
		m_pcBackground = nullptr;
	}
	if (m_pcOutVideo != nullptr) {
		delete m_pcOutVideo;
		m_pcOutVideo = nullptr;
	}
}

bool CUsingBackground::allocateMem()
{
	m_pcInVideo = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcBackground = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcOutVideo = new CIYuv(m_iHeight, m_iWidth, 420);

	return true;
}

void CUsingBackground::releaseMem()
{
	if (m_pcInVideo != nullptr) {
		delete m_pcInVideo;
		m_pcInVideo = nullptr;
	}
	if (m_pcBackground != nullptr) {
		delete m_pcBackground;
		m_pcBackground = nullptr;
	}
	if (m_pcOutVideo != nullptr) {
		delete m_pcOutVideo;
		m_pcOutVideo = nullptr;
	}
}

void CUsingBackground::doOneFrame()
{
	for (int h = 0; h < m_iHeight; h++) {
		for (int w = 0; w < m_iWidth; w++) {
			//>如果输入的像素点为黑色空洞
			if (m_pcInVideo->Y[h][w] == 0 && 
				(m_pcInVideo->U[h / 2][w / 2] >= 125 && m_pcInVideo->U[h / 2][w / 2] <= 131) && 
				(m_pcInVideo->V[h / 2][w / 2] >= 125 && m_pcInVideo->V[h / 2][w / 2] <= 131)) {
				m_pcOutVideo->Y[h][w] = m_pcBackground->Y[h][w];
				m_pcOutVideo->U[h / 2][w / 2] = m_pcBackground->U[h / 2][w / 2];
				m_pcOutVideo->V[h / 2][w / 2] = m_pcBackground->V[h / 2][w / 2];
			}
			else {
				m_pcOutVideo->Y[h][w] = m_pcInVideo->Y[h][w];
				m_pcOutVideo->U[h / 2][w / 2] = m_pcInVideo->U[h / 2][w / 2];
				m_pcOutVideo->V[h / 2][w / 2] = m_pcInVideo->V[h / 2][w / 2];
			}
		}
	}

}
