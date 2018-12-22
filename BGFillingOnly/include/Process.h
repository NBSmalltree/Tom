#ifndef Process_h__
#define Process_h__

#include "ParameterViewSyn.h"
#include "UsingBackground.h"
#include "yuv.h"

class CHoleFilling
{
public:
	CHoleFilling();
	~CHoleFilling();

	void Init(ParameterViewSyn cParameter);
	bool initUsingBackground();
	void releaseUsingBackground();

	bool doHoleFilling();

	bool singleViewFillingWithBG();

private:
	int m_iWidth;
	int m_iHeight;
	int m_iStartFrame;
	int m_iTotalFrame;

	std::string m_cSourceVideoName;
	std::string m_cBackgroundImageName;

	std::string m_cOutImageName;

	FILE *fin_video;
	FILE *fin_backgroundcolor;

	FILE *fout_color;

	CUsingBackground *m_pUsingBackground;
};

#endif // Process_h__
