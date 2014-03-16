/*
* @file roadsegment.cpp
* @author Abhishek Kumar Annamraju ( based on the watershed segmentation algorithm)
* @brief This code provides faster road detection.


USAGE: ./roadsegment IMAGE.EXTENTION 


*/





#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <stdio.h>


using namespace std;
using namespace cv;

class findroad{	//class that separates out roads from images
private:
    Mat path;
public:
    void setpath(Mat& image)
    {
        image.convertTo(path, CV_32S);
    }

    Mat getroad(Mat &image) //major working function attribute of the class
    {
watershed(image, path); //using watershed segmenter
        path.convertTo(path,CV_8U);
        return path;
    }
};

int main( int argc, const char** argv )
{
    Mat image1 = imread(argv[1],1);
Mat image;
resize(image1,image,Size(500,500),0,0,INTER_LINEAR);
    Mat gray;
    cvtColor(image, gray, CV_BGR2GRAY);
    threshold(gray, gray, 100, 255, THRESH_BINARY); //threasholding the grayscale image
    double t = 0;
t = (double)cvGetTickCount(); //setting up timer
    Mat Erode;
    erode(gray,Erode,Mat(),Point(2,2),7); //eroding image 7 times.Experimentally best output given by 7 time erode
    Mat Dilate;
    dilate(gray,Dilate,Mat(),Point(2,2),7); //dilating the image
    threshold(Dilate,Dilate,1, 50,THRESH_BINARY_INV);
    Mat path_trace(gray.size(),CV_8U,Scalar(0));
    path_trace= Erode+Dilate; //morphological addition
    findroad road; //creating an object for our class
    road.setpath(path_trace); //preparing the input
    namedWindow("founded road");
namedWindow("input image");
    Mat road_found = road.getroad(image);
    road_found.convertTo(road_found,CV_8U);
    imshow("founded road", road_found);
imshow("input image",image);
    t = (double)cvGetTickCount() - t; //time of detection
    printf( "road got detected in = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );
cout << endl << "cheers" << endl;
imwrite("ROAD.jpg",image);
imwrite( "ROAD_DETECTED.jpg",road_found);
    waitKey(0);
return 0;
}
