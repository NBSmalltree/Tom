#pragma once
#include "yuv.h"

class CUsingBackground
{
public:
	CUsingBackground();
	~CUsingBackground();

	void setWidth(int sWidth) { m_iWidth = sWidth; }
	void setHeight(int sHeight) { m_iHeight = sHeight; }
	void setStartFrame(int sStartFrame) { m_iStartFrame = sStartFrame; }
	void setTotalFrame(int sTotalFrame) { m_iTotalFrame = sTotalFrame; }

	bool allocateMem();
	void releaseMem();

	bool isHoleinColor(CIYuv *yuvBuffer, int h, int w);
	void doOneFrame();

	CIYuv* getInVideoBuffer() { return m_pcInVideo; }
	CIYuv* getBGColorBuffer() { return m_pcBGColor; }
	CIYuv* getOutVideoBuffer() { return m_pcOutVideo; }

private:
	int m_iWidth;
	int m_iHeight;
	int m_iStartFrame;
	int m_iTotalFrame;

	CIYuv *m_pcInVideo;
	CIYuv *m_pcBGColor;
	CIYuv *m_pcOutVideo;
};

