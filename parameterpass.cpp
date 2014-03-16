/*
* @file roadsegment.cpp
* @author Abhishek Kumar Annamraju 
* @brief This code hepls to understand how can an image parameters be extracted and used for various funcions
         without affecting the image

USAGE: ./parameterpass IMAGE.EXTENTION


*/


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv.h>
#include <highgui.h>
#include <iostream>
#include<stdio.h>
using namespace cv;
using namespace std;

class store              
{
	public:             //Keep it public so that it can be used in other classes
		Mat image;
		int blue;
		int green;
		int red;


	
	public:	
	void getimage(Mat src) //storing image into a class variable named image
	{				
				
		if(! src.data )                              
    	{
        	cout <<  "src not filled" << endl ;
       	}
		
		else
		{
			image = src.clone();
			cout << "got image" <<endl;
		}
	}

	void getdata(int bb,int gg,int rr)//storing data into class variables
	{
		blue =bb;
		green= gg;
		red=rr;
	}

	void showimage()//Display stored image using a class function
	{		
		if(! image.data )                              
    	{
        	cout <<  "Image not passed into this function" << std::endl ;
        	
    	}

		else
		{
		
			namedWindow( "Display window", WINDOW_AUTOSIZE );
    		imshow( "Display window", image );
			cout << "image ready to get displayed" << endl;
		}	
	}
	
};	//first class ends here;

class grayimage   //creating a class where i need to manipulate images and data obtained from another class
{
			
	private:
		Mat gray_image;
		int btrans,gtrans,rtrans;

	public:
		void setgray(Mat src) //converting the image to grayscale
		{
			if(! src.data )                              
	    	{
	        	cout <<  "src in second class not filled" << endl ;
	       	}
			
			else
			{
				cvtColor(src, gray_image, CV_BGR2GRAY );
				cout << "image conversion done" <<endl;
			}
			
		}

		void transferdata(int b,int g,int r)  //creating a transfer function
		{
			btrans = b;
			gtrans = g;
			rtrans = r;
		}		


		void showdata()
		{
			cout << "values of b,g,r before gray scale coversion are" << btrans << "," << gtrans << "," << rtrans << "," <<endl; 
		}

		void displaygray()
		{
			if(! gray_image.data )                              
    		{
        		cout <<  "Image not passed into this function" << std::endl ;
        	
    		}
	
			else
			{
			
				namedWindow( "gray image", WINDOW_AUTOSIZE );
   		 		imshow( "gray image", gray_image );
				cout << "gray image ready to get displayed" << endl;
			}	
		}

};	

//main starts here

int main( int argc, char** argv )
{
    if( argc != 2)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }

    Mat img;
    img = imread(argv[1], CV_LOAD_IMAGE_COLOR);    

    if(! img.data )                              
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

	unsigned char *input = (unsigned char*)(img.data);
	int i,j,r,g,b;
    for(int i = 0;i < img.rows;i++)
	{
	    for(int j = 0;j < img.cols;j++)
		{
	            b = input[img.step * j + i ] ;
	            g = input[img.step * j + i + 1];
	            r = input[img.step * j + i + 2];
            }
    }

	store image1; //object to store image created
	image1.getimage(img);//image from main stored in object
	image1.getdata(b,g,r);//data from main stored in object
	image1.showimage();//displaying the stored image 

	grayimage image2;   //creating a manipulator object
	image2.setgray(image1.image); //fetches image from the first object that was stored and converts it into grayscale 
	image2.transferdata(image1.blue,image1.green,image1.red);//fetches data from the first object that was stored 

	//This data transferred to the new object can be manupulated anytime later
	
	image2.showdata();
	image2.displaygray();          

    waitKey(0);                                         
    return 0;
}
