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

	int getIsSingleViewFilling() { return m_uiIsSingleViewFilling; }

	//>SingleViewFilling
	std::string getSourceVideoName() { return m_cSourceVideoName; }
	std::string getBackgroundImageName() { return m_cBackgroundImageName; }

	//>DoubleViewFilling
	std::string getLeftImageName() { return m_cLeftImageName; }
	std::string getRightImageName() { return m_cRightImageName; }
	std::string getLeftDepthName() { return m_cLeftDepthName; }
	std::string getRightDepthName() { return m_cRightDepthName; }

	std::string getBGImageName() { return m_cBGImageName; }
	std::string getBGDepthName() { return m_cBGDepthName; }

	//>Output
	std::string getOutImageName() { return m_cOutImageName; }
	std::string getOutDepthName() { return m_cOutDepthName; }

private:
	UInt setup();

protected:
	int m_uiSourceWidth;
	int m_uiSourceHeight;
	int m_uiStartFrame;
	int m_uiNumberOfFrames;

	int m_uiIsSingleViewFilling;

	//>SingleViewFilling Input Parameter
	std::string m_cSourceVideoName;
	std::string m_cBackgroundImageName;

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

};

