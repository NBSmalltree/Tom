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

	bool allocateMem();
	void releaseMem();

	void buildOneFrameColorBackground();
	void calcFinalColorBackground();

	void readDepthBackgroundImage(FILE*);
	void writeColorBackgroundImage(FILE*);

private:
	int m_iWidth;
	int m_iHeight;
	int m_iStartFrame;
	int m_iTotalFrame;
	int m_iUpdateStep;
	int m_iRange;

	CIYuv *m_pcDepthVideo;
	CIYuv *m_pcColorVideo;

	int *countNumber;
	int *countNumberUV;

	CIYuv *m_depthBackgroundYUV;
	CIYuv *m_colorBackgroundYUV;
	int **m_temporalColorSpaceY;
	int **m_temporalColorSpaceU;
	int **m_temporalColorSpaceV;

};

