#include "ColorBackground.h"


CColorBackground::CColorBackground()
{
	m_pcDepthVideo = NULL;
	m_pcColorVideo = NULL;
	m_pcDepthBackgroundImage = NULL;
	m_pcColorBackgroundImage = NULL;
	m_pcDepthViewImage = NULL;
	m_pcColorViewImage = NULL;
	m_pcTemporalColorImage = NULL;
}

CColorBackground::~CColorBackground()
{
	if (m_pcDepthVideo != nullptr) delete m_pcDepthVideo;
	if (m_pcColorVideo != nullptr) delete m_pcColorVideo;

	if (m_pcDepthBackgroundImage != nullptr) {
		m_pcDepthBackgroundImage->release();
		delete m_pcDepthBackgroundImage;
	}
	if (m_pcColorBackgroundImage != nullptr) {
		m_pcColorBackgroundImage->release();
		delete m_pcColorBackgroundImage;
	}
	if (m_pcDepthViewImage != nullptr) {
		m_pcDepthViewImage->release();
		delete m_pcDepthViewImage;
	}
	if (m_pcColorViewImage != nullptr) {
		m_pcColorViewImage->release();
		delete m_pcColorViewImage;
	}
	if (m_pcTemporalColorImage != nullptr) {
		m_pcTemporalColorImage->release();
		delete m_pcTemporalColorImage;
	}
	if (countNumber!=nullptr) {
		delete countNumber;
		countNumber = NULL;
	}
}

bool CColorBackground::allocateMem()
{
	m_pcDepthVideo = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcColorVideo = new CIYuv(m_iHeight, m_iWidth, 420);

	m_pcDepthBackgroundImage = new cv::Mat(m_iHeight, m_iWidth, CV_8UC1, cv::Scalar(0));
	m_pcColorBackgroundImage = new cv::Mat(m_iHeight, m_iWidth, CV_8UC3, cv::Scalar(0, 0, 0));
	m_pcDepthViewImage = new cv::Mat(m_iHeight, m_iWidth, CV_8UC3, cv::Scalar(0, 0, 0));
	m_pcColorViewImage = new cv::Mat(m_iHeight, m_iWidth, CV_8UC3, cv::Scalar(0, 0, 0));
	m_pcTemporalColorImage = new cv::Mat(m_iHeight, m_iWidth, CV_16UC3, cv::Scalar(0, 0, 0));

	countNumber = new int[m_iHeight * m_iWidth]{0};
	testsum1 = 0;
	testsum2 = 0;

	return true;
}

void CColorBackground::buildOneFrameColorBackground()
{
	int rangeMin;
	int rangeMax;
	int tempY;
	
	//>��ʼ����ȫͼ��
	for (int h = 0; h < m_iHeight; h++) {
		//>�������ͼMat����ָ��
		uchar* p_DepthBackgroundImageLine = m_pcDepthBackgroundImage->ptr<uchar>(h);
		uchar* p_DepthViewImageLine = m_pcDepthViewImage->ptr<uchar>(h);
		uchar* p_ColorViewImageLine = m_pcColorViewImage->ptr<uchar>(h);
		ushort* p_TemporalColorLine = m_pcTemporalColorImage->ptr<ushort>(h);
		for (int w = 0; w < m_iWidth; w++) {

			//>�ų���ȱ���ͼ�еĿն�
			if(p_DepthViewImageLine[3 * w + 2] == 0 && p_DepthViewImageLine[3 * w + 1] == 0 && p_DepthViewImageLine[3 * w] == 0)
				continue;

			rangeMin = CLIP3(p_DepthBackgroundImageLine[w] - m_iRange, 0, 255);
			rangeMax = CLIP3(p_DepthBackgroundImageLine[w] + m_iRange, 0, 255);

			tempY = (int)(0.299*p_DepthViewImageLine[3 * w + 2] + 0.587*p_DepthViewImageLine[3 * w + 1] + 0.114*p_DepthViewImageLine[3 * w] + 0.5);

			if (tempY >= rangeMin && tempY <= rangeMax) {
				p_TemporalColorLine[3 * w] += p_ColorViewImageLine[3 * w];
				p_TemporalColorLine[3 * w + 1] += p_ColorViewImageLine[3 * w + 1];
				p_TemporalColorLine[3 * w + 2] += p_ColorViewImageLine[3 * w + 2];

				countNumber[h*m_iWidth + w]++;
			}
			
		}
	}//>ȫͼ��������
}

void CColorBackground::calcFinalColorBackground()
{

	//>��ʼ����ȫͼ��
	for (int h = 0; h < m_iHeight; h++) {
		//>�������ͼMat����ָ��
		uchar* p_ColorBackgroundImageLine = m_pcColorBackgroundImage->ptr<uchar>(h);
		ushort* p_TemporalColorLine = m_pcTemporalColorImage->ptr<ushort>(h);
		for (int w = 0; w < m_iWidth; w++) {

			if (countNumber[h*m_iWidth + w] != 0) {
				p_ColorBackgroundImageLine[3 * w] = (uchar)(p_TemporalColorLine[3 * w] / (double)countNumber[h*m_iWidth + w]);
				p_ColorBackgroundImageLine[3 * w + 1] = (uchar)(p_TemporalColorLine[3 * w + 1] / (double)countNumber[h*m_iWidth + w]);
				p_ColorBackgroundImageLine[3 * w + 2] = (uchar)(p_TemporalColorLine[3 * w + 2] / (double)countNumber[h*m_iWidth + w]);
			}

		}
	}//>ȫͼ��������
}

void CColorBackground::readDepthBackgroundImage(const std::string& strName)
{
	cv::Mat &tMat = cv::imread(strName);
	tMat.copyTo(*m_pcDepthBackgroundImage);
}

void CColorBackground::showCurrentImage()
{
	cv::imshow(">��ǰ��ȱ���ͼ", *m_pcDepthBackgroundImage);
	cv::imshow(">��ǰ���ͼ", *m_pcDepthViewImage);
	cv::imshow(">��ǰ��ɫͼ", *m_pcColorViewImage);
	cv::waitKey(0);
}

void CColorBackground::showColorBackgroundImage()
{
	cv::imshow(">��ɫ����ͼ", *m_pcColorBackgroundImage);
	cv::waitKey(0);
}

void CColorBackground::writeColorBackgroundImage(const std::string& strName)
{
	//д������;
	std::vector<int>compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	cv::imwrite(strName, *m_pcColorBackgroundImage);
}

void CColorBackground::testShowCountLineSum()
{
	for (int h = 0; h < m_iHeight; h++) {
		int sum = 0;
		for (int w = 0; w < m_iWidth; w++) {
			if (countNumber[h*m_iWidth + w] != 0)
				sum++;
		}
		std::cout << sum << std::endl;
	}
}
