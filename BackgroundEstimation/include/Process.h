#ifndef Process_h__
#define Process_h__

#include "ParameterViewSyn.h"
#include "DepthBackground.h"
#include "yuv.h"
#include "PixelHist.h"


class CBackgroundEstimation
{
public:
	CBackgroundEstimation();
	~CBackgroundEstimation();

	void Init(ParameterViewSyn cParameter);
	bool initDepthBackground();

	//CIYuv* getDepthImageBuffer() { return m_pcDepthImage; }
	//CIYuv* getColorImageBuffer() { return m_pcColorImage; }

	bool buildDepthMap();
	void calcOnePixelHist(int x, int y);
	void calcHist();

private:
	int m_iWidth;
	int m_iHeight;
	int m_iStartFrame;
	int m_iTotalFrame;
	int m_iUpdateStep;

	std::string m_cDepthVideoName;
	std::string m_cColorVideoName;
	std::string m_cDepthBackgroundImageName;
	std::string m_cColorBackgroundImageName;

	//CIYuv *m_pcDepthImage;
	//CIYuv *m_pcColorImage;

	FILE *fin_depth;
	FILE *fin_color;

	CDepthBackground *m_pDepthBackground;

	cv::Mat *m_pDepthBackgroundImage;
	
};
#endif // Process_h__
