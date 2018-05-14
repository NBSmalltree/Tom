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
	if (fout_depthBackground)
		fclose(fout_depthBackground);
	if (fin_depthBackground)
		fclose(fin_depthBackground);
	if (fout_colorBackground)
		fclose(fout_colorBackground);

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
	m_iSplitRange = cParameter.getSplitRange();

	m_cDepthVideoName = cParameter.getDepthVideoName();
	m_cColorVideoName = cParameter.getColorVideoName();
	m_cDepthBackgroundImageName = cParameter.getDepthBackgroundImageName();
	m_cColorBackgroundImageName = cParameter.getColorBackgroundImageName();

	m_flagIsInpaint = cParameter.getIsInpaint();
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

	if (fopen_s(&fin_depth, m_cDepthVideoName.c_str(), "rb")||
		fopen_s(&fout_depthBackground, m_cDepthBackgroundImageName.c_str(), "wb"))
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
	m_pColorBackground->setRange(m_iSplitRange);
	

	if (!m_pColorBackground->allocateMem())
		return false;

	if (fopen_s(&fin_depth, m_cDepthVideoName.c_str(), "rb")||
		fopen_s(&fin_depthBackground, m_cDepthBackgroundImageName.c_str(), "rb")||
		fopen_s(&fin_color, m_cColorVideoName.c_str(), "rb")||
		fopen_s(&fout_colorBackground, m_cColorBackgroundImageName.c_str(), "wb"))
		return false;

	return true;
}

void CBackgroundEstimation::releaseDepthBackground()
{
	m_pDepthBackground->releaseMem();

	if (fin_depth)
		fclose(fin_depth);
	if (fout_depthBackground)
		fclose(fout_depthBackground);

	if (m_pDepthBackground != nullptr) {
		delete m_pDepthBackground;
		m_pDepthBackground = nullptr;
	}
}

void CBackgroundEstimation::releaseColorBackground()
{
	m_pColorBackground->releaseMem();

	if (fin_depth)
		fclose(fin_depth);
	if (fin_depthBackground)
		fclose(fin_depthBackground);
	if (fin_color)
		fclose(fin_color);
	if (fout_colorBackground)
		fclose(fout_colorBackground);

	if (m_pColorBackground != nullptr) {
		delete m_pColorBackground;
		m_pColorBackground = nullptr;
	}
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
	CIYuv yuvBuffer(m_iHeight, m_iWidth, 420);
	
	//>��ʼ����ȫͼ��
	for (int h = 0; h < m_iHeight; h++) {
		for (int w = 0; w < m_iWidth; w++) {

			//>��ȡ�ض����ص����ȣ��Ը��²���Ϊ���ȣ����һ������Ϊ��ʼ����֡(ת����ȱ��ʽ)
			m_pDepthBackground->extractCertainPixelDepthMap(w, h, 0);

			//>����ֱ��ͼͳ��ģ��(size,scale,updatestep)
			CPixelHist cPixelHist(256, 1, m_iUpdateStep);

			//>���㵱ǰ������ص��ֱ��ͼ
			cPixelHist.calcHist(m_pDepthBackground->getCertainPixelDepthMapBuffer());

			//>�ж��Ƿ�ֻ��һ���壬�������ֻ��һ���壬������ص�ı������ֵ��Ϊ�÷��Ӧ�����ֵ
			if (cPixelHist.isOnlyOneBar()) {
				yuvBuffer.Y[h][w] = cPixelHist.getDepthValueTag();
				continue;
			}

			//>�������ֵ��Сֵ
			cPixelHist.findMaxandMin();

			//>�����߷�Ƶ�Σ��߶ȣ�С��N/5��������ص�ı������ֵ��ֵΪ0������3��4�������,��������ǰ������
			if (cPixelHist.getMaxBarValue() < m_iUpdateStep / 3) {
				yuvBuffer.Y[h][w] = 0;
				continue;
			}

			//>��ֱ��ͼ����߿�ʼ���ұ߱�������¼��һ����ֵ���ڻ������߷�Ƶ��1/8�ķ壬��Ϊ���ֵ��͵�����
			yuvBuffer.Y[h][w] = cPixelHist.getBackgroundMainPeek();
		}
	}//>ȫͼ��������

	//>���ͼ�ն��޸�
	if(m_flagIsInpaint)
		inpaint(m_iHeight, m_iWidth, &yuvBuffer);

	//>д�����ɵ���ȱ���ͼ
	yuvBuffer.WriteOneFrame(fout_depthBackground);

	//>Release CDepthBackground Class
	releaseDepthBackground();
}

void CBackgroundEstimation::inpaint(int height, int width, CIYuv *yuvBuffer)
{
	cv::Mat backgroundImage(height, width, CV_8UC1, yuvBuffer->Y[0]);

	cv::Mat inpaintMask;
	inpaintMask = cv::Mat::zeros(backgroundImage.size(), CV_8UC1);

	for (int h = 0; h < height; h++) {
		uchar* backgroundLine = backgroundImage.ptr<uchar>(h);
		uchar* inpaintMaskLine = inpaintMask.ptr<uchar>(h);
		for (int w = 0; w < width; w++) {
			if (backgroundLine[w] == 0)
				inpaintMaskLine[w] = 255;
		}
	}

	//>��Ĥ���ų��ṹ�Կն�
	int top_index, bottom_index, left_index, right_index, sum_index;
	for (int h = 0; h < height; h++) {
		uchar* inpaintMaskLine = inpaintMask.ptr<uchar>(h);
		for (int w = 0; w < width; w++) {
			//>�������Ĥ���ǿն����أ��ж������������Ƿ�Ϊ�ն������ն��������ڵ���2���ų���Ĥ(����inpaint)
			if (inpaintMaskLine[w] == 255) {
				top_index = CLIP3(h - 1, 0, height);
				bottom_index = CLIP3(h + 1, 0, height);
				left_index = CLIP3(w - 1, 0, width);
				right_index = CLIP3(w + 1, 0, width);

				sum_index = 0;
				if (yuvBuffer->Y[top_index][w] == 0)
					sum_index++;
				if (yuvBuffer->Y[bottom_index][w] == 0)
					sum_index++;
				if (yuvBuffer->Y[h][left_index] == 0)
					sum_index++;
				if (yuvBuffer->Y[h][right_index] == 0)
					sum_index++;

				if (sum_index >= 2)
					inpaintMaskLine[w] = 0;
			}
		}
	}

	cv::Mat inpaintedImage;
	cv::inpaint(backgroundImage, inpaintMask, inpaintedImage, 3, cv::INPAINT_TELEA); //>�˴���5������������ INPAINT_NS ���� INPAINT_TELEA

	for (int h = 0; h < height; h++) {
		uchar* matLine = inpaintedImage.ptr<uchar>(h);
		for (int w = 0; w < width; w++) {
			yuvBuffer->Y[h][w] = matLine[w];
		}
	}

}

bool CBackgroundEstimation::calcColorBackground()
{
	std::cout << "Start to Calculate Color Background :" << std::endl;

	//>������ȱ���ͼ
	m_pColorBackground->readDepthBackgroundImage(fin_depthBackground);

	for (int n = m_iStartFrame; n < m_iStartFrame + m_iUpdateStep; n++) {

		if (!m_pColorBackground->getDepthVideoBuffer()->ReadOneFrame(fin_depth, n)) {
			std::cout << "Set Frame Head Failure!" << std::endl;
			return false;
		}
		if (!m_pColorBackground->getColorVideoBuffer()->ReadOneFrame(fin_color, n)) {
			std::cout << "Set Frame Head Failure!" << std::endl;
			return false;
		}

		//>����һ֡�Ĳ���
		m_pColorBackground->buildOneFrameColorBackground();

		std::cout << '.';
	}
	
	std::cout << std::endl;

	//>��ȡƽ�������ɫ����ͼ
	m_pColorBackground->calcFinalColorBackground();

	//>д�����ɵĲ�ɫ����ͼ
	m_pColorBackground->writeColorBackgroundImage(fout_colorBackground);

	//>Release CColorBackground Class
	releaseColorBackground();

	return true;
}
