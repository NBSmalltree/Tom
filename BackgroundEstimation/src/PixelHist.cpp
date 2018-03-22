#include "PixelHist.h"


CPixelHist::CPixelHist()
{
	m_pcHistImage = NULL;
}

CPixelHist::CPixelHist(int size, int scale, int frame)
{
	m_iSize = size;
	m_iScale = scale;
	m_iTotalFrame = frame;

	m_iMaxValue = 0;
	m_iMinValue = 255;

	for (int i = 0; i < 255; i++)
		m_iBarValue[i] = 0;

	m_idepthValue = 0;

	m_pcHistImage = new cv::Mat(m_iSize * m_iScale, m_iSize, CV_8U, cv::Scalar(0));
}

CPixelHist::~CPixelHist()
{
	m_pcHistImage->release();
	if (m_pcHistImage != NULL) delete m_pcHistImage;
}

void CPixelHist::printInputData(unsigned char * pBuffer)
{
	std::cout << std::endl << "This is Hist Input Data Get Test : " << std::endl;

	for (int i = 0; i < m_iTotalFrame; i++)
		std::cout << (int)pBuffer[i] << ' ';

	std::cout << std::endl;
}

void CPixelHist::findMaxandMin()
{
	m_iMaxValue = 0;
	m_iMinValue = 255;

	for (int i = 0; i < 255; i++) {
		if (m_iBarValue[i] > m_iMaxValue)
			m_iMaxValue = m_iBarValue[i];
		if (m_iBarValue[i] < m_iMinValue && m_iBarValue[i] != 0)
			m_iMinValue = m_iBarValue[i];
	}

	//std::cout << std::endl;
	//std::cout << "MaxValue: " << m_iMaxValue << std::endl;
	//std::cout << "MinValue: " << m_iMinValue << std::endl;

}

void CPixelHist::calcHist(unsigned char * pBuffer)
{
	int i;

	//>Initialize
	for (i = 0; i < 255; i++)
		m_iBarValue[i] = 0;

	for (i = 0; i < m_iTotalFrame; i++)
		m_iBarValue[(int)pBuffer[i]]++;

	//>Test to output the BarValue
	//showFrequency();
}

void CPixelHist::showHist()
{
	int hpt = cv::saturate_cast<int>(0.9 * m_iSize);
	for (int i = 0; i < 256; i++) {
		int realValue = cv::saturate_cast<int>(m_iBarValue[i] * hpt / m_iMaxValue);
		cv::rectangle(*m_pcHistImage, cv::Point(i * m_iScale, m_iSize - 1), cv::Point((i + 1)*m_iScale - 1, m_iSize - realValue), cv::Scalar(255));
	}

	cv::imshow("一维直方图;", *m_pcHistImage);
	cv::waitKey(0);
}

void CPixelHist::showFrequency()
{
	std::cout << std::endl << "This is the Frequency of the Hist!" << std::endl;
	for (int i = 0; i < 255; i++) {
		if (m_iBarValue[i] != 0)
			std::cout << i << ": " << m_iBarValue[i] << std::endl;
	}
}

void CPixelHist::writeHistImageToFile(int x, int y)
{
	//>写入配置
	cv::vector<int>compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	std::string outFileName = "HistImage_(";
	outFileName += std::to_string(x);
	outFileName += ',';
	outFileName += std::to_string(y);
	outFileName += ").png";

	cv::imwrite(outFileName, *m_pcHistImage);
}

bool CPixelHist::isOnlyOneBar()
{
	int sum = 0;
	m_idepthValue = 0;
	for (int i = 0; i < 255; i++) {
		if (m_iBarValue[i] != 0) {
			sum++;
			m_idepthValue = i;
		}
	}
	if (sum == 1)
		return true;
	return false;
}

int CPixelHist::getBackgroundMainPeek()
{
	m_idepthValue = 0;
	for (int i = 0; i < 255; i++) {
		if (m_iBarValue[i] >= m_iMaxValue / 8) {
			m_idepthValue = i;
			break;
		}
	}
	return m_idepthValue;
}
