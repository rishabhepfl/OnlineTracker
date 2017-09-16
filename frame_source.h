#ifndef FRAME_SOURCE_H_
#define FRAME_SOURCE_H_

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
            if (!video_.isOpened()) LOG(FATAL) << "Could not open webcam";
            // Read some dummy frames for webcam
            cv::Mat frame;
            for (int i = 0; i < 5; ++i) video_.read(frame);
        } else {
            LOG(INFO) << "Loading video at " << filename;
            video_.open(filename);
            if (!video_.isOpened()) LOG(FATAL) << "Could not open " << filename;
        }

    }
    ~FrameSource() {}
    bool GetFrame(cv::Mat &);
};

#endif
