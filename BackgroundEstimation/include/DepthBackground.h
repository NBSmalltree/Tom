#include <iostream>
#include <fstream>
#include "yuv.h"

class CDepthMap
{
public:
	CDepthMap();
	CDepthMap(int h, int w, int step);
	~CDepthMap();

	BYTE **DEPTHMAP;

	bool saveOneFrame(CIYuv *yuvBuffer, int n);

	//>¡¾Test¡¿
	void printTestMemory(int rowstart, int colstart, int width, int frame);
	void writeOnePixeltoFile(int x, int y, int frame);

private:
	int m_iWidth;
	int m_iHeight;
	int m_iUpdateStep;

	bool allocate_mem();
	void release_mem();

	BYTE *pBuffer;  //!> 1D array for the DEPTHMAP
};

class CDepthBackground
{
public:
	CDepthBackground();
	~CDepthBackground();

	void setWidth(int sWidth) { m_iWidth = sWidth; }
	void setHeight(int sHeight) { m_iHeight = sHeight; }
	void setStartFrame(int sStartFrame) { m_iStartFrame = sStartFrame; }
	void setTotalFrame(int sTotalFrame) { m_iTotalFrame = sTotalFrame; }
	void setUpdateStep(int sUpdateStep) { m_iUpdateStep = sUpdateStep; }

	bool allocateMem();
	void releaseMem();

	CIYuv* getFrameBuffer() { return m_pcVideo; }
	cv::Mat* getBgrBuffer() { return m_pcBgr; }
	CDepthMap* getDepthMapBuffer() { return m_pcDepthMap; }
	BYTE* getCertainPixelDepthMapBuffer() { return m_pcCertainPixelDepthMap; }

	bool saveOneDepthMap(CIYuv *yuvBuffer, int n);
	void extractCertainPixelDepthMap(int x, int y, int startframe);

	//>¡¾Test¡¿
	void showCurrentImage();
	bool writeCurrentImage();
	void showDepthMapTestMemory(int rowstart, int colstart, int width, int frame);
	void writeOnePixeltoFile(int x, int y, int frame);

private:
	int m_iWidth;
	int m_iHeight;
	int m_iStartFrame;
	int m_iTotalFrame;
	int m_iUpdateStep;

	CIYuv *m_pcVideo;
	cv::Mat *m_pcBgr;
	CDepthMap *m_pcDepthMap;

	BYTE *m_pcCertainPixelDepthMap;

};

