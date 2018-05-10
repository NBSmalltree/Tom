#pragma once

#include <string>
#include "ParameterBase.h"

// 由基础类继承来的特殊类;
class ParameterViewSyn : public ParameterBase
{
public:
	ParameterViewSyn();
	~ParameterViewSyn();

	int Init(int argc,char** argv);
	int getSourceWidth() { return m_uiSourceWidth; }
	int getSourceHeight() { return m_uiSourceHeight; }
	int getStartFrame() { return m_uiStartFrame; }
	int getTotalFrame() { return m_uiNumberOfFrames; }

	std::string getLeftViewImageName() { return m_cLeftViewImageName; }
	std::string getRightViewImageName() { return m_cRightViewImageName; }
	std::string getLeftDepthMapName() { return m_cLeftDepthMapName; }
	std::string getRightDepthMapName() { return m_cRightDepthMapName; }

	std::string getOutputImageName() { return m_cOutputImageName; }
	std::string getOutputDepthMapName() { return m_cOutputDepthMapName; }

private:
	UInt setup();

protected:
	int m_uiSourceWidth;
	int m_uiSourceHeight;
	int m_uiStartFrame;
	int m_uiNumberOfFrames;

	std::string m_cLeftViewImageName;
	std::string m_cRightViewImageName;
	std::string m_cLeftDepthMapName;
	std::string m_cRightDepthMapName;

	std::string m_cOutputImageName;
	std::string m_cOutputDepthMapName;

};

