# 数字图像处理C++


[![ ](https://img.shields.io/badge/%E6%AC%A7%E9%98%B3%E6%96%8C-catchcodes-brightgreen "my_icon")](https://github.com/catchcodes)
[![ ](https://img.shields.io/badge/QQ-%F0%9F%8C%9E%20-yellowgreen "扣扣")](https://wpa.qq.com/msgrd?v=3&uin=1994143440&site=qq&menu=yes&jumpflag=1)
![Github stars](https://img.shields.io/github/stars/catchcodes/DIP_Cpp.svg)

</br>

```
 _______                     __________ 
/  .__. \ ____   ____ ___.__.\______   \
|  |  | |/  _ \ /  _ \  ||  | |    |  _/
|  |__|  ( <_> )  <_> )___  | |    |   \
 \______/\____/ \____// ____| |______  /
                      \/             \/ 
```

***
This project is based on [Visual Studio 2022](https://visualstudio.microsoft.com/zh-hans/vs/) IDE</br>
Using [Opencv 4.7.0](https://opencv.org/releases/)
***
Codes of Digital-Image-Processing homework

* 直方图均衡化
  <details>
  <summary><font size="3" color="yellowgreen">代码展开</font></summary>
  <pre><code class="language-cpp">
    // 直方图均衡化的定义
    void HistEqual(Mat& gray, Mat& result)
    {
        // 哈希表统计0~255像素值的个数
        map<int, int>pixelCounter;
        for (int i = 0; i < gray.rows; i++)
        {
            for (int j = 0; j < gray.cols; j++)
            {
                int value = gray.at<uchar>(i, j);
                pixelCounter[value]++;
            }
        }
        //统计0~255像素值的频率，并计算累计频率
        map<int, double> pixel_fre;
        int pixel_sum = gray.cols * gray.rows;
        double cumul_fre = 0;
        for (int i = 0; i < 256; i++)
        {
            // 累计频率 哪些灰度数量多，映射曲线越陡峭，均衡后越均匀
            cumul_fre += double(pixelCounter[i]) / pixel_sum;
            pixel_fre[i] = cumul_fre;
        }
        //根据累计频率进行转换
        for (int i = 0; i < gray.rows; i++)
        {
            for (int j = 0; j < gray.cols; j++)
            {
                int value = gray.at<uchar>(i, j);
                double fre = pixel_fre[value];
                // 原始灰度值乘以累计频率
                result.at<uchar>(i, j) = fre * value;
            }
        }
    }
  </code></pre>
  </details>

* 拉普拉斯锐化
  <details>
  <summary><font size="3" color="yellow">代码展开</font></summary>
  <pre><code class="language-cpp">
    // 默认0填充
    void Laplacian(Mat& gray, Mat& result, int padding)
    {
        //result.convertTo(result, CV_64F);
        Mat gray_buf(gray.rows + 2, gray.cols + 2, gray.depth());
        // 0填充
        if (padding == 0)
        {
            cv::copyMakeBorder(gray, gray_buf, 1, 1, 1, 1, cv::BORDER_CONSTANT);
        }
        // 镜像填充
        else if (padding == 1)
        {
            cv::copyMakeBorder(gray, gray_buf, 1, 1, 1, 1, cv::BORDER_REFLECT);
        }
        for (int i = 0; i < gray.rows; i++)
        {
            for (int j = 0; j < gray.cols; j++)
            {
                // cv::saturate_cast<uchar>()保证范围为0~255
                // 直接访问
                result.at<uchar>(i, j) = cv::saturate_cast<uchar>(gray.at<uchar>(i, j) + 8 * gray_buf.at<uchar>(i + 1, j + 1) - gray_buf.at<uchar>(i, j) - gray_buf.at<uchar>(i, j + 1) - gray_buf.at<uchar>(i, j + 2) \
                    - gray_buf.at<uchar>(i + 1, j) - gray_buf.at<uchar>(i + 1, j + 2) - gray_buf.at<uchar>(i + 2, j) - gray_buf.at<uchar>(i + 2, j + 1) - gray_buf.at<uchar>(i + 2, j + 2));
            }
        }
    }
  </code></pre>
  </details>

* 理想低通滤波器
  <details>
  <summary><font size="3" color=#98FB98>代码展开</font></summary>
  <pre><code class="language-cpp">
	// 理想低通滤波器
	void ILPF(Mat& gray, Mat& result, int fc)
	{
		// 扩展图像矩阵，为2，3，5的倍数时运算速度快
		int m = cv:: getOptimalDFTSize(gray.rows);
		int n = cv::getOptimalDFTSize(gray.cols);
		Mat padded;
		// 零填充
		cv::copyMakeBorder(gray, padded, 0, m - gray.rows, 0, n - gray.cols, cv::BORDER_CONSTANT);
		padded.convertTo(padded, CV_32FC1);
		int row = padded.rows;
		int col = padded.cols;
		if (fc > MIN(row, col))	
			throw "截止频率超出图像范围";
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
		// imshow("滤波器", filter);
		// 实部和虚部
		Mat plane[] = {padded, Mat::zeros(padded.size(), CV_32FC1)};
		Mat complexIm;
		merge(plane, 2, complexIm); //合并通道 （把两个矩阵合并为一个2通道的Mat类容器）
		dft(complexIm, complexIm);  //进行傅立叶变换，结果保存在自身
		split(complexIm, plane);    //分离通道
		fftshift(plane[0], plane[1]);
		Mat Real, Imag, BLUR;
		Real = plane[0].mul(filter);
		Imag = plane[1].mul(filter);
		// fftshift(Real, Imag); //效果一样 周期性
		Mat plane1[] = { Real, Imag };
		merge(plane1, 2, BLUR);//实部与虚部合并
		idft(BLUR, BLUR);
		split(BLUR, plane);                     //分离通道，主要获取通道
		magnitude(plane[0], plane[1], result);  //求幅值(模)
		normalize(result, result, 0, 1.0, NORM_MINMAX);  //归一化便于显示
	}
	// 低频移动到中心
	void fftshift(Mat plane0, Mat plane1)
	{
		// -2 : 1111_……_1110
		plane0 = plane0(Rect(0, 0, plane0.cols & -2, plane0.rows & -2));
		int cx = plane0.cols / 2;
		int cy = plane0.rows / 2;
		Mat part1_r(plane0, Rect(0, 0, cx, cy));
		Mat part2_r(plane0, Rect(cx, 0, cx, cy));
		Mat part3_r(plane0, Rect(0, cy, cx, cy));
		Mat part4_r(plane0, Rect(cx, cy, cx, cy));
		Mat temp;
		part1_r.copyTo(temp);  //左上与右下交换位置(实部)
		part4_r.copyTo(part1_r);
		temp.copyTo(part4_r);
		part2_r.copyTo(temp);  //右上与左下交换位置(实部)
		part3_r.copyTo(part2_r);
		temp.copyTo(part3_r);
		Mat part1_i(plane1, Rect(0, 0, cx, cy));  //元素坐标(cx,cy)
		Mat part2_i(plane1, Rect(cx, 0, cx, cy));
		Mat part3_i(plane1, Rect(0, cy, cx, cy));
		Mat part4_i(plane1, Rect(cx, cy, cx, cy));
		part1_i.copyTo(temp);  //左上与右下交换位置(虚部)
		part4_i.copyTo(part1_i);
		temp.copyTo(part4_i);
		part2_i.copyTo(temp);  //右上与左下交换位置(虚部)
		part3_i.copyTo(part2_i);
		temp.copyTo(part3_i);
	}
  </code></pre>
  </details>

* 巴特沃斯低通滤波器
  <details>
  <summary><font size="3" color=#FAFAD2>代码展开</font></summary>
  <pre><code class="language-cpp">
	// 巴特沃斯的滤波器
  	Mat filter = Mat::zeros(padded.size(), CV_32FC1);
	for (int i = 0; i < row; i++)
	{
		//float* data = filter.ptr<float>(i);
		for (int j = 0; j < col; j++)
		{
			float d = sqrt(pow((i - row / 2.0), 2) + pow((j - col / 2.0), 2));
			filter.at<float>(i, j) = 1 / (1 + pow(float(d / fc), 2 * level));
		}
	}
  </code></pre>
  </details>

* 高斯低通滤波器
  <details>
  <summary><font size="3" color=#FFDEAD>代码展开</font></summary>
  <pre><code class="language-cpp">
    // 高斯滤波器
  	Mat filter = Mat::zeros(padded.size(), CV_32FC1);
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			float d = sqrt(pow((i - row / 2.0), 2) + pow((j - col / 2.0), 2));
			filter.at<float>(i, j) = exp(-pow(d, 2) / (2 * pow(fc, 2)));
		}
	}
  </code></pre>
  </details>

* 人脸检测（Python）
  <details>
  <summary><font size="3" color=#F08080>代码展开</font></summary>
  <pre><code class="language-python">
  # HOG人脸检测器寻找人脸
  def find_face(filename):
    face_detector = dlib.get_frontal_face_detector()  # 创建HOG人脸检测器
    image = cv2.imread(filename)
    detected_faces = face_detector(image, 1)
	# 存储人脸矩形框的坐标信息
    location = []
    for i, face_rect in enumerate(detected_faces):
        location.append(face_rect.left())
        location.append(face_rect.top())
        location.append(face_rect.right())
        location.append(face_rect.bottom())
    return location
  </code></pre>
  </details>

* 人脸81个特征点位置（Python）
  <details>
  <summary><font size="3" color=#FF8C00>代码展开</font></summary>
  <pre><code class="language-python">
  def find_face_landmarks(filename):
    # 人脸81个关键点模型位置 
    predictor_model = r"E:\Face_Landmarks\shape_predictor_81_face_landmarks.dat"
    face_detector = dlib.get_frontal_face_detector()  # 创建HOG人脸检测器
    face_pose_predictor = dlib.shape_predictor(predictor_model)  # 创建人脸特征点检测器
    image = cv2.imread(filename)
    detected_faces = face_detector(image, 1)
    mark = []
    for i, face_rect in enumerate(detected_faces):
        pose_landmarks = face_pose_predictor(image, face_rect)  # 获取面部的姿势
        for j in range(81):
		    # 以tuple形式存放到mark列表中，方便C++调用
            mark.append((pose_landmarks.part(j).x, pose_landmarks.part(j).y))            
    return mark
  </code></pre>
  </details>

To be continued……

```
								  _ooOoo_
								 o8888888o
								 88" . "88
								 (| -_- |)
								 O\  =  /O
							  ____/`---'\____
							.'  \\|     |//  `.
						   /  \\|||  :  |||//  \
						  /  _||||| -:- |||||-  \
						  |   | \\\  -  /// |   |
						  | \_|  ''\---/''  |   |
						  \  .-\__  `-`  ___/-. /
						___`. .'  /--.--\  `. . __
					 ."" '<  `.___\_<|>_/___.'  >'"".
					| | :  `- \`.;`\ _ /`;.`/ - ` : | |
					\  \ `-.   \_ __\ /__ _/   .-` /  /
			   ======`-.____`-.___\_____/___.-`____.-'======
								  `=---='
			   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
```
