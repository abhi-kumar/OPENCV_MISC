#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "opencv2/video/tracking.hpp"
#include <math.h>
#include <time.h>

using namespace cv;
using namespace std;

float MHI_DURATION = 0.05;                
int DEFAULT_THRESHOLD = 32;
float MAX_TIME_DELTA = 12500.0;
float MIN_TIME_DELTA = 5;
int visual_trackbar = 2;

void draw_motion_comp(Mat& img, int x_coordinate, int y_coordinate, int width, int height, double angle,Mat& result);

Mat frame;

int main(int argc, char** argv)
{
	namedWindow("Motion_tracking",CV_WINDOW_AUTOSIZE);
	string values[4] = {"input", "frame_diff", "motion_hist", "grad_orient"};
	createTrackbar( "visual", "Motion_tracking", &visual_trackbar, 3, NULL );
	createTrackbar("threshold", "Motion_tracking", &DEFAULT_THRESHOLD, 255, NULL);
	VideoCapture cap;
    	cap.open(0);
	if ( !cap.isOpened() )  // if not success, exit program
	{
		cout << "Cannot open the video file" << endl;
	        return -1;
    	}
	Mat frame,ret,frame_diff,gray_diff,motion_mask;
	for(int i = 0; i<10; i++)
	{
		cap.read(frame);
		Size frame_size = frame.size();
		int h = frame_size.height;
		int w = frame_size.width;
		if(i==5)
			break;
	}
	

	cap.open(0);

    	if (!cap.isOpened())  // if not success, exit program
    	{
		cout << "Cannot open the video cam" << endl;
        	return -1;
    	}

	ret = frame.clone();
	Size frame_size = frame.size();
	int h = frame_size.height;
	int w = frame_size.width;
	double timestamp = 1000.0*clock()/CLOCKS_PER_SEC;
	Mat prev_frame = frame.clone();
	Mat motion_history(h,w, CV_32FC1,Scalar(0,0,0));
	Mat hsv(h,w, CV_8UC3,Scalar(0,255,0));
	Mat mg_mask(h,w, CV_8UC1,Scalar(0,0,0));
	Mat mg_orient(h,w, CV_32FC1,Scalar(0,0,0));
	Mat seg_mask(h,w, CV_32FC1,Scalar(0,0,0));
	vector<Rect> seg_bounds;
	String visual_name;
	Mat vis(h,w,CV_32FC3);
	Mat vis1(h,w,CV_8UC1);
	Mat silh_roi,orient_roi,mask_roi,mhi_roi;
	VideoWriter output;
	output.open ( "outputVideo.avi", CV_FOURCC('D','I','V','X'), 80, cv::Size ( 640,480), true );
	while(1)
	{
		cap.retrieve(frame);
		cap.read(frame);
		ret = frame.clone();
		if (!ret.data) //if not success, break loop
		{
                        cout << "video ended" << endl;
			break;
		} 
		absdiff(frame, prev_frame, frame_diff);
		cvtColor(frame_diff,gray_diff, CV_BGR2GRAY );
		threshold(gray_diff,ret,DEFAULT_THRESHOLD,255,0);
		motion_mask = ret.clone();
		double timestamp = 1000.0*clock()/CLOCKS_PER_SEC;
		updateMotionHistory(motion_mask, motion_history, timestamp, MHI_DURATION);			
		calcMotionGradient(motion_history, mg_mask, mg_orient, 5, 12500.0, 3);
		segmentMotion(motion_history, seg_mask, seg_bounds, timestamp, 32);
		
		visual_name = values[visual_trackbar];
		
		if(visual_name == "input")
			vis = frame.clone();
		else if(visual_name == "frame_diff")
			vis = frame_diff.clone();
		else if(visual_name == "motion_hist")
		{			
			
			for(int i=0; i< motion_history.cols; i++)
    			{
       				for(int j=0; j< motion_history.rows ; j++)
      				{ 
					float a = motion_history.at<float>(j,i);
//					cout << (a-timestamp-MHI_DURATION)/MHI_DURATION << endl;				
					if((a-timestamp-MHI_DURATION)/MHI_DURATION <= -5)
						vis1.at<uchar>(j,i) = 0;
					else
						vis1.at<uchar>(j,i) = (a-timestamp-MHI_DURATION)/MHI_DURATION; 
				}
			}
			
			cvtColor(vis1,vis,COLOR_GRAY2BGR);
		}
		else if(visual_name == "grad_orient")
		{
			for(int i=0; i< hsv.cols; i++)
    			{
       				for(int j=0; j< hsv.rows ; j++)
      				{
					float a = (mg_orient.at<float>(j,i))/2;
					
					hsv.at<Vec3b>(j,i)[0] = a;
					float b = (mg_mask.at<uchar>(j,i))*255;
					hsv.at<Vec3b>(j,i)[2] = b;
				}
			}
			cvtColor(hsv,vis,COLOR_HSV2BGR); 					
		}

		for(unsigned int h = 0; h < seg_bounds.size(); h++)
            	{
                	Rect rec = seg_bounds[h];
                	if(rec.area() > 5000 && rec.area() < 70000)
			{
                	    	rectangle(vis, rec,Scalar(0,0,255),3);
				silh_roi = motion_mask(rec);
				orient_roi = mg_orient(rec);
				mask_roi = mg_mask(rec);
				mhi_roi = motion_history(rec);
				if(norm(silh_roi, NORM_L2, noArray()) > rec.area()*0.5)
				{ 
					double angle = calcGlobalOrientation(orient_roi, mask_roi, mhi_roi,timestamp, MHI_DURATION);
//					cout << rec.width << endl;
					draw_motion_comp(vis, rec.x, rec.y, rec.width, rec.height,angle,vis);
				}			
			}            	
		}	
		imshow("Motion_tracking",vis);		
		prev_frame = frame.clone();
//		waitKey(30);
		output.write (vis);
        	if(waitKey(30) >= 0) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break; 
		}
//		MHI_DURATION = 1000.0*clock()/CLOCKS_PER_SEC- timestamp;
	}	
	
//	waitKey(30);
	return 0;
	
}


void draw_motion_comp(Mat& img, int x_coordinate, int y_coordinate, int width, int height, double angle,Mat& result)
{	
//	rectangle(img,Point(x_coordinate,y_coordinate), Point(x_coordinate+width,y_coordinate+width), Scalar(255,0,0), 1, 8, 0);
	int r,cx,cy;
	if(height/2 <= width/2)
		r = height/2;
	else
		r = width/2;
	cx = x_coordinate + width/2;
	cy = y_coordinate + height/2;
	angle = angle*M_PI/180;
	circle(img, Point(cx,cy), r, Scalar(255,0,0),1, 8, 0);
	line(img, Point(cx,cy), Point(int(cx+cos(angle)*r), int(cy+sin(angle)*r)), Scalar(255,0,0), 1, 8, 0);
	result = img.clone();
}
