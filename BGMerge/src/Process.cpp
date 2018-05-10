#include "Process.h"

CMerge::CMerge()
{
	m_iWidth = 1024;
	m_iHeight = 768;
	m_iStartFrame = 0;
	m_iTotalFrame = 1;

	fin_LeftColor = nullptr;
	fin_RightColor = nullptr;
	fin_LeftDepth = nullptr;
	fin_RightDepth = nullptr;
	fout_Color = nullptr;
	fout_Depth = nullptr;
}

CMerge::~CMerge()
{
	if (fin_LeftColor)
		fclose(fin_LeftColor);
	if (fin_RightColor)
		fclose(fin_RightColor);
	if (fin_LeftDepth)
		fclose(fin_LeftDepth);
	if (fin_RightDepth)
		fclose(fin_RightDepth);
	if (fout_Color)
		fclose(fout_Color);
	if (fout_Depth)
		fclose(fout_Depth);

	if (m_pUsingDeeperDepth != nullptr) {
		delete m_pUsingDeeperDepth;
		m_pUsingDeeperDepth = nullptr;
	}
}

void CMerge::Init(ParameterViewSyn cParameter)
{
	m_iWidth = cParameter.getSourceWidth();
	m_iHeight = cParameter.getSourceHeight();
	m_iStartFrame = cParameter.getStartFrame();
	m_iTotalFrame = cParameter.getTotalFrame();

	m_cLeftViewImageName = cParameter.getLeftViewImageName();
	m_cRightViewImageName = cParameter.getRightViewImageName();
	m_cLeftDepthMapName = cParameter.getLeftDepthMapName();
	m_cRightDepthMapName = cParameter.getRightDepthMapName();

	m_cOutputImageName = cParameter.getOutputImageName();
	m_cOutputDepthMapName = cParameter.getOutputDepthMapName();

}

bool CMerge::initUsingDeeperDepth()
{
	m_pUsingDeeperDepth = new CUsingDeeperDepth();

	m_pUsingDeeperDepth->setHeight(m_iHeight);
	m_pUsingDeeperDepth->setWidth(m_iWidth);
	m_pUsingDeeperDepth->setStartFrame(m_iStartFrame);
	m_pUsingDeeperDepth->setTotalFrame(m_iTotalFrame);

	if (!m_pUsingDeeperDepth->allocateMem())
		return false;

	if (fopen_s(&fin_LeftColor, m_cLeftViewImageName.c_str(), "rb") ||
		fopen_s(&fin_RightColor, m_cRightViewImageName.c_str(), "rb") ||
		fopen_s(&fin_LeftDepth, m_cLeftDepthMapName.c_str(), "rb") ||
		fopen_s(&fin_RightDepth, m_cRightDepthMapName.c_str(), "rb") ||
		fopen_s(&fout_Color, m_cOutputImageName.c_str(), "wb")||
		fopen_s(&fout_Depth, m_cOutputDepthMapName.c_str(), "wb"))
		return false;

	return true;
}

void CMerge::releaseUsingDeeperDepth()
{
	m_pUsingDeeperDepth->releaseMem();

	if (fin_LeftColor)
		fclose(fin_LeftColor);
	if (fin_RightColor)
		fclose(fin_RightColor);
	if (fin_LeftDepth)
		fclose(fin_LeftDepth);
	if (fin_RightDepth)
		fclose(fin_RightDepth);
	if (fout_Color)
		fclose(fout_Color);
	if (fout_Depth)
		fclose(fout_Depth);

	if (m_pUsingDeeperDepth != nullptr) {
		delete m_pUsingDeeperDepth;
		m_pUsingDeeperDepth = nullptr;
	}
}

bool CMerge::doMerge()
{
	//>Read Background Image From File
	if (!(m_pUsingDeeperDepth->getLeftColorBuffer()->ReadOneFrame(fin_LeftColor, 0)&&
		m_pUsingDeeperDepth->getRightColorBuffer()->ReadOneFrame(fin_RightColor, 0) &&
		m_pUsingDeeperDepth->getLeftDepthBuffer()->ReadOneFrame(fin_LeftDepth, 0) &&
		m_pUsingDeeperDepth->getRightDepthBuffer()->ReadOneFrame(fin_RightDepth, 0))) {
		std::cout << "Set Frame Head Failure!" << std::endl;
		return false;
	}

	for (int n = m_iStartFrame; n < m_iStartFrame+m_iTotalFrame; n++) {

		//>Do One Frame Processing
		m_pUsingDeeperDepth->doProcess();

		//>Write Target To File
		m_pUsingDeeperDepth->getOutColorBuffer()->WriteOneFrame(fout_Color);
		m_pUsingDeeperDepth->getOutDepthBuffer()->WriteOneFrame(fout_Depth);

	}

	return true;
}
