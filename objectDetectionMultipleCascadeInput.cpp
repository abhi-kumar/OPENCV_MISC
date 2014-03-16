/*
 * @file objectDetectionMultipleCascadeInput.cpp
 * @author Abhishek Kumar Annamraju ( based on the classic facedetect.cpp in samples/c )
 * @brief This code provides faster object detection.Also multiple cascade(xml) files can be used to detect objects,with a benefit that no two 		objects are detected twice.Ever object detected in an image goes through a two stage testing.
 This code provides faster object detection.

 Also for the first time multiple cascade files are used to detect objects,with a benefit that no two objects
 are detected twice.

 Every object detected in an image goes through a two stage testing.

 The number of checkcascades are set to 1.It is desirable not to change this number.

 USAGE: ./objectDetectionMultipleCascadeInput IMAGE.EXTENTION checkcas.xml cas1.xml cas2.xml cas3.xml cas4.xml ..........upto n number of main cascade xml files

 ckeckcas.xml is the one trained with smallest size parameters and the rest are the main cascades
 
*/


void help()
{
	cout << endl << "USAGE: ./detect_object_multiple_cascade_xml IMAGE.EXTENTION checkcas.xml cas1.xml cas2.xml cas3.xml cas4.xml ..........upto n number of main cascade xml files" << endl;
	cout << endl << "ckeckcas.xml is the one trained with smallest size parameters and the rest are the main cascades" << endl;
}   



#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <iterator>


using namespace std;
using namespace cv;


class Objects     //main class
{
	public:	    //variables kept public but precaution taken all over the code
	
	Mat image_input;          //main input image
	Mat image_main_result;    //the final result
	Mat storage;              //introduced to stop detection of same object more than once

	CascadeClassifier cascade;    //the main cascade classifier
	CascadeClassifier checkcascade;        //a test classifier,object detected by both main and test is stated as object

	int num;

	void getimage(Mat src) //getting the input image
    {              
 
        if(! src.data )                             
        {
            cout <<  "src not filled" << endl ;
        }
 
        else
        {
            image_input = src.clone();
			storage = src.clone();              //initialising storage
			image_main_result = src.clone();    //initialising result
			
            cout << "got image" <<endl;
        }
    }

	
	void cascade_load(string cascade_string)            //loading the main cascade
	{
		cascade.load(cascade_string);

		if( !cascade.load(cascade_string) )
    	{
        	cout << endl << "Could not load classifier cascade" << endl;
        	
    	}
		else
		{
			
			cout << "cascade : " << cascade_string << " loaded" << endl;
		}

	}


	void checkcascade_load(string checkcascade_string)               //loading the test/check cascade
	{
		checkcascade.load(checkcascade_string);

		if( !checkcascade.load(checkcascade_string) )
    	{
        	cout << endl << "Could not load classifier checkcascade" << endl;
        	
    	}
		else
		{
			cout<< "checkcascade : " << checkcascade_string << " loaded" << endl;
		}
	}

	
	void display_input()             // function to display input
	{
		namedWindow("display_input");
		imshow("display_input",image_input);
		waitKey(0);
	}

	void display_output()            //function to display output
	{
		
		if(!image_main_result.empty() )
        {
			namedWindow("display_output");
			imshow("display_output",image_main_result);
			waitKey(0);
		}
	}

	void setnum()                      
	{
		num = 0;
	}


	void findObjects()                 //main function
	{
    	int i = 0;
    	
		Mat img = storage.clone();
		//for region of interest.If a object is detected(after testing) by one classifier,then it will not be available for other one		

		Mat temp;                    
		
		if(img.empty() )
        {
			cout << endl << "detect not successful" << endl;
		}
		int cen_x;                         
		int cen_y;
    	vector<Rect> Objects;
    	const static Scalar colors[] =  { CV_RGB(0,0,255),CV_RGB(0,255,0),CV_RGB(255,0,0),CV_RGB(255,255,0),CV_RGB(255,0,255),
					CV_RGB(0,255,255),CV_RGB(255,255,255),CV_RGB(128,0,0),CV_RGB(0,128,0),CV_RGB(0,0,128),
					CV_RGB(128,128,128),CV_RGB(0,0,0)};                   
        	
    	Mat gray; 

    	cvtColor( img, gray, CV_BGR2GRAY );

		Mat resize_image(cvRound (img.rows), cvRound(img.cols), CV_8UC1 );		

    	resize( gray, resize_image, resize_image.size(), 0, 0, INTER_LINEAR );
    	equalizeHist( resize_image, resize_image );

    	
    	cascade.detectMultiScale( resize_image, Objects,1.1,2,0,Size(10,10));                 //detection using main classifier 
        	
		
	for( vector<Rect>::const_iterator main = Objects.begin(); main != Objects.end(); main++, i++ )
    	{
       		Mat resize_image_reg_of_interest;
        	vector<Rect> nestedObjects;
        	Point center;
        	Scalar color = colors[i%8];
        	int radius;

	        	
			//getting points for bouding a rectangle over the object detected by main
			int x0 = cvRound(main->x);                              
			int y0 = cvRound(main->y);
			int x1 = cvRound((main->x + main->width-1));
			int y1 = cvRound((main->y + main->height-1));

		
			
        	if( checkcascade.empty() )
            	continue;
        	resize_image_reg_of_interest = resize_image(*main);
        	checkcascade.detectMultiScale( resize_image_reg_of_interest, nestedObjects,1.1,2,0,Size(30,30));
            	
		//testing the detected object by main using checkcascade
        	for( vector<Rect>::const_iterator sub = nestedObjects.begin(); sub != nestedObjects.end(); sub++ )      
        	{
           		//getting center points for bouding a circle over the object detected by checkcascade
			center.x = cvRound((main->x + sub->x + sub->width*0.5));        
			cen_x = center.x;
			center.y = cvRound((main->y + sub->y + sub->height*0.5));
			cen_y = center.y;
			//if centre of bounding circle is inside the rectangle boundary over a threshold the the object is certified
			if(cen_x>(x0+15) && cen_x<(x1-15) && cen_y>(y0+15) && cen_y<(y1-15))         
			{
                								
				rectangle( image_main_result, cvPoint(x0,y0),
                    	   		cvPoint(x1,y1),
                     	  		color, 3, 8, 0);               //detecting boundary rectangle over the final result
					

					
					//masking the detected object to detect second object if present

					Rect region_of_interest = Rect(x0, y0, x1-x0, y1-y0);
					temp = storage(region_of_interest);
					temp = Scalar(255,255,255);

					num = num+1;     //num if number of Objects detected
				
				}
			}	
				
		}		
				
                    	   
	if(image_main_result.empty() )
    {
		cout << endl << "result storage not successful" << endl;
	}			
			
    }      
			
};

    

		    	
int main( int argc, const char** argv )
{
		
	double t = 0;
    	t = (double)cvGetTickCount();              //starting timer
    	Mat image1 = imread(argv[1],1);
	Mat image;
	resize(image1,image,Size(300,150),0,0,INTER_LINEAR);        //resizing image to get best experimental results
	Objects detectObjects;                      //creating a object

	
	string checkcas = argv[2];

	detectObjects.getimage(image);           //get the image
	detectObjects.setnum();                  //set number of objects detected as 0
	detectObjects.checkcascade_load(checkcas);      //load the test cascade
	
	//Applying various cascades for a finer search.
	if(argc > 3)
	{
		for(int i = 3;i<argc;i++)
		{
			string cas = argv[i];			
			detectObjects.cascade_load(cas);            
			detectObjects.findObjects();
		}
	}
	else
	{
		help();		
		cout << endl << "Please provide atleast one main cascade xml file" << endl;
	}
	
	t = (double)cvGetTickCount() - t;       //stopping the timer
    
	if(detectObjects.num!=0)
	{
		cout << endl << detectObjects.num << " Objects got detected in = " << t/((double)cvGetTickFrequency()*1000.) << " ms" << endl << endl;
	}
	else
	{
		cout << endl << "Objects not found" << endl;
	}
	
	detectObjects.display_output();          //displaying the final result
	
	return 0;
}




	
