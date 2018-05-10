#ifndef Process_h__
#define Process_h__

#include "ParameterViewSyn.h"
#include "yuv.h"
#include "UsingDeeperDepth.h"

class CMerge
{
public:
	CMerge();
	~CMerge();

	void Init(ParameterViewSyn cParameter);
	bool initUsingDeeperDepth();
	void releaseUsingDeeperDepth();

	bool doMerge();

private:
	int m_iWidth;
	int m_iHeight;
	int m_iStartFrame;
	int m_iTotalFrame;

	std::string m_cLeftViewImageName;
	std::string m_cRightViewImageName;
	std::string m_cLeftDepthMapName;
	std::string m_cRightDepthMapName;

	std::string m_cOutputImageName;
	std::string m_cOutputDepthMapName;

	FILE *fin_LeftColor;
	FILE *fin_RightColor;
	FILE *fin_LeftDepth;
	FILE *fin_RightDepth;

	FILE *fout_Color;
	FILE *fout_Depth;

	CUsingDeeperDepth *m_pUsingDeeperDepth;
;

};

#endif // Process_h__
