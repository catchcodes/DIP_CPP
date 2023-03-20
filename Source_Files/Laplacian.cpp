#include "function.h"

using cv::Mat;
using std::map;

// Ĭ��0���
void Laplacian(Mat& gray, Mat& result, int padding)
{
    //result.convertTo(result, CV_64F);
    Mat gray_buf(gray.rows + 2, gray.cols + 2, gray.depth());
    // 0���
    if (padding == 0)
    {
        cv::copyMakeBorder(gray, gray_buf, 1, 1, 1, 1, cv::BORDER_CONSTANT);
    }
    // �������
    else if (padding == 1)
    {
        cv::copyMakeBorder(gray, gray_buf, 1, 1, 1, 1, cv::BORDER_REFLECT);
    }
    for (int i = 0; i < gray.rows; i++)
    {
        uchar* grayptr = gray.ptr<uchar>(i);
        uchar* resultptr = result.ptr<uchar>(i);
        for (int j = 0; j < gray.cols; j++)
        {
            // cv::saturate_cast<uchar>()��֤��ΧΪ0~255
            // ֱ�ӷ���
            resultptr[j] = cv::saturate_cast<uchar>(grayptr[j] + 8 * gray_buf.at<uchar>(i + 1, j + 1) - gray_buf.at<uchar>(i, j) - gray_buf.at<uchar>(i, j + 1) - gray_buf.at<uchar>(i, j + 2) \
                - gray_buf.at<uchar>(i + 1, j) - gray_buf.at<uchar>(i + 1, j + 2) - gray_buf.at<uchar>(i + 2, j) - gray_buf.at<uchar>(i + 2, j + 1) - gray_buf.at<uchar>(i + 2, j + 2));
        }
    }
}