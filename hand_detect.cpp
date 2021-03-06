////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <curl/curl.h>

using namespace cv;
using namespace std;

//Global variables
 RNG rng(12345);

 int main( int argc, char** argv )
 {
    VideoCapture cap(0); //capture the video from webcam

    if ( !cap.isOpened() )  // if not success, exit program
    {
         cout << "Cannot open the web cam" << endl;
         return -1;
    }

    namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

  int iLowH = 22;
 int iHighH = 38;

  int iLowS = 150; 
 int iHighS = 255;

  int iLowV = 60;
 int iHighV = 255;

  //Create trackbars in "Control" window
 createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
 createTrackbar("HighH", "Control", &iHighH, 179);

  createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
 createTrackbar("HighS", "Control", &iHighS, 255);

  createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
 createTrackbar("HighV", "Control", &iHighV, 255);

  int iLastX = -1; 
 int iLastY = -1;

  //Capture a temporary image from the camera
 Mat imgTmp;
 cap.read(imgTmp); 

  //Create a black image with the size as the camera output
 Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );;
 

    while (true)
    {
        Mat imgOriginal;

        bool bSuccess = cap.read(imgOriginal); // read a new frame from video



         if (!bSuccess) //if not success, break loop
        {
             cout << "Cannot read a frame from video stream" << endl;
             break;
        }
//CV_CAP_PROP_FRAME_WIDTH Width of the frames in the video stream.
//CV_CAP_PROP_FRAME_HEIGHT Height of the frames in the video stream.
  //  cout<<cap.get(CV_CAP_PROP_FRAME_WIDTH)<<" - "<<cap.get(CV_CAP_PROP_FRAME_HEIGHT)<<"\n";
    Mat imgHSV,flippedImage;
    flip(imgOriginal,flippedImage,1);
   cvtColor(flippedImage, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
 
  Mat imgThresholded;
    
   inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
      
  //morphological opening (removes small objects from the foreground)
  erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
  dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

   //morphological closing (removes small holes from the foreground)
  dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
  erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );


//Drawing contours here!
  Mat canny_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

    /// Detect edges using canny
  Canny( imgThresholded, canny_output, 100, 200, 3 );
  /// Find contours
  findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

  /// Draw contours
  Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
  Mat blackWindow = Mat::zeros( canny_output.size(), CV_8UC3 );

  rectangle( drawing, Point(0,0), Point( 320,360), Scalar( 255,255,255 ), -1, 8 );
  rectangle( drawing, Point(640,0), Point( 960,360), Scalar( 255,255,255 ), -1, 8 );
  rectangle( drawing, Point(320,360), Point(640,720), Scalar( 255,255,255 ), -1, 8 );
  rectangle( drawing, Point(960,360), Point( 1280,720), Scalar( 255,255,255 ), -1, 8 );

  putText(drawing, "C", Point(90,160), FONT_HERSHEY_SIMPLEX, 3, Scalar(51,255,51), 4);
  putText(drawing, "D", Point(410,160), FONT_HERSHEY_SIMPLEX, 3, Scalar(51,255,51), 4);
  putText(drawing, "E", Point(730,160), FONT_HERSHEY_SIMPLEX, 3, Scalar(51,255,51), 4);
  putText(drawing, "F", Point(1050,160), FONT_HERSHEY_SIMPLEX, 3, Scalar(51,255,51), 4);
  putText(drawing, "G", Point(90,520), FONT_HERSHEY_SIMPLEX, 3, Scalar(51,255,51), 4);
  putText(drawing, "A", Point(410,520), FONT_HERSHEY_SIMPLEX, 3, Scalar(51,255,51), 4);
  putText(drawing, "B", Point(730,520), FONT_HERSHEY_SIMPLEX, 3, Scalar(51,255,51), 4);
  putText(drawing, "C", Point(1050,520), FONT_HERSHEY_SIMPLEX, 3, Scalar(51,255,51), 4);

  putText(drawing, "Virtual Music System using Hand Gestures!", Point(120,620), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(255,0,255), 4);

  for( int i = 0; i< contours.size(); i++ )
     {
  //     Scalar color = Scalar(  rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
      Scalar color = Scalar(255,0,0);      
       drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
       drawContours( blackWindow, contours, i, color, 2, 8, hierarchy, 0, Point() );
     }

   //Calculate the moments of the thresholded image
  Moments oMoments = moments(imgThresholded);

   double dM01 = oMoments.m01;
  double dM10 = oMoments.m10;
  double dArea = oMoments.m00;

   // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
  if (dArea > 10000)
  {
   //calculate the center position of the ball
   int posX = dM10 / dArea;
   int posY = dM01 / dArea;        
        
   if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
   {
    //Draw a red line from the previous point to the current point
    //line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0,0,255), 2);
   }

    iLastX = posX;
   iLastY = posY;
   //cout << "\n" << iLastX << "," << iLastY;
   CURL* c;
   c = curl_easy_init();
      
   if((iLastX >= 0 && iLastX <= 320) && (iLastY >= 0 && iLastY <= 360 ))
      curl_easy_setopt( c, CURLOPT_URL, "http://localhost:8000/music?id=1" );
   else if((iLastX > 320 && iLastX <= 640) && (iLastY >= 0 && iLastY <= 360 ))
      curl_easy_setopt( c, CURLOPT_URL, "http://localhost:8000/music?id=2" );
   else if((iLastX > 640 && iLastX <= 960) && (iLastY >= 0 && iLastY <= 360 ))
      curl_easy_setopt( c, CURLOPT_URL, "http://localhost:8000/music?id=3" );
   else if((iLastX > 960 && iLastX <= 1280) && (iLastY >= 0 && iLastY <= 360 ))
      curl_easy_setopt( c, CURLOPT_URL, "http://localhost:8000/music?id=4" );
   else if((iLastX >= 0 && iLastX <= 320) && (iLastY > 360 && iLastY <= 720 )) 
      curl_easy_setopt( c, CURLOPT_URL, "http://localhost:8000/music?id=5" );
   else if((iLastX > 320  && iLastX <= 640) && (iLastY > 360 && iLastY <= 720 ))
      curl_easy_setopt( c, CURLOPT_URL, "http://localhost:8000/music?id=6" );
   else if((iLastX > 640 && iLastX <= 960) && (iLastY > 360 && iLastY <= 720 ))
      curl_easy_setopt( c, CURLOPT_URL, "http://localhost:8000/music?id=7" );
   else if((iLastX > 960 && iLastX <= 1280) && (iLastY > 360 && iLastY <= 720 ))
      curl_easy_setopt( c, CURLOPT_URL, "http://localhost:8000/music?id=8" );
    else
      curl_easy_setopt( c, CURLOPT_URL, "http://localhost:8000/music?id=0" );
    
     curl_easy_perform( c );
     curl_easy_cleanup( c );
  }
    circle(drawing, Point(iLastX, iLastY), 5, Scalar(0,0,255), 1, 8, 0);
    circle(blackWindow, Point(iLastX, iLastY), 5, Scalar(0,0,255), 1, 8, 0);
    //rectangle(drawing,Point(0,0),Point(320,360),CV_RGB(255,255,255),CV_FILLED,5,8);

   imshow("Music window", drawing); //show the thresholded image
   imshow("Thresholded Image", blackWindow);
   imgOriginal = imgOriginal + imgLines;
  imshow("Original", imgOriginal); //show the original image

        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
       {
            cout << "esc key is pressed by user" << endl;
            break; 
       }
    }

   return 0;
}
