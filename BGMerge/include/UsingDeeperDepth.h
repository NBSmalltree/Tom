#pragma once
#include "yuv.h"

class CUsingDeeperDepth
{
public:
	CUsingDeeperDepth();
	~CUsingDeeperDepth();

	void setWidth(int sWidth) { m_iWidth = sWidth; }
	void setHeight(int sHeight) { m_iHeight = sHeight; }
	void setStartFrame(int sStartFrame) { m_iStartFrame = sStartFrame; }
	void setTotalFrame(int sTotalFrame) { m_iTotalFrame = sTotalFrame; }

	bool allocateMem();
	void releaseMem();

	void doProcess();

	CIYuv* getLeftColorBuffer() { return m_pcLeftViewColor; }
	CIYuv* getRightColorBuffer() { return m_pcRightViewColor; }
	CIYuv* getLeftDepthBuffer() { return m_pcLeftViewDepth; }
	CIYuv* getRightDepthBuffer() { return m_pcRightViewDepth; }

	CIYuv* getOutColorBuffer() { return m_pcOutColor; }
	CIYuv* getOutDepthBuffer() { return m_pcOutDepth; }

private:
	int m_iWidth;
	int m_iHeight;
	int m_iStartFrame;
	int m_iTotalFrame;

	CIYuv *m_pcLeftViewColor;
	CIYuv *m_pcRightViewColor;
	CIYuv *m_pcLeftViewDepth;
	CIYuv *m_pcRightViewDepth;

	CIYuv *m_pcOutColor;
	CIYuv *m_pcOutDepth;
};

