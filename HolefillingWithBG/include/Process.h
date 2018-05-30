#ifndef Process_h__
#define Process_h__

#include "ParameterViewSyn.h"
#include "UsingBackground.h"
#include "DoubleViewFilling.h"
#include "yuv.h"

class CHoleFilling
{
public:
	CHoleFilling();
	~CHoleFilling();

	void Init(ParameterViewSyn cParameter);
	bool initUsingBackground();
	void releaseUsingBackground();
	bool initDoubleViewFilling();
	void releaseDoubleViewFilling();

	bool doHoleFilling();

	bool singleViewFillingWithBG();
	bool doubleViewFillingWithBG();

private:
	int m_iWidth;
	int m_iHeight;
	int m_iStartFrame;
	int m_iTotalFrame;

	int m_uiIsSingleViewFilling;

	//>SingleViewFilling Input Parameter
	std::string m_cSourceVideoName;
	std::string m_cSourceDepthName;
	std::string m_cBackgroundImageName;
	std::string m_cBackgroundDepthName;

	//>DoubleViewFilling Input Parameter
	std::string m_cLeftImageName;
	std::string m_cRightImageName;
	std::string m_cLeftDepthName;
	std::string m_cRightDepthName;

	std::string m_cBGImageName;
	std::string m_cBGDepthName;

	//>Output Parameter
	std::string m_cOutImageName;
	std::string m_cOutDepthName;

	FILE *fin_video;
	FILE *fin_depth;
	FILE *fin_backgroundcolor;
	FILE *fin_backgrounddepth;

	FILE *fin_leftcolor;
	FILE *fin_rghtcolor;
	FILE *fin_leftdepth;
	FILE *fin_rghtdepth;
	FILE *fin_bgcolor;
	FILE *fin_bgdepth;

	FILE *fout_color;
	FILE *fout_depth;

	CUsingBackground *m_pUsingBackground;
	CDoubleViewFilling *m_pDoubleViewFilling;
};

#endif // Process_h__
