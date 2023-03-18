#include "function.h"

using namespace cv;
using std::map;

// ��˹��ͨ�˲���
void GLPF(Mat& gray, Mat& result, int fc)
{
	// ��չͼ�����Ϊ2��3��5�ı���ʱ�����ٶȿ�
	int m = cv::getOptimalDFTSize(gray.rows);
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
			float d = sqrt(pow((i - row / 2.0), 2) + pow((j - col / 2.0), 2));
			filter.at<float>(i, j) = exp(-pow(d, 2) / (2 * pow(fc, 2)));
		}
	}
	// ʵ�����鲿
	Mat plane[] = { padded, Mat::zeros(padded.size(), CV_32FC1) };
	Mat complexIm;
	merge(plane, 2, complexIm); //�ϲ�ͨ�� ������������ϲ�Ϊһ��2ͨ����Mat��������
	dft(complexIm, complexIm);  //���и���Ҷ�任���������������
	split(complexIm, plane);    //����ͨ��

	fftshift(plane[0], plane[1]);

	Mat Real, Imag, BLUR;
	Real = plane[0].mul(filter);
	Imag = plane[1].mul(filter);
	Mat plane1[] = { Real, Imag };

	merge(plane1, 2, BLUR);//ʵ�����鲿�ϲ�

	idft(BLUR, BLUR);
	split(BLUR, plane);                     //����ͨ������Ҫ��ȡͨ��
	magnitude(plane[0], plane[1], result);  //���ֵ(ģ)
	normalize(result, result, 0, 1.0, NORM_MINMAX);  //��һ��������ʾ
}
