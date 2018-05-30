#pragma once
#include "yuv.h"
#include "opencv2/photo/photo.hpp"

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
	void fillDepthStreamHole(CIYuv *yuvBuffer);
	void inpaint(CIYuv *yuvBuffer);

	void doOneFrame();

	CIYuv* getInVideoBuffer() { return m_pcInVideo; }
	CIYuv* getInDepthBuffer() { return m_pcInDepth; }
	CIYuv* getBGColorBuffer() { return m_pcBGColor; }
	CIYuv* getBGDepthBuffer() { return m_pcBGDepth; }
	CIYuv* getOutVideoBuffer() { return m_pcOutVideo; }

private:
	int m_iWidth;
	int m_iHeight;
	int m_iStartFrame;
	int m_iTotalFrame;

	CIYuv *m_pcInVideo;
	CIYuv *m_pcInDepth;
	CIYuv *m_pcBGColor;
	CIYuv *m_pcBGDepth;
	CIYuv *m_pcOutVideo;

	cv::Mat *m_pcMatBuffer;
};

