#include "Process.h"

CHoleFilling::CHoleFilling()
{
	m_iWidth = 1024;
	m_iHeight = 768;
	m_iStartFrame = 0;
	m_iTotalFrame = 100;

	m_uiIsSingleViewFilling = 1;

	fin_video = nullptr;
	fin_background = nullptr;

	fin_leftcolor = nullptr;
	fin_rghtcolor = nullptr;
	fin_leftdepth = nullptr;
	fin_rghtdepth = nullptr;
	fin_bgcolor = nullptr;
	fin_bgdepth = nullptr;

	fout_color = nullptr;
	fout_depth = nullptr;
}

CHoleFilling::~CHoleFilling()
{
	if (fin_video)
		fclose(fin_video);
	if (fin_background)
		fclose(fin_background);

	if (fin_leftcolor)
		fclose(fin_leftcolor);
	if (fin_rghtcolor)
		fclose(fin_rghtcolor);
	if (fin_leftdepth)
		fclose(fin_leftdepth);
	if (fin_rghtdepth)
		fclose(fin_rghtdepth);
	if (fin_bgcolor)
		fclose(fin_bgcolor);
	if (fin_bgdepth)
		fclose(fin_bgdepth);

	if (fout_color)
		fclose(fout_color);
	if (fout_depth)
		fclose(fout_depth);
}

void CHoleFilling::Init(ParameterViewSyn cParameter)
{
	m_iWidth = cParameter.getSourceWidth();
	m_iHeight = cParameter.getSourceHeight();
	m_iStartFrame = cParameter.getStartFrame();
	m_iTotalFrame = cParameter.getTotalFrame();

	m_uiIsSingleViewFilling = cParameter.getIsSingleViewFilling();

	//>SingleViewFilling Input Parameter
	m_cSourceVideoName = cParameter.getSourceVideoName();
	m_cBackgroundImageName = cParameter.getBackgroundImageName();

	//>DoubleViewFilling Input Parameter
	m_cLeftImageName = cParameter.getLeftImageName();
	m_cRightImageName = cParameter.getRightImageName();
	m_cLeftDepthName = cParameter.getLeftDepthName();
	m_cRightDepthName = cParameter.getRightDepthName();
	m_cBGImageName = cParameter.getBGImageName();
	m_cBGDepthName = cParameter.getBGDepthName();

	//>Output Parameter
	m_cOutImageName = cParameter.getOutImageName();
	m_cOutDepthName = cParameter.getOutDepthName();
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
		fopen_s(&fin_background, m_cBackgroundImageName.c_str(), "rb")||
		fopen_s(&fout_color, m_cOutImageName.c_str(), "wb"))
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
	if (fout_color)
		fclose(fout_color);

	if (m_pUsingBackground != nullptr) {
		delete m_pUsingBackground;
		m_pUsingBackground = nullptr;
	}
}

bool CHoleFilling::initDoubleViewFilling()
{
	m_pDoubleViewFilling = new CDoubleViewFilling();

	m_pDoubleViewFilling->setHeight(m_iHeight);
	m_pDoubleViewFilling->setWidth(m_iWidth);
	m_pDoubleViewFilling->setStartFrame(m_iStartFrame);
	m_pDoubleViewFilling->setTotalFrame(m_iTotalFrame);

	if (!m_pDoubleViewFilling->allocateMem())
		return false;

	if (fopen_s(&fin_leftcolor, m_cLeftImageName.c_str(), "rb") ||
		fopen_s(&fin_rghtcolor, m_cRightImageName.c_str(), "rb") ||
		fopen_s(&fin_leftdepth, m_cLeftDepthName.c_str(), "rb") ||
		fopen_s(&fin_rghtdepth, m_cRightDepthName.c_str(), "rb") ||
		fopen_s(&fin_bgcolor, m_cBGImageName.c_str(), "rb") ||
		fopen_s(&fin_bgdepth, m_cBGDepthName.c_str(), "rb") ||
		fopen_s(&fout_color, m_cOutImageName.c_str(), "wb") ||
		fopen_s(&fout_depth, m_cOutDepthName.c_str(), "wb"))
		return false;

	return true;
}

void CHoleFilling::releaseDoubleViewFilling()
{
	m_pDoubleViewFilling->releaseMem();

	if (fin_leftcolor)
		fclose(fin_leftcolor);
	if (fin_rghtcolor)
		fclose(fin_rghtcolor);
	if (fin_leftdepth)
		fclose(fin_leftdepth);
	if (fin_rghtdepth)
		fclose(fin_rghtdepth);

	if (fin_bgcolor)
		fclose(fin_bgcolor);
	if (fin_bgdepth)
		fclose(fin_bgdepth);

	if (fout_color)
		fclose(fout_color);
	if (fout_depth)
		fclose(fout_depth);

	if (m_pDoubleViewFilling != nullptr) {
		delete m_pDoubleViewFilling;
		m_pDoubleViewFilling = nullptr;
	}
}

bool CHoleFilling::doHoleFilling()
{
	if (m_uiIsSingleViewFilling) {
		if (!initUsingBackground()) {
			std::cout << "Initialize CUsingBackground Class Failure!" << std::endl;
			return false;
		}

		singleViewFillingWithBG();

		releaseUsingBackground();
	}
	else {
		if (!initDoubleViewFilling()) {
			std::cout << "Initialize CDoubleViewFilling Class Failure!" << std::endl;
			return false;
		}

		doubleViewFillingWithBG();

		releaseDoubleViewFilling();
	}

	return true;
}

bool CHoleFilling::singleViewFillingWithBG()
{
	//>Read Background Image From File
	if (!m_pUsingBackground->getBackgroundBuffer()->ReadOneFrame(fin_background, 0)) {
		std::cout << "Set Frame Head Failure!" << std::endl;
		return false;
	}

	for (int n = m_iStartFrame; n < m_iStartFrame + m_iTotalFrame; n++) {

		//>Read One InVideo Frame From File
		if (!m_pUsingBackground->getInVideoBuffer()->ReadOneFrame(fin_video, n)) {
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

bool CHoleFilling::doubleViewFillingWithBG()
{
	//>Read Background Image From File
	if (!(m_pDoubleViewFilling->getBGColorBuffer()->ReadOneFrame(fin_bgcolor, 0) &&
		m_pDoubleViewFilling->getBGDepthBuffer()->ReadOneFrame(fin_bgdepth, 0))) {
		std::cout << "Set Frame Head Failure!" << std::endl;
		return false;
	}

	for (int n = m_iStartFrame; n < m_iStartFrame + m_iTotalFrame; n++) {

		//>Read One InVideo Frame From File
		if (!(m_pDoubleViewFilling->getLeftColorBuffer()->ReadOneFrame(fin_leftcolor, n) &&
			m_pDoubleViewFilling->getLeftDepthBuffer()->ReadOneFrame(fin_leftdepth, n) &&
			m_pDoubleViewFilling->getRghtColorBuffer()->ReadOneFrame(fin_rghtcolor, n) && 
			m_pDoubleViewFilling->getRghtDepthBuffer()->ReadOneFrame(fin_rghtdepth, n))) {
			std::cout << "Set Frame Head Failure!" << std::endl;
			return false;
		}

		//>Do One Frame Processing
		m_pDoubleViewFilling->doOneFrame();

		//>Write Target To File
		m_pDoubleViewFilling->getOutColorBuffer()->WriteOneFrame(fout_color);
		m_pDoubleViewFilling->getOutDepthBuffer()->WriteOneFrame(fout_depth);

	}

	return true;
}
