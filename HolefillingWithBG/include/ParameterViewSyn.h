#pragma once

#include <string>
#include "ParameterBase.h"

// �ɻ�����̳�����������;
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
	std::string getOutVideoName() { return m_cOutVideoName; }

private:
	UInt setup();

protected:
	int m_uiSourceWidth;
	int m_uiSourceHeight;
	int m_uiStartFrame;
	int m_uiNumberOfFrames;

	std::string m_cSourceVideoName;
	std::string m_cBackgroundImageName;
	std::string m_cOutVideoName;

};

