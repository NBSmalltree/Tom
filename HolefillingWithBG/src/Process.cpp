#include "Process.h"

CHoleFilling::CHoleFilling()
{
	m_iWidth = 1024;
	m_iHeight = 768;
	m_iStartFrame = 0;
	m_iTotalFrame = 100;

	fin_video = nullptr;
	fin_background = nullptr;
	fout_video = nullptr;
}

CHoleFilling::~CHoleFilling()
{
	if (fin_video)
		fclose(fin_video);
	if (fin_background)
		fclose(fin_background);
	if (fout_video)
		fclose(fout_video);
}

void CHoleFilling::Init(ParameterViewSyn cParameter)
{
	m_iWidth = cParameter.getSourceWidth();
	m_iHeight = cParameter.getSourceHeight();
	m_iStartFrame = cParameter.getStartFrame();
	m_iTotalFrame = cParameter.getTotalFrame();

	m_cSourceVideoName = cParameter.getSourceVideoName();
	m_cBackgroundImageName = cParameter.getBackgroundImageName();
	m_cOutVideoName = cParameter.getOutVideoName();
}

bool CHoleFilling::initUsingBackground()
{
	m_pUsingBackground = new CUsingBackground();

	m_pUsingBackground->setHeight(m_iHeight);
	m_pUsingBackground->setWidth(m_iWidth);
	m_pUsingBackground->setStartFrame(m_iStartFrame);
	m_pUsingBackground->setTotalFrame(m_iTotalFrame);

	if (m_pUsingBackground->allocateMem())
		return false;

	if (fopen_s(&fin_video, m_cSourceVideoName.c_str(), "rb") ||
		fopen_s(&fin_background, m_cBackgroundImageName.c_str(), "rb")||
		fopen_s(&fout_video, m_cOutVideoName.c_str(), "wb"))
		return false;

	return true;
}

void CHoleFilling::releaseUsingBackground()
{
	m_pUsingBackground->releaseMem();

	if (fin_video)
		fclose(fin_video);
	if (fin_background)
		fclose(fin_background);
	if (fout_video)
		fclose(fout_video);

	if (m_pUsingBackground != nullptr) {
		delete m_pUsingBackground;
		m_pUsingBackground = nullptr;
	}
}

bool CHoleFilling::doHoleFilling()
{
	//>Read Background Image From File
	if (!m_pUsingBackground->getBackgroundBuffer()->ReadOneFrame(fin_background, 0)) {
		std::cout << "Set Frame Head Failure!" << std::endl;
		return false;
	}

	for (int n = m_iStartFrame; n < m_iStartFrame+m_iTotalFrame; n++) {

		//>Read One InVideo Frame From File
		if (!m_pUsingBackground->getInVideoBuffer()->ReadOneFrame(fin_background, 0)) {
			std::cout << "Set Frame Head Failure!" << std::endl;
			return false;
		}

		//>Do One Frame Processing
		m_pUsingBackground->doOneFrame();

		//>Write Target To File
		m_pUsingBackground->getOutVideoBuffer()->WriteOneFrame(fout_video);

	}

	return true;
}
