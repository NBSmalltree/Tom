#include "DoubleViewFilling.h"



CDoubleViewFilling::CDoubleViewFilling()
{
	m_iWidth = 1024;
	m_iHeight = 768;
	m_iStartFrame = 0;
	m_iTotalFrame = 10;

	m_pcLeftColor = nullptr;
	m_pcRghtColor = nullptr;
	m_pcLeftDepth = nullptr;
	m_pcRghtDepth = nullptr;

	m_pcBGColor = nullptr;
	m_pcBGDepth = nullptr;

	m_pcOutColor = nullptr;
	m_pcOutDepth = nullptr;

	temp1 = nullptr;
	temp2 = nullptr;
}


CDoubleViewFilling::~CDoubleViewFilling()
{
	if (m_pcLeftColor != nullptr) {
		delete m_pcLeftColor;
		m_pcLeftColor = nullptr;
	}
	if (m_pcRghtColor != nullptr) {
		delete m_pcRghtColor;
		m_pcRghtColor = nullptr;
	}
	if (m_pcLeftDepth != nullptr) {
		delete m_pcLeftDepth;
		m_pcLeftDepth = nullptr;
	}
	if (m_pcRghtDepth != nullptr) {
		delete m_pcRghtDepth;
		m_pcRghtDepth = nullptr;
	}

	if (m_pcBGColor != nullptr) {
		delete m_pcBGColor;
		m_pcBGColor = nullptr;
	}
	if (m_pcBGDepth != nullptr) {
		delete m_pcBGDepth;
		m_pcBGDepth = nullptr;
	}

	if (m_pcOutColor != nullptr) {
		delete m_pcOutColor;
		m_pcOutColor = nullptr;
	}
	if (m_pcOutDepth != nullptr) {
		delete m_pcOutDepth;
		m_pcOutDepth = nullptr;
	}

	if (temp1 != nullptr) {
		delete temp1;
		temp1 = nullptr;
	}
	if (temp2 != nullptr) {
		delete temp2;
		temp2 = nullptr;
	}
}

bool CDoubleViewFilling::allocateMem()
{
	m_pcLeftColor = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcRghtColor = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcLeftDepth = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcRghtDepth = new CIYuv(m_iHeight, m_iWidth, 420);

	m_pcBGColor = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcBGDepth = new CIYuv(m_iHeight, m_iWidth, 420);

	m_pcOutColor = new CIYuv(m_iHeight, m_iWidth, 420);
	m_pcOutDepth = new CIYuv(m_iHeight, m_iWidth, 420);

	temp1 = new CIYuv(m_iHeight, m_iWidth, 420);
	temp2 = new CIYuv(m_iHeight, m_iWidth, 420);

	return true;
}

void CDoubleViewFilling::releaseMem()
{
	if (m_pcLeftColor != nullptr) {
		delete m_pcLeftColor;
		m_pcLeftColor = nullptr;
	}
	if (m_pcRghtColor != nullptr) {
		delete m_pcRghtColor;
		m_pcRghtColor = nullptr;
	}
	if (m_pcLeftDepth != nullptr) {
		delete m_pcLeftDepth;
		m_pcLeftDepth = nullptr;
	}
	if (m_pcRghtDepth != nullptr) {
		delete m_pcRghtDepth;
		m_pcRghtDepth = nullptr;
	}

	if (m_pcBGColor != nullptr) {
		delete m_pcBGColor;
		m_pcBGColor = nullptr;
	}
	if (m_pcBGDepth != nullptr) {
		delete m_pcBGDepth;
		m_pcBGDepth = nullptr;
	}

	if (m_pcOutColor != nullptr) {
		delete m_pcOutColor;
		m_pcOutColor = nullptr;
	}
	if (m_pcOutDepth != nullptr) {
		delete m_pcOutDepth;
		m_pcOutDepth = nullptr;
	}

	if (temp1 != nullptr) {
		delete temp1;
		temp1 = nullptr;
	}
	if (temp2 != nullptr) {
		delete temp2;
		temp2 = nullptr;
	}
}

/*
1���ж��Ƿ�Ϊ��ɫͼ�еĿն�
*/
bool CDoubleViewFilling::isHoleinColor(CIYuv *yuvBuffer, int h, int w)
{
	//>�ж�����������Yͨ����UVͨ��
	//if (yuvBuffer->Y[h][w] == 0 &&
	//	(yuvBuffer->U[h / 2][w / 2] >= 120 && yuvBuffer->U[h / 2][w / 2] <= 135) &&
	//	(yuvBuffer->V[h / 2][w / 2] >= 120 && yuvBuffer->V[h / 2][w / 2] <= 135))
	//>�ж�����ֻ������Yͨ��
	if (yuvBuffer->Y[h][w] == 0)
		return true;
	else
		return false;
}

/*
1��������ӵ����ֵ��Ϊ��������ӵ��ֵ��
2��������ӵ����ֵ��Ϊ��������ӵ��ֵ��
3��������ӵ����ֵ����Ϊ��������ֵ��С���ӵ���Ϣ����
4�������䡿������ӵ����ֵ��Ϊ0(���ǲ�ɫֵ���ܲ�Ϊ0���������),>������ָ����������
*/
//>�˴��޸��˿ն����ж���׼:��ɫͼ�����ص�Ϊ��ɫ
void CDoubleViewFilling::mergeWithZBuffer(CIYuv *leftColor, CIYuv *leftDepth, CIYuv *rghtColor, CIYuv *rghtDepth, CIYuv *outColor, CIYuv *outDepth)
{
	for (int h = 0; h < m_iHeight; h++) {
		for (int w = 0; w < m_iWidth; w++) {
			if (isHoleinColor(leftColor, h, w) && !isHoleinColor(rghtColor, h, w)) {
				outColor->Y[h][w] = rghtColor->Y[h][w];
				outColor->U[h / 2][w / 2] = rghtColor->U[h / 2][w / 2];
				outColor->V[h / 2][w / 2] = rghtColor->V[h / 2][w / 2];

				outDepth->Y[h][w] = rghtDepth->Y[h][w];
			}
			else if (!isHoleinColor(leftColor, h, w) && isHoleinColor(rghtColor, h, w)) {
				outColor->Y[h][w] = leftColor->Y[h][w];
				outColor->U[h / 2][w / 2] = leftColor->U[h / 2][w / 2];
				outColor->V[h / 2][w / 2] = leftColor->V[h / 2][w / 2];

				outDepth->Y[h][w] = leftDepth->Y[h][w];
			}
			else if (!isHoleinColor(leftColor, h, w) && !isHoleinColor(rghtColor, h, w)) {
				if (leftDepth->Y[h][w] <= rghtDepth->Y[h][w]) {
					outColor->Y[h][w] = leftColor->Y[h][w];
					outColor->U[h / 2][w / 2] = leftColor->U[h / 2][w / 2];
					outColor->V[h / 2][w / 2] = leftColor->V[h / 2][w / 2];

					outDepth->Y[h][w] = leftDepth->Y[h][w];
				}
				else {
					outColor->Y[h][w] = rghtColor->Y[h][w];
					outColor->U[h / 2][w / 2] = rghtColor->U[h / 2][w / 2];
					outColor->V[h / 2][w / 2] = rghtColor->V[h / 2][w / 2];

					outDepth->Y[h][w] = rghtDepth->Y[h][w];
				}
			}
			else if (isHoleinColor(leftColor, h, w) && isHoleinColor(rghtColor, h, w)) {
				//>����ɫ������������
				//outColor->Y[h][w] = 29;
				//outColor->U[h / 2][w / 2] = 255;
				//outColor->V[h / 2][w / 2] = 107;

				//>�����
				outColor->Y[h][w] = 0;
				outColor->U[h / 2][w / 2] = 128;
				outColor->V[h / 2][w / 2] = 128;

				//outDepth->Y[h][w] = 255;
			}
		}
	}
}

/*void CDoubleViewFilling::mergeWithZBuffer(CIYuv *leftColor, CIYuv *leftDepth, CIYuv *rghtColor, CIYuv *rghtDepth, CIYuv *outColor, CIYuv *outDepth)
{
	for (int h = 0; h < m_iHeight; h++) {
		for (int w = 0; w < m_iWidth; w++) {
			if (leftDepth->Y[h][w] == 0 && rghtDepth->Y[h][w] != 0) {
				outColor->Y[h][w] = rghtColor->Y[h][w];
				outColor->U[h / 2][w / 2] = rghtColor->U[h / 2][w / 2];
				outColor->V[h / 2][w / 2] = rghtColor->V[h / 2][w / 2];

				outDepth->Y[h][w] = rghtDepth->Y[h][w];
			}
			else if (leftDepth->Y[h][w] != 0 && rghtDepth->Y[h][w] == 0) {
				outColor->Y[h][w] = leftColor->Y[h][w];
				outColor->U[h / 2][w / 2] = leftColor->U[h / 2][w / 2];
				outColor->V[h / 2][w / 2] = leftColor->V[h / 2][w / 2];

				outDepth->Y[h][w] = leftDepth->Y[h][w];
			}
			else if (leftDepth->Y[h][w] != 0 && rghtDepth->Y[h][w] != 0) {
				if (leftDepth->Y[h][w] <= rghtDepth->Y[h][w]) {
					outColor->Y[h][w] = leftColor->Y[h][w];
					outColor->U[h / 2][w / 2] = leftColor->U[h / 2][w / 2];
					outColor->V[h / 2][w / 2] = leftColor->V[h / 2][w / 2];

					outDepth->Y[h][w] = leftDepth->Y[h][w];
				}
				else {
					outColor->Y[h][w] = rghtColor->Y[h][w];
					outColor->U[h / 2][w / 2] = rghtColor->U[h / 2][w / 2];
					outColor->V[h / 2][w / 2] = rghtColor->V[h / 2][w / 2];

					outDepth->Y[h][w] = rghtDepth->Y[h][w];
				}
			}
			//>�����ص������ӵ����ֵΪ0������ɫͼֵ��Ϊ0����������������(����ֵ������ֵ����������ֵ)
			//>(��������ǰһ�׶α��������㷨����Щ���ص�ԭʼ���ͼ����0����)
			//>�������ͼ��ֵΪ0����ɫͼ��ֵ��һ��Ϊ0
			else if (leftDepth->Y[h][w] == 0 && rghtDepth->Y[h][w] == 0) {
				if (leftColor->Y[h][w] != 0 && rghtColor->Y[h][w] != 0) {
					outColor->Y[h][w] = (char)((leftColor->Y[h][w] + rghtColor->Y[h][w]) / 2);
					outColor->U[h / 2][w / 2] = (char)((leftColor->U[h / 2][w / 2] + rghtColor->U[h / 2][w / 2]) / 2);
					outColor->V[h / 2][w / 2] = (char)((leftColor->V[h / 2][w / 2] + rghtColor->V[h / 2][w / 2]) / 2);
				}
				else if (leftColor->Y[h][w] == 0 && rghtColor->Y[h][w] == 0) {
					outDepth->Y[h][w] = 0;
					continue;
				}
				else if (leftColor->Y[h][w] != 0) {
					outColor->Y[h][w] = leftColor->Y[h][w];
					outColor->U[h / 2][w / 2] = leftColor->U[h / 2][w / 2];
					outColor->V[h / 2][w / 2] = leftColor->V[h / 2][w / 2];
				}
				else if (rghtColor->Y[h][w] != 0) {
					outColor->Y[h][w] = rghtColor->Y[h][w];
					outColor->U[h / 2][w / 2] = rghtColor->U[h / 2][w / 2];
					outColor->V[h / 2][w / 2] = rghtColor->V[h / 2][w / 2];
				}

				outDepth->Y[h][w] = 0;
			}
		}
	}
}*/

/*
1����������ɫͼ���еĺ�ɫ�ն������
   >֮���ԶԲ�ɫͼ����пն��ж�û�����ͼ�ж�����Ϊ�ܶ��ɫ�ն����ֵ��Ϊ0��ֻ�ǽӽ���0
2������ÿն��㴦�����ֵ�ڱ������ֵ��Χ�ڣ����ñ������
3���������ͼ���޸��㷨inpainting
*/
//>ȡ���������ֵ
void CDoubleViewFilling::fillingWithZBuffer(CIYuv *inputColor, CIYuv *inputDepth, CIYuv *bgColor, CIYuv *bgDepth, CIYuv *outColor, CIYuv *outDepth)
{
	for (int h = 0; h < m_iHeight; h++) {
		for (int w = 0; w < m_iWidth; w++) {

			//>�����������ص�Ϊ��ɫ�ն�
			if (isHoleinColor(inputColor, h, w)) {
				outColor->Y[h][w] = bgColor->Y[h][w];
				outColor->U[h / 2][w / 2] = bgColor->U[h / 2][w / 2];
				outColor->V[h / 2][w / 2] = bgColor->V[h / 2][w / 2];

				outDepth->Y[h][w] = inputDepth->Y[h][w];

				//>�Ժ�ɫ������������
				//outColor->Y[h][w] = 76;
				//outColor->U[h / 2][w / 2] = 85;
				//outColor->V[h / 2][w / 2] = 255;

				//outDepth->Y[h][w] = 255;			
			}
			//>������ǿն�
			else {
				outColor->Y[h][w] = inputColor->Y[h][w];
				outColor->U[h / 2][w / 2] = inputColor->U[h / 2][w / 2];
				outColor->V[h / 2][w / 2] = inputColor->V[h / 2][w / 2];

				outDepth->Y[h][w] = inputDepth->Y[h][w];
			}
		}
	}
}

/*void CDoubleViewFilling::fillingWithZBuffer(CIYuv *inputColor, CIYuv *inputDepth, CIYuv *bgColor, CIYuv *bgDepth, CIYuv *outColor, CIYuv *outDepth)
{
	int depthThreshold = 5;
	int sum;
	int Total_y, Total_u, Total_v, Total_d;
	int lefttop_x, lefttop_y, rghtbottom_x, rghtbottom_y;

	for (int h = 0; h < m_iHeight; h++) {
		for (int w = 0; w < m_iWidth; w++) {

			//>�����������ص�Ϊ��ɫ�ն�
			if (isHoleinColor(inputColor, h, w)) {

				//>����ÿն��㴦�����ֵ�ڱ������ֵ��Χ�ڣ��ñ������
				if (inputDepth->Y[h][w] <= bgDepth->Y[h][w] + depthThreshold && inputDepth->Y[h][w] >= bgDepth->Y[h][w] - depthThreshold) {
					//outColor->Y[h][w] = bgColor->Y[h][w];
					//outColor->U[h / 2][w / 2] = bgColor->U[h / 2][w / 2];
					//outColor->V[h / 2][w / 2] = bgColor->V[h / 2][w / 2];

					//outDepth->Y[h][w] = inputDepth->Y[h][w];

					//>�Ժ�ɫ������������
					outColor->Y[h][w] = 76;
					outColor->U[h / 2][w / 2] = 85;
					outColor->V[h / 2][w / 2] = 255;

					outDepth->Y[h][w] = 255;
				}
				//>�������ͼ���޸��㷨inpainting(ǰ���е�����)
				else {
					sum = 0;
					Total_y = 0;
					Total_u = 0;
					Total_v = 0;
					Total_d = 0;

					lefttop_x = CLIP3(w - 2, 0, m_iWidth);
					lefttop_y = CLIP3(h - 2, 0, m_iHeight);
					rghtbottom_x = CLIP3(w + 2, 0, m_iWidth);
					rghtbottom_y = CLIP3(h + 2, 0, m_iHeight);

					//>Make Statistics
					for(int j = lefttop_y; j < rghtbottom_y; j++)
						for (int i = lefttop_x; i < rghtbottom_x; i++) {
							if (isHoleinColor(inputColor, j, i)) {
								Total_y += inputColor->Y[j][i];
								Total_u += inputColor->U[j / 2][i / 2];
								Total_v += inputColor->V[j / 2][i / 2];

								Total_d = inputDepth->Y[j][i];
								
								sum++;
							}
						}

					//>Average to output
					outColor->Y[h][w] = (char)(Total_y / sum);
					outColor->U[h / 2][w / 2] = (char)(Total_u / sum);
					outColor->V[h / 2][w / 2] = (char)(Total_v / sum);

					outDepth->Y[h][w] = (char)(Total_d / sum);
				}
			}
			//>������ǿն�
			else {
				outColor->Y[h][w] = inputColor->Y[h][w];
				outColor->U[h / 2][w / 2] = inputColor->U[h / 2][w / 2];
				outColor->V[h / 2][w / 2] = inputColor->V[h / 2][w / 2];

				outDepth->Y[h][w] = inputDepth->Y[h][w];
			}
		}
	}
}*/

void CDoubleViewFilling::doOneFrame()
{
	//>b)���ںϺ�ն��
	//>temp1:outcolor
	//>temp2:outdepth
	mergeWithZBuffer(m_pcLeftColor, m_pcLeftDepth, m_pcRghtColor, m_pcRghtDepth, temp1, temp2);
	//mergeWithZBuffer(m_pcLeftColor, m_pcLeftDepth, m_pcRghtColor, m_pcRghtDepth, m_pcOutColor, m_pcOutDepth);
	fillingWithZBuffer(temp1, temp2, m_pcBGColor, m_pcBGDepth, m_pcOutColor, m_pcOutDepth);
}
