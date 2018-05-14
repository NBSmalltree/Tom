#include "ColorBackground.h"


CColorBackground::CColorBackground()
{
	m_pcDepthVideo = nullptr;
	m_pcColorVideo = nullptr;
	
	m_depthBackgroundYUV = nullptr;
	m_colorBackgroundYUV = nullptr;
	m_temporalColorSpaceY = nullptr;
	m_temporalColorSpaceU = nullptr;
	m_temporalColorSpaceV = nullptr;
	countNumber = nullptr;
	countNumberUV = nullptr;
}

CColorBackground::~CColorBackground()
{
	if (m_pcDepthVideo != nullptr) delete m_pcDepthVideo;
	if (m_pcColorVideo != nullptr) delete m_pcColorVideo;

	if (countNumber!=nullptr) {
		delete countNumber;
		countNumber = nullptr;
	}
	if (countNumberUV != nullptr) {
		delete countNumberUV;
		countNumberUV = nullptr;
	}

	if (m_depthBackgroundYUV != nullptr) {
		delete m_depthBackgroundYUV;
		m_depthBackgroundYUV = nullptr;
	}
	if (m_colorBackgroundYUV != nullptr) {
		delete m_colorBackgroundYUV;
		m_colorBackgroundYUV = nullptr;
	}

	if (m_temporalColorSpaceY != nullptr) {
		delete[] m_temporalColorSpaceY[0];
		m_temporalColorSpaceY = nullptr;
	}
	if (m_temporalColorSpaceU != nullptr) {
		delete[] m_temporalColorSpaceU[0];
		m_temporalColorSpaceU = nullptr;
	}
	if (m_temporalColorSpaceV != nullptr) {
		delete[] m_temporalColorSpaceV[0];
		m_temporalColorSpaceV = nullptr;
	}
}

bool CColorBackground::allocateMem()
{
	m_pcDepthVideo = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcColorVideo = new CIYuv(m_iHeight, m_iWidth, 420);

	countNumber = new int[m_iHeight * m_iWidth]{0};
	countNumberUV = new int[m_iHeight * m_iWidth / 4]{ 0 };

	//>Change BGR Format to YUV Format
	m_depthBackgroundYUV = new CIYuv(m_iHeight, m_iWidth, 420);
	m_colorBackgroundYUV = new CIYuv(m_iHeight, m_iWidth, 420);

	m_temporalColorSpaceY = new int *[m_iHeight];
	m_temporalColorSpaceY[0] = new int[m_iHeight * m_iWidth];
	for (int h = 1; h < m_iHeight; h++) {
		m_temporalColorSpaceY[h] = m_temporalColorSpaceY[h - 1] + m_iWidth;
	}
	for (int h = 0; h < m_iHeight; h++)
		for(int w = 0; w < m_iWidth; w++)
			m_temporalColorSpaceY[h][w] = 0;

	m_temporalColorSpaceU = new int *[m_iHeight / 2];
	m_temporalColorSpaceU[0] = new int[m_iHeight * m_iWidth / 4];
	for (int h = 1; h < m_iHeight / 2; h++) {
		m_temporalColorSpaceU[h] = m_temporalColorSpaceU[h - 1] + m_iWidth / 2;
	}
	for (int h = 0; h < m_iHeight / 2; h++)
		for (int w = 0; w < m_iWidth / 2; w++)
			m_temporalColorSpaceU[h][w] = 0;

	m_temporalColorSpaceV = new int *[m_iHeight / 2];
	m_temporalColorSpaceV[0] = new int[m_iHeight * m_iWidth / 4];
	for (int h = 1; h < m_iHeight / 2; h++) {
		m_temporalColorSpaceV[h] = m_temporalColorSpaceV[h - 1] + m_iWidth / 2;
	}
	for (int h = 0; h < m_iHeight / 2; h++)
		for (int w = 0; w < m_iWidth / 2; w++)
			m_temporalColorSpaceV[h][w] = 0;

	return true;
}

void CColorBackground::releaseMem()
{
	if (m_pcDepthVideo!=nullptr){
		delete m_pcDepthVideo;
		m_pcDepthVideo = nullptr;
	}
	if (m_pcColorVideo != nullptr) {
		delete m_pcColorVideo;
		m_pcColorVideo = nullptr;
	}

	if (countNumber != nullptr) {
		delete countNumber;
		countNumber = nullptr;
	}
	if (countNumberUV != nullptr) {
		delete countNumberUV;
		countNumberUV = nullptr;
	}

	if (m_depthBackgroundYUV != nullptr) {
		delete m_depthBackgroundYUV;
		m_depthBackgroundYUV = nullptr;
	}
	if (m_colorBackgroundYUV != nullptr) {
		delete m_colorBackgroundYUV;
		m_colorBackgroundYUV = nullptr;
	}

	if (m_temporalColorSpaceY != nullptr) {
		delete[] m_temporalColorSpaceY[0];
		m_temporalColorSpaceY = nullptr;
	}
	if (m_temporalColorSpaceU != nullptr) {
		delete[] m_temporalColorSpaceU[0];
		m_temporalColorSpaceU = nullptr;
	}
	if (m_temporalColorSpaceV != nullptr) {
		delete[] m_temporalColorSpaceV[0];
		m_temporalColorSpaceV = nullptr;
	}
}

void CColorBackground::buildOneFrameColorBackground()
{
	int rangeMin;
	int rangeMax;
	
	//>开始遍历全图像
	for (int h = 0; h < m_iHeight; h++) {
		for (int w = 0; w < m_iWidth; w++) {
			//>排除深度背景图中的空洞
			//if(m_depthBackgroundYUV->Y[h][w] == 0)
			//	continue;

			rangeMin = CLIP3(m_depthBackgroundYUV->Y[h][w] - m_iRange, 0, 255);
			rangeMax = CLIP3(m_depthBackgroundYUV->Y[h][w] + m_iRange, 0, 255);

			if (m_pcDepthVideo->Y[h][w] >= rangeMin && m_pcDepthVideo->Y[h][w] <= rangeMax) {
				//>Y
				m_temporalColorSpaceY[h][w] += m_pcColorVideo->Y[h][w];
				//>U、V
				m_temporalColorSpaceU[h / 2][w / 2] += m_pcColorVideo->U[h / 2][w / 2];
				m_temporalColorSpaceV[h / 2][w / 2] += m_pcColorVideo->V[h / 2][w / 2];

				//>CountNumber
				countNumber[h*m_iWidth + w]++;
				countNumberUV[h / 2 * m_iWidth / 2 + w / 2]++;
			}
			
		}
	}//>全图像遍历完成
}

void CColorBackground::calcFinalColorBackground()
{
	//>Y
	for (int h = 0; h < m_iHeight; h++) {
		for (int w = 0; w < m_iWidth; w++) {
			if (countNumber[h*m_iWidth + w] != 0) {
				m_colorBackgroundYUV->Y[h][w] = (BYTE)(m_temporalColorSpaceY[h][w] / 1.0 / countNumber[h*m_iWidth + w]);
			}

		}
	}

	//>U、V
	for (int h = 0; h < m_iHeight / 2; h++) {
		for (int w = 0; w < m_iWidth / 2; w++) {
			if (countNumberUV[h*m_iWidth/2 + w] != 0) {
				m_colorBackgroundYUV->U[h][w] = (BYTE)(1.0 * m_temporalColorSpaceU[h][w] / countNumberUV[h * m_iWidth / 2 + w]);
				m_colorBackgroundYUV->V[h][w] = (BYTE)(1.0 * m_temporalColorSpaceV[h][w] / countNumberUV[h * m_iWidth / 2 + w]);
			}
		}
	}

}

void CColorBackground::readDepthBackgroundImage(FILE *fin)
{
	//>Read DepthBackground From .yuv File
	m_depthBackgroundYUV->ReadOneFrame(fin, 0);
}

void CColorBackground::writeColorBackgroundImage(FILE *fin)
{
	m_colorBackgroundYUV->WriteOneFrame(fin);
}
