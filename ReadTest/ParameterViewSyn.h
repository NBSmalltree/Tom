#pragma once

#include <string>
#include "ParameterBase.h"

class CCameraParameters
{
public:
	double m_fIntrinsicMatrix[3][3];
	double m_fExtrinsicMatrix[3][3];
	double m_fTranslationVector[3];

	CCameraParameters();
	~CCameraParameters() {};

	CCameraParameters& operator = (CCameraParameters& src);
};

class ParameterViewSyn : public ParameterBase
{
public:
	ParameterViewSyn();
	~ParameterViewSyn();

	int Init(int argc,char** argv);
	void showParameter();

private:
	UInt setup();

	UInt xReadCameraParameters();

protected:
	int m_uiSourceWidth;
	int m_uiSourceHeight;
	int m_uiStartFrame;
	int m_uiNumberOfFrames;
	
	double m_dLeftNearestDepthValue;
	double m_dLeftFarthestDepthValue;
	double m_dRightNearestDepthValue;
	double m_dRightFarthestDepthValue;
	
	std::string m_cCameraParameterFile;

	std::string m_cLeftCameraName;
	std::string m_cRightCameraName;
	std::string m_cVirtualCameraName;

	std::string m_cLeftViewImageName;
	std::string m_cLeftDepthMapName;
	std::string m_cRightViewImageName;
	std::string m_cRightDepthMapName;

	std::string m_cOutputVirViewImageName;

	// Camera parameters
	CCameraParameters m_camParam[3]; //!> 0: Left, 1: Center, 2: Right
	Double m_FocalLength;
	Double m_dLeftBaselineDistance;
	Double m_dRightBaselineDistance;
	Double m_ParameterLeftP;
	Double m_Parameter_LeftQ;
	Double m_ParameterRightP;
	Double m_Parameter_RightQ;
};

