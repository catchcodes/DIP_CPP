#include "function.h"
#include <core/core_c.h>

using namespace cv;
using std::map;


// �����ͨ�˲���
void ILPF(Mat& gray, Mat& result, int fc)
{
	// ��չͼ�����Ϊ2��3��5�ı���ʱ�����ٶȿ�
	int m = cv:: getOptimalDFTSize(gray.rows);
	int n = cv::getOptimalDFTSize(gray.cols);
	Mat padded;
	// �����
	cv::copyMakeBorder(gray, padded, 0, m - gray.rows, 0, n - gray.cols, cv::BORDER_CONSTANT);
	padded.convertTo(padded, CV_32FC1);
	int row = padded.rows;
	int col = padded.cols;
	if (fc > MIN(row, col))	
		throw "��ֹƵ�ʳ���ͼ��Χ";
	Mat filter = Mat::zeros(padded.size(), CV_32FC1);
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			double d = sqrt(pow((i - row / 2.0), 2) + pow((j - col / 2.0), 2));
			if (d <= fc)
			{
				filter.at<float>(i, j) = 1;
			}
		}
	}
	// imshow("�˲���", filter);
	// ʵ�����鲿
	Mat plane[] = {padded, Mat::zeros(padded.size(), CV_32FC1)};
	Mat complexIm;
	merge(plane, 2, complexIm); //�ϲ�ͨ�� ������������ϲ�Ϊһ��2ͨ����Mat��������
	dft(complexIm, complexIm);  //���и���Ҷ�任���������������
	split(complexIm, plane);    //����ͨ��

	fftshift(plane[0], plane[1]);

	Mat Real, Imag, BLUR;
	Real = plane[0].mul(filter);
	Imag = plane[1].mul(filter);
	// fftshift(Real, Imag); //Ч��һ�� ������
	Mat plane1[] = { Real, Imag };

	merge(plane1, 2, BLUR);//ʵ�����鲿�ϲ�

	idft(BLUR, BLUR);
	split(BLUR, plane);                     //����ͨ������Ҫ��ȡͨ��
	magnitude(plane[0], plane[1], result);  //���ֵ(ģ)
	normalize(result, result, 0, 1.0, NORM_MINMAX);  //��һ��������ʾ
}

void fftshift(Mat plane0, Mat plane1)
{
	// -2 : 1111_����_1110
	plane0 = plane0(Rect(0, 0, plane0.cols & -2, plane0.rows & -2));
	int cx = plane0.cols / 2;
	int cy = plane0.rows / 2;

	Mat part1_r(plane0, Rect(0, 0, cx, cy));
	Mat part2_r(plane0, Rect(cx, 0, cx, cy));
	Mat part3_r(plane0, Rect(0, cy, cx, cy));
	Mat part4_r(plane0, Rect(cx, cy, cx, cy));

	Mat temp;
	part1_r.copyTo(temp);  //���������½���λ��(ʵ��)
	part4_r.copyTo(part1_r);
	temp.copyTo(part4_r);
	part2_r.copyTo(temp);  //���������½���λ��(ʵ��)
	part3_r.copyTo(part2_r);
	temp.copyTo(part3_r);

	Mat part1_i(plane1, Rect(0, 0, cx, cy));  //Ԫ������(cx,cy)
	Mat part2_i(plane1, Rect(cx, 0, cx, cy));
	Mat part3_i(plane1, Rect(0, cy, cx, cy));
	Mat part4_i(plane1, Rect(cx, cy, cx, cy));

	part1_i.copyTo(temp);  //���������½���λ��(�鲿)
	part4_i.copyTo(part1_i);
	temp.copyTo(part4_i);
	part2_i.copyTo(temp);  //���������½���λ��(�鲿)
	part3_i.copyTo(part2_i);
	temp.copyTo(part3_i);
}
