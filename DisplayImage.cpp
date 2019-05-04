#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    VideoCapture cap(argv[1]);
    Mat frame, grayedFrame, smoothedFrame, edgedFrame, abs_dst;
    int64 e1, e2;
    double laneCenter;
    int steeringAngle;
    
    double t;
    if (!cap.isOpened()) {
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

    cap >> frame;
    if (frame.empty())
        exit;

    cv::Mat mask = cv::Mat::zeros(frame.size(), 0);
    cv::Mat dstImage = cv::Mat::zeros(frame.size(), frame.type());

    cv::Point pts[3] = {
        cv::Point(0.05 * frame.cols, frame.rows),
        cv::Point(int(frame.cols / 2), int(frame.rows / 2+0.075*frame.rows)),
        cv::Point(frame.cols - 0.05 * frame.cols, frame.rows)
    };

    cv::fillConvexPoly(mask, pts, 3, cv::Scalar(255));
    

    while (1) {
        laneCenter = 0;
        cap >> frame;
        if (frame.empty())
            break;
        Mat res, src;
        cvtColor(frame, grayedFrame, COLOR_RGB2GRAY);
        GaussianBlur(grayedFrame, smoothedFrame, Size(5, 5), 0);
        Canny(smoothedFrame, edgedFrame, 60, 150, 3);
        convertScaleAbs(edgedFrame, abs_dst);
        
        
        bitwise_and(abs_dst, mask, res);

        vector<Vec4i> lines;
        HoughLinesP(res, lines, 1, CV_PI/180, 50, 50, 10 );
        for (size_t i = 0; i < lines.size(); i++)
        {
            Vec4i l = lines[i];
            line( frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, LINE_AA);
            laneCenter += l[0];
        }
        steeringAngle = (frame.cols/2 - laneCenter/lines.size())/10;
        imshow("Frame", frame);

        char c = (char)waitKey(25);
        if (c == 27)
            break;
    }

    cap.release();
    destroyAllWindows();

    return 0;
}