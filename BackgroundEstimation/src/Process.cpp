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

		//>��Test��:�м���̵ı��漰��ʾ
		//m_pDepthBackground->getFrameBuffer()->getData_inBGR(m_pDepthBackground->getBgrBuffer());
		//
		//if (!m_pDepthBackground->writeCurrentImage()) {
		//	std::cout << "Write Current Image Failure!" << std::endl;
		//	break;
		//}
		//
		//m_pDepthBackground->showCurrentImage();

		//>����ǰ֡������Ϣ���浽���ͼ���ݽṹ��
		if (!m_pDepthBackground->saveOneDepthMap(m_pDepthBackground->getFrameBuffer(), n)) {
			std::cout << "Current Frame Depth Map Save Failure!" << std::endl;
			return false;
		}
		std::cout << '.';

		//>��Test��:(x,y)=(256,144)�������ֵ��ʾ
		//m_pDepthBackground->showDepthMapTestMemory(144, 256, 15, n);
		//
		//>��Test��:д��һ���ض�������ֵ���ļ�
		//m_pDepthBackground->writeOnePixeltoFile(304, 160, n);
				
		//system("pause");
	}//>������ͼ��Ϣ����
	std::cout << std::endl;

	return true;
}

void CBackgroundEstimation::calcOnePixelHist(int x, int y)
{
	//>��ȡ�ض����ص����ȣ��Ը��²���Ϊ���ȣ����һ������Ϊ��ʼ����֡(ת����ȱ��ʽ)
	m_pDepthBackground->extractCertainPixelDepthMap(x, y, 0);

	//>����ֱ��ͼͳ��ģ��(size,scale,updatestep)
	CPixelHist cPixelHist(256, 1, 100);

	//>����ֱ��ͼģ�����������
	//cPixelHist.printInputData(m_pDepthBackground->getCertainPixelDepthMapBuffer());

	//>���㵱ǰ������ص��ֱ��ͼ
	cPixelHist.calcHist(m_pDepthBackground->getCertainPixelDepthMapBuffer());

	//>�������ֵ��Сֵ
	cPixelHist.findMaxandMin();

	//>��ʾ��ǰ������ص��ֱ��ͼ
	cPixelHist.showHist();

	//>���浱ǰ������ص��ֱ��ͼ
	cPixelHist.writeHistImageToFile(x, y);
}

void CBackgroundEstimation::calcHist()
{
	//>������ȱ���ͼ�ռ�
	m_pDepthBackgroundImage = new cv::Mat(m_iHeight, m_iWidth, CV_8UC1);
	
	//>��ʼ����ȫͼ��
	for (int h = 0; h < m_iHeight; h++) {
		//>�������ͼMat����ָ��
		uchar* p_bgLine = m_pDepthBackgroundImage->ptr<uchar>(h);
		for (int w = 0; w < m_iWidth; w++) {

			//>��ȡ�ض����ص����ȣ��Ը��²���Ϊ���ȣ����һ������Ϊ��ʼ����֡(ת����ȱ��ʽ)
			m_pDepthBackground->extractCertainPixelDepthMap(w, h, 0);

			//>����ֱ��ͼͳ��ģ��(size,scale,updatestep)
			CPixelHist cPixelHist(256, 1, m_iUpdateStep);

			//>���㵱ǰ������ص��ֱ��ͼ
			cPixelHist.calcHist(m_pDepthBackground->getCertainPixelDepthMapBuffer());

			//>�ж��Ƿ�ֻ��һ���壬�������ֻ��һ���壬������ص�ı������ֵ��Ϊ�÷��Ӧ�����ֵ
			if (cPixelHist.isOnlyOneBar()) {
				p_bgLine[w] = cPixelHist.getDepthValueTag();
				continue;
			}

			//>�������ֵ��Сֵ
			cPixelHist.findMaxandMin();

			//�����߷�Ƶ�Σ��߶ȣ�С��N/5��������ص�ı������ֵ��ֵΪ0������3��4�������
			if (cPixelHist.getMaxBarValue() < m_iUpdateStep / 3) {
				p_bgLine[w] = 0;
				continue;
			}

			//>��ֱ��ͼ����߿�ʼ���ұ߱�������¼��һ����ֵ���ڻ������߷�Ƶ��1/8�ķ壬��Ϊ���ֵ��͵�����
			p_bgLine[w] = cPixelHist.getBackgroundMainPeek();
		}
	}//>ȫͼ��������

	//>������ȡ����ȱ���ͼ
	//д������;
	std::vector<int>compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);
	cv::imwrite(m_cDepthBackgroundImageName, *m_pDepthBackgroundImage);
	//cv::imshow("�������ͼ;", *m_pDepthBackgroundImage);
	//cv::waitKey(0);
}

bool CBackgroundEstimation::calcColorBackground()
{
	std::cout << "Start to Calculate Color Background :" << std::endl;

	//>������ȱ���ͼ
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

		//>ת����ǰ֡��ͼ���ʽ
		m_pColorBackground->getDepthVideoBuffer()->getData_inBGR(m_pColorBackground->getDepthViewBGRBuffer());
		m_pColorBackground->getColorVideoBuffer()->getData_inBGR(m_pColorBackground->getColorViewBGRBuffer());

		//>��Test������м���
		//m_pColorBackground->showCurrentImage();

		//>����һ֡�Ĳ���
		m_pColorBackground->buildOneFrameColorBackground();

		std::cout << '.';
	}
	
	std::cout << std::endl;

	//>��ȡƽ�������ɫ����ͼ
	m_pColorBackground->calcFinalColorBackground();

	//>��ʾ��ɫ����ͼ���
	m_pColorBackground->showColorBackgroundImage();

	//>��Test������ÿһ�е�countNumber��Ϊ0�ĸ���
	//m_pColorBackground->testShowCountLineSum();

	return true;
}
