#ifndef BackgroundEstimation_h__
#define BackgroundEstimation_h__

#include <opencv2/highgui/highgui.hpp>
#include "ParameterViewSyn.h"
#include "yuv.h"

class CDepthMap
{
public:
	CDepthMap();
	CDepthMap(int h, int w, int step);
	~CDepthMap();

	BYTE **DEPTHMAP;

	void printTestMemory(int rowstart, int colstart, int width, int frame);

	bool writeOneFrame(CIYuv *yuvBuffer, int n);

private:
	int m_iWidth;
	int m_iHeight;
	int m_iUpdateStep;

	bool allocate_mem();
	void release_mem();

	BYTE *pBuffer;  //!> 1D array for the DEPTHMAP
};

class CBackgroundEstimation
{
public:
	CBackgroundEstimation();
	~CBackgroundEstimation();

	bool Init(ParameterViewSyn cParameter);

	CIYuv* getFrameBuffer() { return m_pcVideo; }
	cv::Mat* getBgrBuffer() { return m_pcBgr; }

	void showCurrentImage();
	bool writeCurrentImage();

	void showDepthMapTestMemory(int rowstart, int colstart, int width, int frame);

	bool writeOneDepthMap(CIYuv *yuvBuffer, int n); 


private:
	int m_iWidth;
	int m_iHeight;
	int m_iFrameNumber;
	int m_iTotalNumber;
	int m_iUpdateStep;

	CIYuv *m_pcVideo;
	cv::Mat *m_pcBgr;
	CDepthMap *m_pcDepthMap;
	
};
#endif // BackgroundEstimation_h__

