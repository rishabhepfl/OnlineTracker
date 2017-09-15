#include <glog/logging.h>
#include <opencv2/opencv.hpp>

class FrameSource {
private:
    cv::VideoCapture video_;
public:
    FrameSource(const std::string & filename) {
        if (filename.empty()) {
            LOG(INFO) << "Starting webcam";
            video_.open(0);
        } else {
            LOG(INFO) << "Loading video at " << filename;
            video_.open(filename);
        }
        if (!video_.isOpened()) LOG(FATAL) << "Cannot open video";
    }
    ~FrameSource() {}
    bool GetFrame(cv::Mat &);
};
