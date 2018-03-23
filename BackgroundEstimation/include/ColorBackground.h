#include <iostream>
#include "yuv.h"

#define CLIP3(x, min, max) ((x)<(min)?(min):((x)>(max)?(max):(x)))

class CColorBackground
{
public:
	CColorBackground();
	~CColorBackground();

	void setWidth(int sWidth) { m_iWidth = sWidth; }
	void setHeight(int sHeight) { m_iHeight = sHeight; }
	void setStartFrame(int sStartFrame) { m_iStartFrame = sStartFrame; }
	void setTotalFrame(int sTotalFrame) { m_iTotalFrame = sTotalFrame; }
	void setUpdateStep(int sUpdateStep) { m_iUpdateStep = sUpdateStep; }
	void setRange(int sRange) { m_iRange = sRange; }
	
	CIYuv* getDepthVideoBuffer() { return m_pcDepthVideo; }
	CIYuv* getColorVideoBuffer() { return m_pcColorVideo; }
	cv::Mat* getDepthViewBGRBuffer() { return m_pcDepthViewImage; }
	cv::Mat* getColorViewBGRBuffer() { return m_pcColorViewImage; }

	bool allocateMem();

	void buildOneFrameColorBackground();
	void calcFinalColorBackground();

	void readDepthBackgroundImage(const std::string&);
	void showCurrentImage();
	void showColorBackgroundImage();
	void writeColorBackgroundImage(const std::string&);

	void testShowCountLineSum();

private:
	int m_iWidth;
	int m_iHeight;
	int m_iStartFrame;
	int m_iTotalFrame;
	int m_iUpdateStep;
	int m_iRange;

	CIYuv *m_pcDepthVideo;
	CIYuv *m_pcColorVideo;

	cv::Mat *m_pcDepthBackgroundImage;
	cv::Mat *m_pcColorBackgroundImage;
	cv::Mat *m_pcDepthViewImage;
	cv::Mat *m_pcColorViewImage;
	cv::Mat *m_pcTemporalColorImage;

	int *countNumber;
	int testsum1, testsum2;
};

