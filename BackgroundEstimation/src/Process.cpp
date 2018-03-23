#include "Process.h"

CBackgroundEstimation::CBackgroundEstimation()
{
	m_iWidth = 1024;
	m_iHeight = 768;
	m_iStartFrame = 0;
	m_iTotalFrame = 100;
	m_iUpdateStep = 10;

	fin_depth = NULL;
	fin_color = NULL;
}

CBackgroundEstimation::~CBackgroundEstimation()
{
	if(fin_depth)
		fclose(fin_depth);
	if (fin_color)
		fclose(fin_color);

	if (m_pDepthBackground != NULL) delete m_pDepthBackground;
	if (m_pColorBackground != NULL) delete m_pColorBackground;

	if (m_pDepthBackgroundImage != NULL) {
		m_pDepthBackgroundImage->release();
		delete m_pDepthBackgroundImage;
	}
}

void CBackgroundEstimation::Init(ParameterViewSyn cParameter)
{
	m_iWidth = cParameter.getSourceWidth();
	m_iHeight = cParameter.getSourceHeight();
	m_iStartFrame = cParameter.getStartFrame();
	m_iTotalFrame = cParameter.getTotalFrame();
	m_iUpdateStep = cParameter.getUpdateStep();

	m_cDepthVideoName = cParameter.getDepthVideoName();
	m_cColorVideoName = cParameter.getColorVideoName();
	m_cDepthBackgroundImageName = cParameter.getDepthBackgroundImageName();
	m_cColorBackgroundImageName = cParameter.getColorBackgroundImageName();
}

bool CBackgroundEstimation::initDepthBackground()
{
	m_pDepthBackground = new CDepthBackground();

	m_pDepthBackground->setWidth(m_iWidth);
	m_pDepthBackground->setHeight(m_iHeight);
	m_pDepthBackground->setStartFrame(m_iStartFrame);
	m_pDepthBackground->setTotalFrame(m_iTotalFrame);
	m_pDepthBackground->setUpdateStep(m_iUpdateStep);

	if (!m_pDepthBackground->allocateMem())
		return false;

	if (fopen_s(&fin_depth, m_cDepthVideoName.c_str(), "rb"))
		return false;

	return true;
}

bool CBackgroundEstimation::initColorBackground()
{
	m_pColorBackground = new CColorBackground();

	m_pColorBackground->setWidth(m_iWidth);
	m_pColorBackground->setHeight(m_iHeight);
	m_pColorBackground->setStartFrame(m_iStartFrame);
	m_pColorBackground->setTotalFrame(m_iTotalFrame);
	m_pColorBackground->setUpdateStep(m_iUpdateStep);
	m_pColorBackground->setRange(50);
	

	if (!m_pColorBackground->allocateMem())
		return false;

	if (fopen_s(&fin_depth, m_cDepthVideoName.c_str(), "rb")||
		fopen_s(&fin_color, m_cColorVideoName.c_str(), "rb"))
		return false;

	return true;
}

bool CBackgroundEstimation::buildDepthMap()
{
	std::cout << "Start to Build Depth Map :" << std::endl;
	for (int n = m_iStartFrame; n < m_iStartFrame + m_iUpdateStep; n++) {
		if (!m_pDepthBackground->getFrameBuffer()->ReadOneFrame(fin_depth, n)) {
			std::cout << "Set Frame Head Failure!" << std::endl;
			return false;
		}

		//>【Test】:中间过程的保存及显示
		//m_pDepthBackground->getFrameBuffer()->getData_inBGR(m_pDepthBackground->getBgrBuffer());
		//
		//if (!m_pDepthBackground->writeCurrentImage()) {
		//	std::cout << "Write Current Image Failure!" << std::endl;
		//	break;
		//}
		//
		//m_pDepthBackground->showCurrentImage();

		//>将当前帧亮度信息保存到深度图数据结构中
		if (!m_pDepthBackground->saveOneDepthMap(m_pDepthBackground->getFrameBuffer(), n)) {
			std::cout << "Current Frame Depth Map Save Failure!" << std::endl;
			return false;
		}
		std::cout << '.';

		//>【Test】:(x,y)=(256,144)宏块的深度值显示
		//m_pDepthBackground->showDepthMapTestMemory(144, 256, 15, n);
		//
		//>【Test】:写入一个特定的像素值到文件
		//m_pDepthBackground->writeOnePixeltoFile(304, 160, n);
				
		//system("pause");
	}//>完成深度图信息保存
	std::cout << std::endl;

	return true;
}

void CBackgroundEstimation::calcOnePixelHist(int x, int y)
{
	//>提取特定像素点的深度，以更新步长为长度，最后一个参数为起始更新帧(转换深度表格式)
	m_pDepthBackground->extractCertainPixelDepthMap(x, y, 0);

	//>进入直方图统计模块(size,scale,updatestep)
	CPixelHist cPixelHist(256, 1, 100);

	//>测试直方图模块的输入数据
	//cPixelHist.printInputData(m_pDepthBackground->getCertainPixelDepthMapBuffer());

	//>计算当前深度像素点的直方图
	cPixelHist.calcHist(m_pDepthBackground->getCertainPixelDepthMapBuffer());

	//>测试最大值最小值
	cPixelHist.findMaxandMin();

	//>显示当前深度像素点的直方图
	cPixelHist.showHist();

	//>保存当前深度像素点的直方图
	cPixelHist.writeHistImageToFile(x, y);
}

void CBackgroundEstimation::calcHist()
{
	//>声明深度背景图空间
	m_pDepthBackgroundImage = new cv::Mat(m_iHeight, m_iWidth, CV_8UC1);
	
	//>开始遍历全图像
	for (int h = 0; h < m_iHeight; h++) {
		//>定义深度图Mat类行指针
		uchar* p_bgLine = m_pDepthBackgroundImage->ptr<uchar>(h);
		for (int w = 0; w < m_iWidth; w++) {

			//>提取特定像素点的深度，以更新步长为长度，最后一个参数为起始更新帧(转换深度表格式)
			m_pDepthBackground->extractCertainPixelDepthMap(w, h, 0);

			//>进入直方图统计模块(size,scale,updatestep)
			CPixelHist cPixelHist(256, 1, m_iUpdateStep);

			//>计算当前深度像素点的直方图
			cPixelHist.calcHist(m_pDepthBackground->getCertainPixelDepthMapBuffer());

			//>判断是否只有一个峰，如果有且只有一个峰，则此像素点的背景深度值即为该峰对应的深度值
			if (cPixelHist.isOnlyOneBar()) {
				p_bgLine[w] = cPixelHist.getDepthValueTag();
				continue;
			}

			//>测试最大值最小值
			cPixelHist.findMaxandMin();

			//如果最高峰频次（高度）小于N/5，则此像素点的背景深度值赋值为0【参数3、4间调整】
			if (cPixelHist.getMaxBarValue() < m_iUpdateStep / 3) {
				p_bgLine[w] = 0;
				continue;
			}

			//>从直方图最左边开始向右边遍历，记录第一个峰值大于或等于最高峰频次1/8的峰，作为深度值最低的主峰
			p_bgLine[w] = cPixelHist.getBackgroundMainPeek();
		}
	}//>全图像遍历完成

	//>测试提取的深度背景图
	//写入配置;
	std::vector<int>compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);
	cv::imwrite(m_cDepthBackgroundImageName, *m_pDepthBackgroundImage);
	//cv::imshow("背景深度图;", *m_pDepthBackgroundImage);
	//cv::waitKey(0);
}

bool CBackgroundEstimation::calcColorBackground()
{
	std::cout << "Start to Calculate Color Background :" << std::endl;

	//>读入深度背景图
	m_pColorBackground->readDepthBackgroundImage(m_cDepthBackgroundImageName);

	for (int n = m_iStartFrame; n < m_iStartFrame + m_iUpdateStep; n++) {

		if (!m_pColorBackground->getDepthVideoBuffer()->ReadOneFrame(fin_depth, n)) {
			std::cout << "Set Frame Head Failure!" << std::endl;
			return false;
		}
		if (!m_pColorBackground->getColorVideoBuffer()->ReadOneFrame(fin_color, n)) {
			std::cout << "Set Frame Head Failure!" << std::endl;
			return false;
		}

		//>转换当前帧的图像格式
		m_pColorBackground->getDepthVideoBuffer()->getData_inBGR(m_pColorBackground->getDepthViewBGRBuffer());
		m_pColorBackground->getColorVideoBuffer()->getData_inBGR(m_pColorBackground->getColorViewBGRBuffer());

		//>【Test】输出中间结果
		//m_pColorBackground->showCurrentImage();

		//>进行一帧的操作
		m_pColorBackground->buildOneFrameColorBackground();

		std::cout << '.';
	}
	
	std::cout << std::endl;

	//>求取平均计算彩色背景图
	m_pColorBackground->calcFinalColorBackground();

	//>显示彩色背景图结果
	m_pColorBackground->showColorBackgroundImage();

	//>【Test】测试每一行的countNumber不为0的个数
	//m_pColorBackground->testShowCountLineSum();

	return true;
}
