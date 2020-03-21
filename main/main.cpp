// OPENCV3_HW.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include "opencv2\opencv.hpp"
#include <iostream>
#include <math.h>
#include <cmath>
#define PI 3.14159265
using namespace cv;
using namespace std;


unsigned char DrawFrame_OR_GetPoint = 1;        //1畫框，2取點
void on_mouse(int EVENT, int x, int y, int flags, void* ustc);



//If you have an idea what size circles you are looking for, 
//then it would be best to set min_radius and max_radius accordingly.Otherwise, 
//it will return anything circular of any size.
//
//Parameters 1 and 2 don't affect accuracy as such, more reliability. Param 1 will set the sensitivity; how strong the edges of the circles need to be.
//Too high and it won't detect anything, too low and it will find too much clutter.
//Param 2 will set how many edge points it needs to find to declare that it's found a circle. Again, too high will detect nothing, 
//too low will declare anything to be a circle. The ideal value of param 2 will be related to the circumference of the circles.

//void mouseEvent(int event, int x, int y, int flags, void *param)
//{
//	IplImage* img = (IplImage*)param;
//	if (event == EVENT_LBUTTONDBLCLK);
//	{
//		printf("%d,%d\n", x, y);
//	}
//}


//define parameters
float scale = 0.25;

int iteration  = 10;
int window_size = 9;
int upperbound = 800;

int way = 1;
//way =1 if using SAD, 2 using SSD 3 using Cfg

int half_window_size = window_size / 2;
int half_upperbound = upperbound / 2;

//getting window search range 
//show image result
//
//for (size_t i = 0; i < circles.size(); i++)
//{
//	Vec3i c = circles[i];
//	Point center = Point(c[0], c[1]);
//	// circle center
//	circle(src, center, 1, Scalar(0, 100, 100), 3, LINE_AA);
//	// circle outline
//	int radius = c[2];
//	circle(src, center, radius, Scalar(255, 0, 255), 3, LINE_AA);
//}

/*cout << "total number of points: " << circles.size() << endl;


for (size_t j = 0; j < circles.size(); j++)
{
Vec3i c = circles[j];
cout << "x : " << c[0] << " y : " << c[1] << endl;
cout << j << " original x : " << arrx[j] << "  original y : " << arry[j] << endl;

}*/
Mat1f windowrange(const Mat1f &img, int x, int y) {
	if (window_size > 0) {
		Range window_range_y(y - half_window_size, y + half_window_size + 1);
		//中心點-半window到中心點+半window
		Range window_range_x(x - half_window_size, x + half_window_size + 1);
		//中心點-半window到中心點+半window
		return img(window_range_y, window_range_x);
	}
	else if (window_size < 0) {
		Range window_range_y(0, 0);
		
		Range window_range_x(0, 0);

		return img(window_range_y, window_range_x);
	

	}
}


void CreatTxt(char* pathName, unsigned char* rBuffer, int length)//建立txt檔案
{
	char* path = "C:\\Users\\Security\\Downloads\\OPENCV3_HW\\OPENCV3_HW\\OPENCV3_HW\\Circules.txt"; // 你要建立檔案的路徑
	ofstream fout(pathName);
	if (fout) { // 如果建立成功
		for (int i = 0; i < length; i++)
		{
			fout << "test" << endl; // 使用與cout同樣的方式進行寫入


		}

		fout.close();  // 執行完操作後關閉檔案控制代碼
	}
}
void DSImap(const Mat1f	&imgone, const Mat1f &imgtwo, Mat1f &disparity) {
	//int imagesize = imgone.size();

	if (window_size > 0) {
		//初始disparity 
		disparity = Mat1f::zeros(imgone.size());
		int y = 0 + half_window_size;
		int x = 0 + half_window_size;
		//while (y < imgone.rows - half_window_size) {
		for(int y = half_window_size;y < imgone.rows- half_window_size;++y){

			//while (x < imgone.cols - half_window_size) {
			for(int x = half_window_size;x<imgone.cols-half_window_size;++x){
				//find the range
				Mat1f search_range_L = windowrange(imgone, x, y);
				//search each line
				float best_disparity = 0;
				float SAD_MIN = 878787878787;
				float SSD_MIN = 878787878787;
				float Cfg_MIN = 878787878787;

				int xMIN = max(x - half_upperbound, half_window_size);
				int xMAX = min(x + half_upperbound, imgone.cols - half_window_size);

				for (int x_Dst = xMIN; x_Dst < xMAX; ++x_Dst) {
					Mat1f search_range_R = windowrange(imgtwo, x_Dst, y);
					//find difference

					if (way == 1) {
						Mat1f diff = abs(search_range_L - search_range_R);
						float SAD = 0;

						SAD = sum(diff)[0];
						//check for the update

						if (SAD < SAD_MIN) {
							SAD_MIN = SAD;
							best_disparity = abs(x_Dst - x);//found disparity
						}
						else {

						}
					}
					if (way == 2) {
						Mat1f diff = (search_range_L-search_range_R)*(search_range_L - search_range_R);
						float SSD = 0;
						SSD = sum(diff)[0];
						/*
						double minv = 0.0, maxv = 0.0;
						double* minp = &minv;
						double* maxp = &maxv;	
						minMaxIdx(diff, minp, maxp);
						MEAN = minv;*/
						if (SSD <SSD_MIN) {
							SSD_MIN = SSD;
							best_disparity = abs(x_Dst - x);
						}
					}

					if (way == 3) {
						Mat1f diff =  search_range_L*search_range_R;
						float Cfg = 0;
						Cfg = sum(diff)[0];
						/*
						double minv = 0.0, maxv = 0.0;
						double* minp = &minv;
						double* maxp = &maxv;
						minMaxIdx(diff, minp, maxp);
						MEAN = minv;*/
						if (Cfg <Cfg_MIN) {
							Cfg_MIN = Cfg;
							best_disparity = abs(x_Dst - x);
						}
					}
					
				}
				disparity(y, x) = best_disparity;

				//++x;

			}
			//++y;
		}
	}
}
int main()
{
	
	//Mat1f imgL;
	//Mat1f imgR;

	//char fileName[] = "view0.png";
	//Mat image1, grayImage1,image2, grayImage2;
	//image1 = cvLoadImage(fileName);
	//grayImage1 = cvCreateImage(cvGetSize(image1), IPL_DEPTH_8U, 1);//1代表灰階

	//if (!image1 || !grayImage1)
	//{
	//	cout << "找不到檔案!!!" << endl;
	//}

	//else
	//{
	//	
	//	cvCvtColor(image1, grayImage1, CV_RGB2GRAY);//RGB to Gray
	//	//cvShowImage("gray image", grayImage1);
	//	//cvWaitKey(0);
	//}
	//imwrite("view01_gray.png", grayImage1);
	//system("pause");
	//return 0;
	clock_t time = clock();

	Mat1f imageright, imageleft;
	Mat img1_gray = imread("view0.png", 0);
	Mat img2_gray = imread("view1.png", 0);

	Mat1f left_dis;
	Mat1f right_dis;

	imwrite("view0_gray.png", img1_gray);
	imwrite("view1_gray.png", img2_gray);
	imread("view1.png", 0).convertTo(imageright, CV_32FC1);
	imread("view0.png", 0).convertTo(imageleft, CV_32FC1);


	
	resize(imageright, imageright, Size(), 1,1);
	resize(imageleft, imageleft, Size(), 1, 1);

	
	
	 
	DSImap(imageright, imageleft, right_dis); 
	DSImap(imageleft, imageright, left_dis); 
	
	clock_t time2 = clock();


	double total_time = time2 - time;

	/*if (total_time > 2000000) {
		cout << "time limit exit";
	} */                                                             
	printf("time: %f sec\n", total_time / CLOCKS_PER_SEC);


	//do normalization
	normalize(left_dis, left_dis, 0, 1, NORM_MINMAX);
	normalize(right_dis, right_dis, 0, 1, NORM_MINMAX);
	imshow("disparityL", left_dis);
	imshow("disparityR", right_dis);

	//wrire file
	imwrite("disparityL.png", left_dis * 255);
	imwrite("disparityR.png", right_dis * 255);

	/*resize(left_dis, left_dis, Size(), 2, 2);
	resize(right_dis, right_dis, Size(),2, 2);

	imwrite("disparityL.png", left_dis * 255);
	imwrite("disparityR.png", right_dis * 255);
*/

	waitKey();

	return 0;
}
