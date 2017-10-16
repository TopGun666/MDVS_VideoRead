#include "detect.h"
#include <thread>
#include <vector>
#include <unistd.h>


namespace HeYuDetector {

#ifdef DETECT_SHOW
cv::Mat detectRes::res;
std::mutex detectRes::ResMutex;

cv::Mat detectRes::getRes() {
    std::unique_lock<std::mutex> lock(ResMutex);
    return res.clone();
}

void detectRes::initial(cv::Mat img) {
    ResMutex.lock();
    res = img.clone();
    ResMutex.unlock();
}

#endif // DETECT_SHOW

using namespace cv;

int rX = -1, gX = -1;

void DetectColor(cv::Mat& img, const int iLowH, const int iHighH, const int iLowS, const int iHighS, const int iLowV, const int iHighV,
                 std::vector<Rect> &rects /* out */, size_t &id /* out */) {
    std::vector<Mat> hsvSplit;
    split(img, hsvSplit);
    equalizeHist(hsvSplit[2],hsvSplit[2]);
    merge(hsvSplit,img);
    Mat imgThresholded;
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
    Mat element2=getStructuringElement(MORPH_RECT,Size(15,15));
    inRange(img, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

    morphologyEx(imgThresholded, imgThresholded, MORPH_OPEN, element);

    morphologyEx(imgThresholded, imgThresholded, MORPH_CLOSE, element);

    Mat dstimage;
    erode(imgThresholded,dstimage,element2);

    const double whRatio = 3;
    cv::Mat src;
    src = dstimage;

    blur(src, src, Size(3, 3));

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours(src, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    double maxarea = 0;
    rects.clear();
    rects.reserve(contours.size());
    for (size_t i = 0; i < contours.size(); i++)
    {
        double tmparea = fabs(contourArea(contours[i]));
        Rect aRect =boundingRect(contours[i]);
        rects.push_back(aRect);
        if ((aRect.width / aRect.height)< whRatio)
            continue;

        if (tmparea>maxarea)
        {
            maxarea = tmparea;
            id = i;
            continue;
        }
    }
}

void DetectRed(cv::Mat img) {
    rX = -1;
    const int iLowH = 156;
    const int iHighH = 180;

    const int iLowS = 43;
    const int iHighS = 255;

    const int iLowV = 46;
    const int iHighV = 255;

    std::vector<Rect> rects;
    size_t id = 0;
    DetectColor(img, iLowH, iHighH, iLowS, iHighS, iLowV, iHighV, rects, id);
    if(!rects.empty()) {
        rX = rects[id].x;

#ifdef DETECT_SHOW
        detectRes::ResMutex.lock();
        rectangle(detectRes::res, rects[id], Scalar(0, 0, 255));
        detectRes::ResMutex.unlock();
#endif // DETECT_SHOW

    }
}

void DetectGreen(cv::Mat img) {
    gX = -1;
    const int iLowH = 35;
    const int iHighH = 77;

    const int iLowS = 43;
    const int iHighS = 255;

    const int iLowV = 46;
    const int iHighV = 255;

    std::vector<Rect> rects;
    size_t id = 0;
    DetectColor(img, iLowH, iHighH, iLowS, iHighS, iLowV, iHighV, rects, id);

    if(!rects.empty()) {
        gX = rects[id].x;

#ifdef DETECT_SHOW
    detectRes::ResMutex.lock();
    rectangle(detectRes::res, rects[id], Scalar(0, 255, 0));
    detectRes::ResMutex.unlock();
#endif // DETECT_SHOW

    }
}

JudeRes Judge(Mat img) {
    cv::Mat imgHSV;
    cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);

#ifdef DETECT_SHOW
    detectRes::initial(img);
#endif // DETECT_SHOW

    std::thread detectRed(&DetectRed, imgHSV);
    std::thread detectGreen(&DetectGreen, imgHSV);
    usleep(1);
    detectRed.join();
    detectGreen.join();
#ifdef DE_BUG
    std::cout << "rX = " << rX << ", gX = " << gX << std::endl;
#endif //DE_BUG
    if(rX >= 0 && gX >= 0) {

        if(rX < gX)
            return JudeRes::Right;

        if(rX > gX)
            return JudeRes::Left;
    }

    return JudeRes::Invaild;
}

}
