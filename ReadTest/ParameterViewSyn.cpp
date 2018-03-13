#define _CRT_SECURE_NO_WARNINGS 
#include "ParameterViewSyn.h"
#include "math.h"

CCameraParameters::CCameraParameters()
{
	int i, j;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			m_fIntrinsicMatrix[j][i] = 0.0;
			m_fExtrinsicMatrix[j][i] = 0.0;
		}
		m_fTranslationVector[i] = 0.0;
	}
}

CCameraParameters& CCameraParameters::operator = (CCameraParameters& src)
{
	int i, j;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			m_fIntrinsicMatrix[j][i] = src.m_fIntrinsicMatrix[j][i];
			m_fExtrinsicMatrix[j][i] = src.m_fExtrinsicMatrix[j][i];
		}
		m_fTranslationVector[i] = src.m_fTranslationVector[i];
	}

	return (*this);
}

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
	xReadFromCommandLine(argc, argv);

	xPrintParam();

	release();

	xReadCameraParameters();

	

	return 1;
}

UInt ParameterViewSyn::setup()
{
	int uiParLnCount = 0;

	m_pCfgLines[uiParLnCount++] = new ConfigLineInt("SourceWidth", &m_uiSourceWidth, 1024);
	m_pCfgLines[uiParLnCount++] = new ConfigLineInt("SourceHeight", &m_uiSourceHeight, 768);
	m_pCfgLines[uiParLnCount++] = new ConfigLineInt("TotalNumberOfFrames", &m_uiNumberOfFrames, 100);
	m_pCfgLines[uiParLnCount++] = new ConfigLineInt("StartFrame", &m_uiStartFrame, 0);

	m_pCfgLines[uiParLnCount++] = new ConfigLineDbl("LeftNearestDepthValue", &m_dLeftNearestDepthValue, 0.0);
	m_pCfgLines[uiParLnCount++] = new ConfigLineDbl("LeftFarthestDepthValue", &m_dLeftFarthestDepthValue, 0.0);
	m_pCfgLines[uiParLnCount++] = new ConfigLineDbl("RightNearestDepthValue", &m_dRightNearestDepthValue, 0.0);
	m_pCfgLines[uiParLnCount++] = new ConfigLineDbl("RightFarthestDepthValue", &m_dRightFarthestDepthValue, 0.0);

	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("CameraParameterFile", &m_cCameraParameterFile, "cam_param.txt");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("LeftCameraName", &m_cLeftCameraName, "param_dog38");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("VirtualCameraName", &m_cVirtualCameraName, "param_dog39");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("RightCameraName", &m_cRightCameraName, "param_dog41");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("LeftViewImageName", &m_cLeftViewImageName, "dog038.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("RightViewImageName", &m_cRightViewImageName, "dog041.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("LeftDepthMapName", &m_cLeftDepthMapName, "depth_dog038.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("RightDepthMapName", &m_cRightDepthMapName, "depth_dog041.yuv");
	m_pCfgLines[uiParLnCount++] = new ConfigLineStr("OutputVirtualViewImageName", &m_cOutputVirViewImageName, "dog_virtual039.yuv");

	m_pCfgLines[uiParLnCount] = NULL;

	return uiParLnCount;
}

UInt
ParameterViewSyn::xReadCameraParameters()
{
	int   i;
	const char* cameraId[3];
	char id[255];  // To store the cam/view id temporally
	int read = 0;
	double gomi[2];
	int   found;
	FILE *fp;

	if ((fp = fopen(m_cCameraParameterFile.c_str(), "rt")) == NULL)
	{
		fprintf(stderr, "Can't open camera parameter file: %s\n", m_cCameraParameterFile.c_str());
		return 0;
	}

	cameraId[0] = m_cLeftCameraName.c_str();
	cameraId[1] = m_cVirtualCameraName.c_str();
	cameraId[2] = m_cRightCameraName.c_str();

	for (i = 0; i < 3; i++)
	{
		if (fseek(fp, 0, SEEK_SET)) return 0;

		found = 0;
		while (fscanf(fp, "%s", id) == 1)
		{
			read = 0;
			if (strcmp(cameraId[i], id) == 0)
			{
				read += fscanf(fp, "%lf %lf %lf", &m_camParam[i].m_fIntrinsicMatrix[0][0], &m_camParam[i].m_fIntrinsicMatrix[0][1], &m_camParam[i].m_fIntrinsicMatrix[0][2]);
				read += fscanf(fp, "%lf %lf %lf", &m_camParam[i].m_fIntrinsicMatrix[1][0], &m_camParam[i].m_fIntrinsicMatrix[1][1], &m_camParam[i].m_fIntrinsicMatrix[1][2]);
				read += fscanf(fp, "%lf %lf %lf", &m_camParam[i].m_fIntrinsicMatrix[2][0], &m_camParam[i].m_fIntrinsicMatrix[2][1], &m_camParam[i].m_fIntrinsicMatrix[2][2]);
				read += fscanf(fp, "%lf %lf", &gomi[0], &gomi[1]);
				read += fscanf(fp, "%lf %lf %lf %lf", &m_camParam[i].m_fExtrinsicMatrix[0][0], &m_camParam[i].m_fExtrinsicMatrix[0][1], &m_camParam[i].m_fExtrinsicMatrix[0][2], &m_camParam[i].m_fTranslationVector[0]);
				read += fscanf(fp, "%lf %lf %lf %lf", &m_camParam[i].m_fExtrinsicMatrix[1][0], &m_camParam[i].m_fExtrinsicMatrix[1][1], &m_camParam[i].m_fExtrinsicMatrix[1][2], &m_camParam[i].m_fTranslationVector[1]);
				read += fscanf(fp, "%lf %lf %lf %lf", &m_camParam[i].m_fExtrinsicMatrix[2][0], &m_camParam[i].m_fExtrinsicMatrix[2][1], &m_camParam[i].m_fExtrinsicMatrix[2][2], &m_camParam[i].m_fTranslationVector[2]);
				if (read != 23) return 0;
				found = 1;
			}
		}

		if (found == 0)
		{
			printf("Camera \"%s\" is not found in the camera parameter file.\n", cameraId[i]);
			return 0;
		}
	}

	m_FocalLength = m_camParam[0].m_fIntrinsicMatrix[0][0];
	m_dLeftBaselineDistance = abs(m_camParam[0].m_fTranslationVector[0] - m_camParam[1].m_fTranslationVector[0]);
	m_dRightBaselineDistance = abs(m_camParam[1].m_fTranslationVector[0] - m_camParam[2].m_fTranslationVector[0]);

	m_ParameterLeftP = m_dLeftBaselineDistance * m_FocalLength / 255.0 * (1 / m_dLeftNearestDepthValue - 1 / m_dLeftFarthestDepthValue);
	m_Parameter_LeftQ = m_dLeftBaselineDistance * m_FocalLength / m_dLeftFarthestDepthValue;
	m_ParameterRightP = m_dRightBaselineDistance * m_FocalLength / 255.0 * (1 / m_dRightNearestDepthValue - 1 / m_dRightFarthestDepthValue);
	m_Parameter_RightQ = m_dRightBaselineDistance * m_FocalLength / m_dRightFarthestDepthValue;

	return 1;
}

void ParameterViewSyn::showParameter() {
	printf("\n\n\n");
	printf("This is showParameter\n\n");
	printf("SourceWidth:%d\n", m_uiSourceWidth);
	printf("SourceHeight:%d\n", m_uiSourceHeight);
	printf("StartFrame:%d\n", m_uiStartFrame);
	printf("TotalNumberOfFrames:%d\n", m_uiNumberOfFrames);

	printf("LeftNearestDepthValue:%lf\n", m_dLeftNearestDepthValue);
	printf("LeftFarthestDepthValue:%lf\n", m_dLeftFarthestDepthValue);
	printf("RightNearestDepthValue:%lf\n", m_dRightNearestDepthValue);
	printf("RightFarthestDepthValue:%lf\n", m_dRightFarthestDepthValue);

	printf("FocalLength:%lf\n", m_FocalLength);
	printf("LeftBaselineDistance:%lf\n", m_dLeftBaselineDistance);
	printf("RightBaselineDistance:%lf\n", m_dRightBaselineDistance);

	printf("m_ParameterLeftP:%lf\n", m_ParameterLeftP);
	printf("m_Parameter_LeftQ:%lf\n", m_Parameter_LeftQ);
	printf("m_ParameterRightP:%lf\n", m_ParameterRightP);
	printf("m_Parameter_RightQ:%lf\n", m_Parameter_RightQ);
}
