#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class CPixelHist
{
public:
	CPixelHist();
	CPixelHist(int size, int scale, int frame);
	~CPixelHist();

	void printInputData(unsigned char *pBuffer);

	void findMaxandMin();

	void calcHist(unsigned char *pBuffer);

	void showHist();

	void writeHistImageToFile(int x, int y);

	bool isOnlyOneBar();

	int getDepthValueTag() { return m_idepthValue; }

	int getMaxBarValue() { return m_iMaxValue; }

	int getBackgroundMainPeek();

private:
	int m_iTotalFrame;
	int m_iSize;
	int m_iScale;
	int m_iMaxValue;
	int m_iMinValue;

	int m_iBarValue[256];

	int m_idepthValue;

	cv::Mat *m_pcHistImage;

	cv::Mat *m_pcBackgroundImage;

	void showFrequency();
	
};

