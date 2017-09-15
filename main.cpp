#include <iostream>
#include <memory>

#include <opencv2/opencv.hpp>
#include <glog/logging.h>

#include "frame_source.h"

int main(int argc, char ** argv) {
    google::SetUsageMessage("Usage message");
    google::InitGoogleLogging("OnlineTracker");
    google::SetStderrLogging(google::INFO);
    google::ParseCommandLineFlags(&argc, &argv, true);

    std::unique_ptr<FrameSource> frame_source;
    if (argc > 1) {
        frame_source = std::unique_ptr<FrameSource>(new FrameSource(std::string(argv[1])));
    } else {
        frame_source = std::unique_ptr<FrameSource>(new FrameSource(""));
    }

    cv::Mat frame;
    while (frame_source->GetFrame(frame)) {
        cv::imshow("Frame", frame);
        int key = cv::waitKey(5) & 0xff;

        if (key == 27) break;
    }
}
