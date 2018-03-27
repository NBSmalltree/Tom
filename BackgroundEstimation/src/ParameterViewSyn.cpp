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
	m_pCfgLines[uiParLnCount++] = new ConfigLineInt("UpdateStep", &m_uiUpdateStep, 100);
	m_pCfgLines[uiParLnCount++] = new ConfigLineInt("SplitRange", &m_uiSplitRange, 5);

	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("DepthVideoName", &m_cDepthVideoName, "depth_test.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("ColorVideoName", &m_cColorVideoName, "color_test.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("OutputDepthBackgroundImageName", &m_cDepthBackgroundImageName, "depthbackground_test.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("OutputColorBackgroundImageName", &m_cColorBackgroundImageName, "colorbackground_test.yuv");

	m_pCfgLines[uiParLnCount++] = new ConfigLineInt("IsInpaint", &m_flagIsInpaint, 1);

	m_pCfgLines[uiParLnCount] = NULL;

	return uiParLnCount;
}
