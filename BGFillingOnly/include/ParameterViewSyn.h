#pragma once

#include <string>
#include "ParameterBase.h"

//>由基础类继承来的特殊类
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

	std::string getSourceVideoName() { return m_cSourceVideoName; }
	std::string getBackgroundImageName() { return m_cBackgroundImageName; }

	std::string getOutImageName() { return m_cOutImageName; }

private:
	UInt setup();

protected:
	int m_uiSourceWidth;
	int m_uiSourceHeight;
	int m_uiStartFrame;
	int m_uiNumberOfFrames;

	std::string m_cSourceVideoName;
	std::string m_cBackgroundImageName;

	std::string m_cOutImageName;

};

