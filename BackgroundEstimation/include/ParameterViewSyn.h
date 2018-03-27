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
	int getSplitRange() { return m_uiSplitRange; }

	int getIsInpaint() { return m_flagIsInpaint; }

	std::string getDepthVideoName() { return m_cDepthVideoName; }
	std::string getColorVideoName() { return m_cColorVideoName; }
	std::string getDepthBackgroundImageName() { return m_cDepthBackgroundImageName; }
	std::string getColorBackgroundImageName() { return m_cColorBackgroundImageName; }

private:
	UInt setup();

protected:
	int m_uiSourceWidth;
	int m_uiSourceHeight;
	int m_uiStartFrame;
	int m_uiNumberOfFrames;
	int m_uiUpdateStep;
	int m_uiSplitRange;

	std::string m_cDepthVideoName;
	std::string m_cColorVideoName;

	std::string m_cDepthBackgroundImageName;
	std::string m_cColorBackgroundImageName;

	int m_flagIsInpaint;

};

