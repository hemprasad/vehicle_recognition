#include "opencv2/opencv.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

int main(int argc, char** argv)
{
    int key = 0, crop = 0;
    VideoCapture cap;
    std::string input;
    std::vector<int> frames_to_save;
    if (argc < 2)
    {
        input = "../media_src/the_video.mp4";
        crop = 1;
    }
    else
    {
        input = argv[1];
    }
    
    cap = VideoCapture(input);
    Size S = Size((int) cap.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                  (int) cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    
    std::cout << "Opening file " << input << std::endl;
    std::cout << "Input frame resolution: Width=" << S.width << "  Height=" << S.height
         << " of nr#: " << cap.get(CV_CAP_PROP_FRAME_COUNT) << std::endl;
    
    if(!cap.isOpened())  // check if we succeeded
	return -1;
    
    BackgroundSubtractorMOG2 bg_sub(500,32,false);
    
    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(7,7),Point(3,3));
    namedWindow("Origin",1);
    namedWindow("Background mask",1);
    namedWindow("Opening",1);
    for(;;)
    {
        Mat frame, bg_mask, frame_masked, op_mask;
        cap >> frame; // get a new frame from camera
        
        if (crop)
        	frame = frame.colRange(160,1120);



        /// Separate the image in 3 places ( B, G and R )
		vector<Mat> bgr_planes, bgr_eq;
		split(frame, bgr_planes );
		/*
		equalizeHist(bgr_planes[0],bgr_planes[0]);
		equalizeHist(bgr_planes[1],bgr_planes[1]);
		equalizeHist(bgr_planes[2],bgr_planes[2]);

		merge(bgr_planes, frame);
		*/
		/// Establish the number of bins
		  int histSize = 256;

		  /// Set the ranges ( for B,G,R) )
		  float range[] = { 0, 256 } ;
		  const float* histRange = { range };

		  bool uniform = true; bool accumulate = false;

		  Mat b_hist, g_hist, r_hist;

		  /// Compute the histograms:
		  calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
		  calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
		  calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

		  // Draw the histograms for B, G and R
		  int hist_w = 512; int hist_h = 400;
		  int bin_w = cvRound( (double) hist_w/histSize);

		  Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

		  /// Normalize the result to [ 0, histImage.rows ]
		  normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
		  normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
		  normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

		  /// Draw for each channel
		  for( int i = 1; i < histSize; i++ )
		  {
		      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
		                       Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
		                       Scalar( 255, 0, 0), 2, 8, 0  );
		      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
		                       Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
		                       Scalar( 0, 255, 0), 2, 8, 0  );
		      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
		                       Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
		                       Scalar( 0, 0, 255), 2, 8, 0  );
		  }

		  /// Display
		  namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE );
		  imshow("calcHist Demo", histImage );

        //cvtColor(frame, frame, CV_BGR2GRAY);
        GaussianBlur(frame, frame, Size(5,5), 1.5, 1.5);
        
        bg_sub(frame,bg_mask);
        
        morphologyEx(bg_mask, op_mask, CV_MOP_CLOSE, element, Point(3,3), 3);
        frame.copyTo(frame_masked,op_mask);
        
        std::stringstream ss;
        rectangle(frame,cv::Point(100,5),cv::Point(210,25),cv::Scalar(255,255,255,-1),CV_FILLED);
        ss << "frame " << cap.get(CV_CAP_PROP_POS_FRAMES);
        std::string fns = ss.str();

        putText(frame,fns.c_str(),cv::Point(105,20),FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(0,0,0));
        //bgsub1.getBackgroundImage(bg1);
        
        imshow("Origin", frame);
        imshow("Background mask", bg_mask);
        imshow("Opening", frame_masked);
        
        key = waitKey(10);
        
        if (key == 112)
        {
            while(key < 0 || key == 112)
            {
                key = waitKey(10);
            }
        }        
        else if(key >= 0)
        {
            break;
        }
        //if(waitKey(30) == )
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
