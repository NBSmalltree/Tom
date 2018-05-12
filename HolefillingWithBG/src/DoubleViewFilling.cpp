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
1、判断是否为彩色图中的空洞
*/
bool CDoubleViewFilling::isHoleinColor(CIYuv *yuvBuffer, int h, int w)
{
	if (yuvBuffer->Y[h][w] == 0 &&
		(yuvBuffer->U[h / 2][w / 2] >= 125 && yuvBuffer->U[h / 2][w / 2] <= 131) &&
		(yuvBuffer->V[h / 2][w / 2] >= 125 && yuvBuffer->V[h / 2][w / 2] <= 131))
		return true;
	else
		return false;
}

/*
1、如果左视点深度值不为零就用左视点的值填
2、如果右视点深度值不为零就用右视点的值填
3、如果两视点深度值均不为零就用深度值较小的视点信息来填
*/
void CDoubleViewFilling::mergeWithZBuffer(CIYuv *leftColor, CIYuv *leftDepth, CIYuv *rghtColor, CIYuv *rghtDepth, CIYuv *outColor, CIYuv *outDepth)
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
		}
	}
}

/*
1、针对输入彩色图像中的黑色空洞进行填补
   >之所以对彩色图像进行空洞判断没用深度图判断是因为很多彩色空洞深度值不为0，只是接近于0
2、如果该空洞点处的深度值在背景深度值范围内，则用背景填充
3、否则采用图像修复算法inpainting
*/
void CDoubleViewFilling::fillingWithZBuffer(CIYuv *inputColor, CIYuv *inputDepth, CIYuv *bgColor, CIYuv *bgDepth, CIYuv *outColor, CIYuv *outDepth)
{
	int depthThreshold = 5;
	int sum;
	int Total_y, Total_u, Total_v, Total_d;
	int lefttop_x, lefttop_y, rghtbottom_x, rghtbottom_y;

	for (int h = 0; h < m_iHeight; h++) {
		for (int w = 0; w < m_iWidth; w++) {

			//>如果输入的像素点为黑色空洞
			if (isHoleinColor(inputColor, h, w)) {

				//>如果该空洞点处的深度值在背景深度值范围内，用背景填充
				if (inputDepth->Y[h][w] <= bgDepth->Y[h][w] + depthThreshold && inputDepth->Y[h][w] >= bgDepth->Y[h][w] - depthThreshold) {
					outColor->Y[h][w] = bgColor->Y[h][w];
					outColor->U[h / 2][w / 2] = bgColor->U[h / 2][w / 2];
					outColor->V[h / 2][w / 2] = bgColor->V[h / 2][w / 2];

					outDepth->Y[h][w] = inputDepth->Y[h][w];
				}
				//>否则采用图像修复算法inpainting
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
					outColor->Y[h][w] = (char)Total_y / sum;
					outColor->U[h / 2][w / 2] = (char)Total_u / sum;
					outColor->V[h / 2][w / 2] = (char)Total_v / sum;

					outDepth->Y[h][w] = (char)Total_d / sum;
				}
			}
			else {
				outColor->Y[h][w] = inputColor->Y[h][w];
				outColor->U[h / 2][w / 2] = inputColor->U[h / 2][w / 2];
				outColor->V[h / 2][w / 2] = inputColor->V[h / 2][w / 2];

				outDepth->Y[h][w] = inputDepth->Y[h][w];
			}
		}
	}
}

void CDoubleViewFilling::doOneFrame()
{
	//>b)先融合后空洞填补
	//>temp1:outcolor
	//>temp2:outdepth
	mergeWithZBuffer(m_pcLeftColor, m_pcLeftDepth, m_pcRghtColor, m_pcRghtDepth, temp1, temp2);
	fillingWithZBuffer(temp1, temp2, m_pcBGColor, m_pcBGDepth, m_pcOutColor, m_pcOutDepth);
}
