#include "BackgroundEstimation.h"

CDepthMap::CDepthMap()
{
	DEPTHMAP = NULL;
	pBuffer = NULL;
}

CDepthMap::CDepthMap(int h, int w, int step)
{
	m_iHeight = h;
	m_iWidth = w;
	m_iUpdateStep = step;

	if (!allocate_mem()) {
		m_iHeight = m_iWidth = m_iUpdateStep = 0;
		DEPTHMAP = NULL;
		pBuffer = NULL;
	}
}

CDepthMap::~CDepthMap()
{
	release_mem();
}

bool CDepthMap::allocate_mem()
{
	int h, pos;
	BYTE *buf1D;
	BYTE **buf2D;

	release_mem();

	if ((buf1D = (unsigned char *)malloc(m_iHeight * m_iWidth * m_iUpdateStep * sizeof(BYTE))) == NULL) return false;

	if ((buf2D = (BYTE **)malloc(m_iUpdateStep * sizeof(BYTE *))) == NULL) {
		free(buf1D);
		return false;
	}
	memset(buf1D, 0, m_iHeight * m_iWidth * m_iUpdateStep);
	
	DEPTHMAP = buf2D;
	for (h = pos = 0; h < m_iUpdateStep; h++, pos += m_iHeight * m_iWidth)
		DEPTHMAP[h] = &(buf1D[pos]);
	
	pBuffer = DEPTHMAP[0]; // buf1D;

	return true;
}

void CDepthMap::release_mem()
{
	if (DEPTHMAP != NULL) {
		if (DEPTHMAP[0] != NULL) free(DEPTHMAP[0]);
		free(DEPTHMAP);
		DEPTHMAP = NULL;
		pBuffer = NULL;
	}
}

void CDepthMap::printTestMemory(int rowstart, int colstart, int width, int frame)
{
	std::cout << std::endl << "This is Test Memory for Check" << std::endl;
	
	std::cout << "Frame: " << frame << std::endl;
	//for (int k = 0; k <= frame; k++) {
		for (int i = rowstart; i < rowstart + width; i++)
		{
			for (int j = colstart; j < colstart + width; j++)
			{
				std::cout << (int)DEPTHMAP[frame][i*m_iWidth + j] << ' ';
			}
			std::cout << std::endl;
		}
	//}
	
}

bool CDepthMap::writeOneFrame(CIYuv *yuvBuffer, int n)
{
	if (yuvBuffer == NULL || DEPTHMAP[n] == NULL) return false;

	for (int i = 0; i < m_iHeight; i++)
	{
		for (int j = 0; j < m_iWidth; j++)
		{
			DEPTHMAP[n][i*m_iWidth + j] = yuvBuffer->Y[i][j];
		}
	}

	return true;
}

CBackgroundEstimation::CBackgroundEstimation()
{
	m_pcVideo = NULL;
	m_pcBgr = NULL;
	m_pcDepthMap = NULL;
}


CBackgroundEstimation::~CBackgroundEstimation()
{
	if (m_pcVideo != NULL) delete m_pcVideo;
	if (m_pcBgr != NULL) delete m_pcBgr;
	if (m_pcDepthMap != NULL) delete m_pcDepthMap;
}

bool CBackgroundEstimation::Init(ParameterViewSyn cParameter)
{
	m_iWidth = cParameter.getSourceWidth();
	m_iHeight = cParameter.getSourceHeight();
	m_iFrameNumber = cParameter.getStartFrame();
	m_iTotalNumber = cParameter.getTotalFrame();
	m_iUpdateStep = cParameter.getUpdateStep();

	m_pcVideo = new CIYuv(m_iHeight, m_iWidth, 420);
	
	m_pcBgr = new cv::Mat(m_iHeight, m_iWidth, CV_8UC3);
	
	m_pcDepthMap = new CDepthMap(m_iHeight, m_iWidth, m_iUpdateStep);
	//m_pcDepthMap->printTestMemory();

	return true;
}

void CBackgroundEstimation::showCurrentImage()
{
	cv::imshow("��ǰ֡ͼ��;", *m_pcBgr);
	cv::waitKey(0);
}

bool CBackgroundEstimation::writeCurrentImage()
{
	//д������;
	std::vector<int>compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	if (!imwrite("test.png", *m_pcBgr)) return false;

	return true;
}

void CBackgroundEstimation::showDepthMapTestMemory(int rowstart, int colstart, int width, int frame)
{
	m_pcDepthMap->printTestMemory(rowstart, colstart, width, frame);
}

bool CBackgroundEstimation::writeOneDepthMap(CIYuv *yuvBuffer, int n)
{
	return m_pcDepthMap->writeOneFrame(yuvBuffer, n);
}
