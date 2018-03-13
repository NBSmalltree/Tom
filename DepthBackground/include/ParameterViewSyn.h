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
	int getUpdateStep() { return m_uiUpdateStep; }

	std::string getVideoName() { return m_cVideoName; }

private:
	UInt setup();

protected:
	int m_uiSourceWidth;
	int m_uiSourceHeight;
	int m_uiStartFrame;
	int m_uiNumberOfFrames;
	int m_uiUpdateStep;

	std::string m_cVideoName;

	std::string m_cOutputBackgroundImageName;

};

