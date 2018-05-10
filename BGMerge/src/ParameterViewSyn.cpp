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

	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("LeftViewImageName", &m_cLeftViewImageName, "leftviewcolor.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("RightViewImageName", &m_cRightViewImageName, "rightviewcolor.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("LeftDepthMapName", &m_cLeftDepthMapName, "leftviewdepth.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("RightDepthMapName", &m_cRightDepthMapName, "rightviewdepth.yuv");

	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("OutputImageName", &m_cOutputImageName, "BGVirtualViewColor.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("OutputDepthMapName", &m_cOutputDepthMapName, "BGVirtualViewDepth.yuv");

	m_pCfgLines[uiParLnCount] = NULL;

	return uiParLnCount;
}
