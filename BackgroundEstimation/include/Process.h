#ifndef Process_h__
#define Process_h__

#include <opencv2/photo/photo.hpp>
#include "ParameterViewSyn.h"
#include "DepthBackground.h"
#include "ColorBackground.h"
#include "yuv.h"
#include "PixelHist.h"


class CBackgroundEstimation
{
public:
	CBackgroundEstimation();
	~CBackgroundEstimation();

	void Init(ParameterViewSyn cParameter);
	bool initDepthBackground();
	bool initColorBackground();
	void releaseDepthBackground();
	void releaseColorBackground();

	bool buildDepthMap();
	void calcOnePixelHist(int x, int y);
	void calcHist();
	void inpaint(int height, int width, CIYuv* yuvBuffer);

	bool calcColorBackground();

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

	FILE *fin_depth;
	FILE *fin_color;
	FILE *fout_depthBackground;
	FILE *fin_depthBackground;
	FILE *fout_colorBackground;

	CDepthBackground *m_pDepthBackground;
	CColorBackground *m_pColorBackground;

	cv::Mat *m_pDepthBackgroundImage;
	
};
#endif // Process_h__
