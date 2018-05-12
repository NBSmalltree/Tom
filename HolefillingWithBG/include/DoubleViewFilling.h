#pragma once
#include "yuv.h"


class CDoubleViewFilling
{
public:
	CDoubleViewFilling();
	~CDoubleViewFilling();

	void setWidth(int sWidth) { m_iWidth = sWidth; }
	void setHeight(int sHeight) { m_iHeight = sHeight; }
	void setStartFrame(int sStartFrame) { m_iStartFrame = sStartFrame; }
	void setTotalFrame(int sTotalFrame) { m_iTotalFrame = sTotalFrame; }

	bool allocateMem();
	void releaseMem();

	bool isHoleinColor(CIYuv *yuvBuffer, int h, int w);

	void mergeWithZBuffer(CIYuv *leftColor, CIYuv *leftDepth, CIYuv *rghtColor, CIYuv *rghtDepth, CIYuv *outColor, CIYuv *outDepth);
	void fillingWithZBuffer(CIYuv *inputColor, CIYuv *inputDepth, CIYuv *bgColor, CIYuv *bgDepth, CIYuv *outColor, CIYuv *outDepth);

	void doOneFrame();

	CIYuv* getLeftColorBuffer() { return m_pcLeftColor; }
	CIYuv* getRghtColorBuffer() { return m_pcRghtColor; }
	CIYuv* getLeftDepthBuffer() { return m_pcLeftDepth; }
	CIYuv* getRghtDepthBuffer() { return m_pcRghtDepth; }

	CIYuv* getBGColorBuffer() { return m_pcBGColor; }
	CIYuv* getBGDepthBuffer() { return m_pcBGDepth; }

	CIYuv* getOutColorBuffer() { return m_pcOutColor; }
	CIYuv* getOutDepthBuffer() { return m_pcOutDepth; }

private:
	int m_iWidth;
	int m_iHeight;
	int m_iStartFrame;
	int m_iTotalFrame;

	CIYuv *m_pcLeftColor;
	CIYuv *m_pcRghtColor;
	CIYuv *m_pcLeftDepth;
	CIYuv *m_pcRghtDepth;

	CIYuv *m_pcBGColor;
	CIYuv *m_pcBGDepth;

	CIYuv *m_pcOutColor;
	CIYuv *m_pcOutDepth;

	CIYuv *temp1;
	CIYuv *temp2;
};

