#include <iostream>
 
// Point cloud library
#include <pcl/point_cloud.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_types.h>
#include <pcl/filters/passthrough.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <vtkSmartPointer.h>
 
// Opencv
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
 
using namespace cv;
 
int a = 22;
int b = 12;
int c=  10;
 
// PCL Visualizer to view the pointcloud
 pcl::visualization::PCLVisualizer viewer ("Get the view here");

int main (int argc, char** argv)
{
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZ>);
 
   pcl::io::loadPCDFile (argv[1], *cloud);
 
    pcl::copyPointCloud( *cloud,*cloud_filtered);
 
    float i ;
    float j;
    float k;
 
    cv::namedWindow( "picture");
 
    cvCreateTrackbar("X_limit", "picture", &a, 30, NULL);
    cvCreateTrackbar("Y_limit", "picture", &b, 30, NULL);
    cvCreateTrackbar("Z_limit", "picture", &c, 30, NULL);
 
    char last_c = 0;

//	pcl::visualization::PCLVisualizer viewer ("picture");
        
        
	viewer.setBackgroundColor (0.0, 0.0, 0.5);//set backgroung according to the color of points

	pcl::PassThrough<pcl::PointXYZ> pass;
        

	while (!viewer.wasStopped ())
             {

               
		pcl::copyPointCloud(*cloud_filtered, *cloud);
 
        
 
        i = 0.1*((float)a);
        j = 0.1*((float)b);
        k = 0.1*((float)c);
 
        
 
//        cout << "i = " << i << " j = " << j << " k = " << k << endl;
 
        
        pass.setInputCloud (cloud);
	pass.setFilterFieldName ("y");
        pass.setFilterLimits (-j, j);
        pass.filter (*cloud);
 
        pass.setInputCloud (cloud);
        pass.setFilterFieldName ("x");
        pass.setFilterLimits (-i, i);
        pass.filter (*cloud);

	pass.setInputCloud (cloud);
        pass.setFilterFieldName ("z");
        pass.setFilterLimits (-k,k);
        pass.filter (*cloud);

	viewer.addPointCloud (cloud, "scene_cloud");
	viewer.spinOnce ();
	viewer.removePointCloud("scene_cloud");
	waitKey(10);
	}

return 0;
}

