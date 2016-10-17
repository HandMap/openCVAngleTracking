#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <math.h>

namespace cv
{
    using std::vector;
}

#define PI 3.14159265

int main(int, char**)
{
    cv::VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    cv::Mat image_HSV;
    cv::Mat image_Color1;
    cv::Mat image_Color2;
    cv::Moments moments_color1;
    cv::Moments moments_color2;

    cv::vector<cv::vector<cv::Point> > contours_1;
    cv::vector<cv::vector<cv::Point> > contours_2;
    cv::vector<cv::Vec4i> hierarchy_1;
    cv::vector<cv::Vec4i> hierarchy_2;

    cv::Scalar color_1 = cv::Scalar( 255, 0, 0 );
    cv::Scalar color_2 = cv::Scalar( 0, 0, 255 );
    cv::Mat image_frame;
    cv::namedWindow("Angles",0);

    std::string point1;
    std::string point2;


    cv::Point center_1;
    cv::Point center_2;
    for(;;)
    {
        cap >> image_frame; // get a new frame from camera
        if (image_frame.empty()) break;

        // Generates HSV Matrix
        cv::cvtColor(image_frame,   // Input image
                     image_HSV, // output image in HSV
                     CV_BGR2HSV); // constant refering to color space transformation

        //filtering image for colors
        cv::inRange(image_HSV, //input image to be filtered
                    cv::Scalar(110,50,50), //min thresshold value
                    cv::Scalar(130,255,255), // max threshold value
                    image_Color1); //output image

        cv::inRange(image_HSV, //input image to be filtered
                    cv::Scalar(0,150,150), //min thresshold value
                    cv::Scalar(10,255,255), // max threshold value
                    image_Color2); //output image

        /// Find contours
        findContours( image_Color1, // input image
                      contours_1, // vector to save contours
                      hierarchy_1,
                      CV_RETR_TREE,
                      CV_CHAIN_APPROX_SIMPLE,
                      cv::Point(0, 0) );

        findContours( image_Color2, // input image
                      contours_2, // vector to save contours
                      hierarchy_2,
                      CV_RETR_TREE,
                      CV_CHAIN_APPROX_SIMPLE,
                      cv::Point(0, 0) );


        // Get the moments
        cv::vector<cv::Moments> mu_1(contours_1.size() ); // initialize a vector of moments called mu, vector size the number of contours
        for( int i = 0; i < contours_1.size(); i++ )
        { mu_1[i] = moments( contours_1[i], false ); }

        cv::vector<cv::Moments> mu_2(contours_2.size() ); // initialize a vector of moments called mu, vector size the number of contours
        for( int i = 0; i < contours_2.size(); i++ )
        { mu_2[i] = moments( contours_2[i], false ); }

        ///  Get the mass centers:
        cv::vector<cv::Point2f> mc_1(contours_1.size()); //vector to store all the center points of the contours.
        for( int i = 0; i < contours_1.size(); i++ )
        { mc_1[i] = cv::Point2f( mu_1[i].m10/mu_1[i].m00 , mu_1[i].m01/mu_1[i].m00 );}

        cv::vector<cv::Point2f> mc_2(contours_2.size()); //vector to store all the center points of the contours.
        for( int i = 0; i < contours_2.size(); i++ )
        { mc_2[i] = cv::Point2f( mu_2[i].m10/mu_2[i].m00 , mu_2[i].m01/mu_2[i].m00 );}

        /// Draw contours
        for( int i = 0; i< contours_1.size(); i++ )
        {
            if  (mu_1[i].m00>1000){
                center_1=mc_1[i];

                drawContours( image_frame, contours_1, i, color_1, 2, 8, hierarchy_1, 0, cv::Point() );
                circle( image_frame, mc_1[i], 4, color_1, -1, 8, 0 );

                //std::cout<< "red pen: " <<mc[i] << '\n';
            }
        }
        for( int i = 0; i< contours_2.size(); i++ )
        {
            if  (mu_2[i].m00>1000){
                center_2=mc_2[i];
                drawContours( image_frame, contours_2, i, color_2, 2, 8, hierarchy_2, 0, cv::Point() );
                circle( image_frame, center_2, 4, color_2, -1, 8, 0 );

                line(image_frame,center_2,center_1,color_1,4,8,0);
                cv::putText(image_frame, (std::to_string(int(atan ( (abs(center_1.y-center_2.y)*1.0 /(center_1.x-center_2.x)*1.0))*(180.0/PI))) + "Deg"), (center_2 + cv::Point(-100,0)),CV_FONT_HERSHEY_DUPLEX,1,cv::Scalar(0,255,0),1,8);
                line(image_frame,center_2,cv::Point(image_frame.cols,center_2.y),color_2,4,8,0);
            }
        }
        std::cout <<"Point 1" <<center_1 <<",  Point 2"<< center_2
                  << "  angle :" <<atan ( (abs(center_1.y-center_2.y)*1.0 /(center_1.x-center_2.x)*1.0))*(180.0/PI)
                  <<'\n';
        cv::imshow("Angles", image_frame);

        if(cv::waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructoratan
    return 0;
}