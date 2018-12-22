#include "Process.h"

CHoleFilling::CHoleFilling()
{
	m_iWidth = 1024;
	m_iHeight = 768;
	m_iStartFrame = 0;
	m_iTotalFrame = 100;

	fin_video = nullptr;
	fin_backgroundcolor = nullptr;

	fout_color = nullptr;
}

CHoleFilling::~CHoleFilling()
{
	if (fin_video)
		fclose(fin_video);
	if (fin_backgroundcolor)
		fclose(fin_backgroundcolor);

	if (fout_color)
		fclose(fout_color);
}

void CHoleFilling::Init(ParameterViewSyn cParameter)
{
	m_iWidth = cParameter.getSourceWidth();
	m_iHeight = cParameter.getSourceHeight();
	m_iStartFrame = cParameter.getStartFrame();
	m_iTotalFrame = cParameter.getTotalFrame();

	m_cSourceVideoName = cParameter.getSourceVideoName();
	m_cBackgroundImageName = cParameter.getBackgroundImageName();

	m_cOutImageName = cParameter.getOutImageName();
}

bool CHoleFilling::initUsingBackground()
{
	m_pUsingBackground = new CUsingBackground();

	m_pUsingBackground->setHeight(m_iHeight);
	m_pUsingBackground->setWidth(m_iWidth);
	m_pUsingBackground->setStartFrame(m_iStartFrame);
	m_pUsingBackground->setTotalFrame(m_iTotalFrame);

	if (!m_pUsingBackground->allocateMem())
		return false;

	if (fopen_s(&fin_video, m_cSourceVideoName.c_str(), "rb") ||
		fopen_s(&fin_backgroundcolor, m_cBackgroundImageName.c_str(), "rb")||
		fopen_s(&fout_color, m_cOutImageName.c_str(), "wb"))
		return false;

	return true;
}

void CHoleFilling::releaseUsingBackground()
{
	m_pUsingBackground->releaseMem();

	if (fin_video)
		fclose(fin_video);
	if (fin_backgroundcolor)
		fclose(fin_backgroundcolor);
	if (fout_color)
		fclose(fout_color);

	if (m_pUsingBackground != nullptr) {
		delete m_pUsingBackground;
		m_pUsingBackground = nullptr;
	}
}

bool CHoleFilling::doHoleFilling()
{
	if (!initUsingBackground()) {
		std::cout << "Initialize CUsingBackground Class Failure!" << std::endl;
		return false;
	}

	singleViewFillingWithBG();

	releaseUsingBackground();

	return true;
}

bool CHoleFilling::singleViewFillingWithBG()
{
	//>Read Background Image From File
	if (!(m_pUsingBackground->getBGColorBuffer()->ReadOneFrame(fin_backgroundcolor, 0))) {
		std::cout << "Set Frame Head Failure!" << std::endl;
		return false;
	}

	for (int n = m_iStartFrame; n < m_iStartFrame + m_iTotalFrame; n++) {

		//>Read One InVideo Frame From File
		if (!(m_pUsingBackground->getInVideoBuffer()->ReadOneFrame(fin_video, n))) {
			std::cout << "Set Frame Head Failure!" << std::endl;
			return false;
		}

		//>Do One Frame Processing
		m_pUsingBackground->doOneFrame();

		//>Write Target To File
		m_pUsingBackground->getOutVideoBuffer()->WriteOneFrame(fout_color);

	}

	return true;
}
