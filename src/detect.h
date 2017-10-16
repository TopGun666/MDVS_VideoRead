#ifndef DETECT_H
#define DETECT_H
#define DETECT_SHOW

#include "opencv2/opencv.hpp"
#include <mutex>

namespace HeYuDetector {

enum JudeRes {
    Invaild = 0,
    Left = 1,
    Right = 2
};

void DetectRed(cv::Mat img);
void DetectGreen(cv::Mat img);
JudeRes Judge(cv::Mat img);


#ifdef DETECT_SHOW
    class detectRes {
    public:
        friend void DetectRed(cv::Mat img);
        friend void DetectGreen(cv::Mat img);
        static cv::Mat getRes();
        static void initial(cv::Mat img);

    private:
        static cv::Mat res;
        static std::mutex ResMutex;
    };
#endif // DETECT_SHOW

}

#endif // DETECT_H
