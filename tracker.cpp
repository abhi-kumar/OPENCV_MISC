#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "opencv2/video/tracking.hpp"

using namespace cv;
using namespace std;

IplImage* frame, * img1;
CvPoint point;
int drag = 0;
int x_point,width_point,y_point,height_point;
CvCapture *capture = 0;
int key = 0;
CvRect rect;
Rect region_of_interest;
int test;
Mat src,src_gray,image,src_gray_prev,src1,src_gray1,copy,copy1,frames,copy2;
int maxCorners = 23;
RNG rng(12345);
vector<Point2f> corners,corners_prev,corners_temp;
double qualityLevel = 0.01;
double minDistance = 10;
int blockSize = 3;
bool useHarrisDetector = false;
double k = 0.04;
vector<uchar> status;
vector<float> err;
float x_cord[100];
float y_cord[100];

void mouseHandler(int event, int x, int y, int flags, void* param)
{
    if (event == CV_EVENT_LBUTTONDOWN && !drag)
    {
        point = cvPoint(x, y);
        drag = 1;
    }
    
    if (event == CV_EVENT_MOUSEMOVE && drag)
    {
        img1 = cvCloneImage(frame);
        cvRectangle(img1,point,cvPoint(x, y),CV_RGB(255, 0, 0),1,8,0);
        cvShowImage("result", img1);
    }
    
    if (event == CV_EVENT_LBUTTONUP && drag)
    {
        rect = cvRect(point.x,point.y,x-point.x,y-point.y);
		x_point = point.x;
		y_point = point.y;
		width_point = x-point.x;
		height_point = y-point.y;
        cvShowImage("result", frame);
        drag = 0;
    }

    
    if (event == CV_EVENT_RBUTTONUP)
    {
        drag = 0;
    }
}

int main(int argc, char *argv[])
{
    capture = cvCaptureFromCAM( 0 ); 
    if ( !capture ) {
        printf("Cannot open initialize webcam!\n" );
        exit(0);
    }
    cvNamedWindow( "result", CV_WINDOW_AUTOSIZE );
	int small,big; //declares integer
    
	int x = 1;	

    while( key != 'q' )
    {
        frame = cvQueryFrame( capture );
        if (rect.width>0)
		{	
			if(corners.size() == 0 || x==0)
			{
				Mat frames(frame);
				src = frames.clone();
				cvtColor( src, src_gray, CV_BGR2GRAY );
				cv::Mat mask1 = cv::Mat::zeros(src.size(), CV_8UC1);  
				cv::Mat roi(mask1, cv::Rect(x_point,y_point,width_point,height_point));
				roi = cv::Scalar(255, 255, 255);
				copy1 = src.clone();		
				goodFeaturesToTrack( src_gray,
    		           corners,
    		           maxCorners,
    		           qualityLevel,
    		           minDistance,
    		           mask1,
    		           blockSize,
    		           useHarrisDetector,
    	           k );

				int rad = 3;
  				for( int i = 0; i < corners.size(); i++ )
  				   { circle( copy1, corners[i], rad, Scalar(rng.uniform(0,255), rng.uniform(0,255),
  			            rng.uniform(0,255)), -1, 8, 0 );
			
					}
				IplImage test1 = copy1;
			  	IplImage* test2 = &test1;
				x = 1;

			    cvShowImage("result", test2);
			}

			else
				{
					src_gray_prev = src_gray.clone();
					corners_prev = corners;
					Mat framess(frame);
					src = framess.clone();
					cvtColor( src, src_gray, CV_BGR2GRAY ); 
					cv::Mat mask = cv::Mat::zeros(src.size(), CV_8UC1);  
					cv::Mat roi(mask, cv::Rect(x_point,y_point,width_point,height_point));
					roi = cv::Scalar(255, 255, 255);	
					Mat copy;
  					copy = src.clone();
					goodFeaturesToTrack( src_gray,
    		           corners,
    		           maxCorners,
    		           qualityLevel,
    		           minDistance,
    		           mask,
    		           blockSize,
    		           useHarrisDetector,
    		           k );
	
					calcOpticalFlowPyrLK(src_gray_prev, src_gray, corners_prev, corners, status, err);
			
  					int r = 3;
  					for( int i = 0; i < corners.size(); i++ )
    				 { circle( copy, corners[i], r, Scalar(rng.uniform(0,255), rng.uniform(0,255),
    			          rng.uniform(0,255)), -1, 8, 0 );
					 x_cord[i] = corners[i].x;
					 y_cord[i] = corners[i].y;
					 
					}			
					IplImage test3 = copy;
					IplImage* test4 = &test3;
					cvShowImage("result", test4);			
					
				}

			
	
    	}        

        cvSetMouseCallback("result", mouseHandler, NULL);
        key = cvWaitKey(10);
        if( (char) key== 'r' )
	{ 
		rect = cvRect(0,0,0,0); cvResetImageROI(frame);
		x = 0;
	}
        cvShowImage("result", frame);
//	cout << x_point << " " << y_point <<  " " << width_point << " " << height_point<< endl;

    }
    cvDestroyWindow("result");
    cvReleaseImage(&img1);
    return 0;
}
