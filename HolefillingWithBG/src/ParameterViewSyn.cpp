#define _CRT_SECURE_NO_WARNINGS 
#include "ParameterViewSyn.h"
#include "math.h"

ParameterViewSyn::ParameterViewSyn()
{
}


ParameterViewSyn::~ParameterViewSyn()
{
}

int ParameterViewSyn::Init(int argc, char ** argv)
{
	if (argc < 2) return -1;

	std::string cFilename = argv[1];

	setup();
	
	xReadFromFile(cFilename);

	xPrintParam();

	release();

	return 1;
}

UInt ParameterViewSyn::setup()
{
	int uiParLnCount = 0;

	m_pCfgLines[uiParLnCount++] = new ConfigLineInt("SourceWidth", &m_uiSourceWidth, 1024);
	m_pCfgLines[uiParLnCount++] = new ConfigLineInt("SourceHeight", &m_uiSourceHeight, 768);
	m_pCfgLines[uiParLnCount++] = new ConfigLineInt("TotalNumberOfFrames", &m_uiNumberOfFrames, 100);
	m_pCfgLines[uiParLnCount++] = new ConfigLineInt("StartFrame", &m_uiStartFrame, 0);

	m_pCfgLines[uiParLnCount++] = new ConfigLineInt("IsSingleViewFilling", &m_uiIsSingleViewFilling, 1);

	//>SingleViewFilling Input Parameter
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("SourceVideoName", &m_cSourceVideoName, "test.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("BackgroundImageName", &m_cBackgroundImageName, "ColorBackground.yuv");

	//>DoubleViewFilling Input Parameter
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("LeftImageName", &m_cLeftImageName, "leftcolor.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("RightImageName", &m_cRightImageName, "rightcolor.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("LeftDepthName", &m_cLeftDepthName, "leftdepth.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("RightDepthName", &m_cRightDepthName, "rightdepth.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("BGImageName", &m_cBGImageName, "leftdepth.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("BGDepthName", &m_cBGDepthName, "rightdepth.yuv");

	//>Output Parameter
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("OutputImageName", &m_cOutImageName, "outImage.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("OutputDepthName", &m_cOutDepthName, "outDepth.yuv");

	m_pCfgLines[uiParLnCount] = NULL;

	return uiParLnCount;
}
