#include "UsingDeeperDepth.h"

CUsingDeeperDepth::CUsingDeeperDepth()
{
	m_iWidth = 1024;
	m_iHeight = 768;
	m_iStartFrame = 0;
	m_iTotalFrame = 1;

	m_pcLeftViewColor = nullptr;
	m_pcRightViewColor = nullptr;
	m_pcLeftViewDepth = nullptr;
	m_pcRightViewDepth = nullptr;
	m_pcOutColor = nullptr;
	m_pcOutDepth = nullptr;
}

CUsingDeeperDepth::~CUsingDeeperDepth()
{
	if (m_pcLeftViewColor != nullptr) {
		delete m_pcLeftViewColor;
		m_pcLeftViewColor = nullptr;
	}
	if (m_pcRightViewColor != nullptr) {
		delete m_pcRightViewColor;
		m_pcRightViewColor = nullptr;
	}
	if (m_pcLeftViewDepth != nullptr) {
		delete m_pcLeftViewDepth;
		m_pcLeftViewDepth = nullptr;
	}
	if (m_pcRightViewDepth != nullptr) {
		delete m_pcRightViewDepth;
		m_pcRightViewDepth = nullptr;
	}
	if (m_pcOutColor != nullptr) {
		delete m_pcOutColor;
		m_pcOutColor = nullptr;
	}
	if (m_pcOutDepth != nullptr) {
		delete m_pcOutDepth;
		m_pcOutDepth = nullptr;
	}
}

bool CUsingDeeperDepth::allocateMem()
{
	m_pcLeftViewColor = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcRightViewColor = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcLeftViewDepth = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcRightViewDepth = new CIYuv(m_iHeight, m_iWidth, 420);

	m_pcOutColor = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcOutDepth = new CIYuv(m_iHeight, m_iWidth, 420);

	return true;
}

void CUsingDeeperDepth::releaseMem()
{
	if (m_pcLeftViewColor != nullptr) {
		delete m_pcLeftViewColor;
		m_pcLeftViewColor = nullptr;
	}
	if (m_pcRightViewColor != nullptr) {
		delete m_pcRightViewColor;
		m_pcRightViewColor = nullptr;
	}
	if (m_pcLeftViewDepth != nullptr) {
		delete m_pcLeftViewDepth;
		m_pcLeftViewDepth = nullptr;
	}
	if (m_pcRightViewDepth != nullptr) {
		delete m_pcRightViewDepth;
		m_pcRightViewDepth = nullptr;
	}
	if (m_pcOutColor != nullptr) {
		delete m_pcOutColor;
		m_pcOutColor = nullptr;
	}
	if (m_pcOutDepth != nullptr) {
		delete m_pcOutDepth;
		m_pcOutDepth = nullptr;
	}
}

/*
1��������ӵ����ֵ��Ϊ��������ӵ��ֵ��
2��������ӵ����ֵ��Ϊ��������ӵ��ֵ��
3��������ӵ����ֵ����Ϊ��������ֵ��С���ӵ���Ϣ����
4�������䡿������ӵ����ֵ��Ϊ0(���ǲ�ɫֵ���ܲ�Ϊ0���������),>������ָ����������
*/
void CUsingDeeperDepth::doProcess()
{
	for (int h = 0; h < m_iHeight; h++) {
		for (int w = 0; w < m_iWidth; w++) {
			if (m_pcLeftViewDepth->Y[h][w] == 0 && m_pcRightViewDepth->Y[h][w] != 0) {
				m_pcOutColor->Y[h][w] = m_pcRightViewColor->Y[h][w];
				m_pcOutColor->U[h / 2][w / 2] = m_pcRightViewColor->U[h / 2][w / 2];
				m_pcOutColor->V[h / 2][w / 2] = m_pcRightViewColor->V[h / 2][w / 2];

				m_pcOutDepth->Y[h][w] = m_pcRightViewDepth->Y[h][w];
			}
			else if (m_pcLeftViewDepth->Y[h][w] != 0 && m_pcRightViewDepth->Y[h][w] == 0) {
				m_pcOutColor->Y[h][w] = m_pcLeftViewColor->Y[h][w];
				m_pcOutColor->U[h / 2][w / 2] = m_pcLeftViewColor->U[h / 2][w / 2];
				m_pcOutColor->V[h / 2][w / 2] = m_pcLeftViewColor->V[h / 2][w / 2];

				m_pcOutDepth->Y[h][w] = m_pcLeftViewDepth->Y[h][w];
			}
			else if (m_pcLeftViewDepth->Y[h][w] != 0 && m_pcRightViewDepth->Y[h][w] != 0) {
				if (m_pcLeftViewDepth->Y[h][w] <= m_pcRightViewDepth->Y[h][w]) {
					m_pcOutColor->Y[h][w] = m_pcLeftViewColor->Y[h][w];
					m_pcOutColor->U[h / 2][w / 2] = m_pcLeftViewColor->U[h / 2][w / 2];
					m_pcOutColor->V[h / 2][w / 2] = m_pcLeftViewColor->V[h / 2][w / 2];

					m_pcOutDepth->Y[h][w] = m_pcLeftViewDepth->Y[h][w];
				}
				else {
					m_pcOutColor->Y[h][w] = m_pcRightViewColor->Y[h][w];
					m_pcOutColor->U[h / 2][w / 2] = m_pcRightViewColor->U[h / 2][w / 2];
					m_pcOutColor->V[h / 2][w / 2] = m_pcRightViewColor->V[h / 2][w / 2];

					m_pcOutDepth->Y[h][w] = m_pcRightViewDepth->Y[h][w];
				}
			}
			//>�����ص������ӵ����ֵΪ0������ɫͼֵ��Ϊ0����������������(����ֵ������ֵ����������ֵ)
			//>(��������ǰһ�׶α��������㷨����Щ���ص�ԭʼ���ͼ����0����)
			//>�������ͼ��ֵΪ0����ɫͼ��ֵ��һ��Ϊ0
			else if (m_pcLeftViewDepth->Y[h][w] == 0 && m_pcRightViewDepth->Y[h][w] == 0) {
				if (m_pcLeftViewColor->Y[h][w] != 0 && m_pcRightViewColor->Y[h][w] != 0) {
					m_pcOutColor->Y[h][w] = (char)((m_pcLeftViewColor->Y[h][w] + m_pcRightViewColor->Y[h][w]) / 2);
					m_pcOutColor->U[h / 2][w / 2] = (char)((m_pcLeftViewColor->U[h / 2][w / 2] + m_pcRightViewColor->U[h / 2][w / 2]) / 2);
					m_pcOutColor->V[h / 2][w / 2] = (char)((m_pcLeftViewColor->V[h / 2][w / 2] + m_pcRightViewColor->V[h / 2][w / 2]) / 2);				
				}
				else if (m_pcLeftViewColor->Y[h][w] == 0 && m_pcRightViewColor->Y[h][w] == 0) {
					m_pcOutDepth->Y[h][w] = 0;
					continue;
				}
				else if (m_pcLeftViewColor->Y[h][w] != 0) {
					m_pcOutColor->Y[h][w] = m_pcLeftViewColor->Y[h][w];
					m_pcOutColor->U[h / 2][w / 2] = m_pcLeftViewColor->U[h / 2][w / 2];
					m_pcOutColor->V[h / 2][w / 2] = m_pcLeftViewColor->V[h / 2][w / 2];
				}
				else if (m_pcRightViewColor->Y[h][w] != 0) {
					m_pcOutColor->Y[h][w] = m_pcRightViewColor->Y[h][w];
					m_pcOutColor->U[h / 2][w / 2] = m_pcRightViewColor->U[h / 2][w / 2];
					m_pcOutColor->V[h / 2][w / 2] = m_pcRightViewColor->V[h / 2][w / 2];
				}

				m_pcOutDepth->Y[h][w] = 0;
			}
		}
	}
}
