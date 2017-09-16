#include <memory>

#include <opencv2/opencv.hpp>
#include <glog/logging.h>

#include "frame_source.h"
#include "state.h"
#include "predictor.h"

cv::Point2d top_left;
bool is_second = false;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN) {
        if (!is_second) {
            top_left = cv::Point2d(x, y);
            is_second = true;
        } else {
            std::vector<BoundingBox> * ref_bounding_boxes = (std::vector<BoundingBox> *)userdata;
            BoundingBox bounding_box;
            bounding_box.tl = top_left;
            bounding_box.br = cv::Point2d(x, y);
            ref_bounding_boxes->push_back(bounding_box);
            is_second = false;
        }
    }
}

void AddInitialBoundingBoxes(const cv::Mat & first_frame, State & state) {
    std::vector<BoundingBox> bounding_boxes;

    const std::string window_name = "First Frame";
    cv::namedWindow(window_name, CV_WINDOW_AUTOSIZE);
    cv::setMouseCallback(window_name, CallBackFunc, &bounding_boxes);
    while (true) {
        cv::imshow(window_name, first_frame);
        for (const auto bb: bounding_boxes) {
            cv::rectangle(first_frame, bb.tl, bb.br, cv::Scalar(255, 0, 0));
        }

        int key = cv::waitKey(5) & 0xff;
        if (key == 13) break;
    }

    cv::destroyWindow(window_name);

    int track_id = 0;
    for (const auto & bb: bounding_boxes) {
        state.AddToCurrentState(bb.tl, bb.br, track_id);
        track_id += 1;
    }
}

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

    // Load first frame of the video to set the intial state
    cv::Mat first_frame;
    if (!frame_source->GetFrame(first_frame)) LOG(FATAL) << "Could not load first frame";
    State state;
    AddInitialBoundingBoxes(first_frame, state);

    // Setup the predictor
    Predictor predictor(0.5);

    cv::Mat frame;
    cv::Mat last_frame;
    first_frame.copyTo(last_frame);

    while (frame_source->GetFrame(frame)) {
        // Get the current state (vector of bounding boxes);
        auto cur_state = state.GetState();
        state.Clear();
        for (const auto & bb_info: cur_state) {
            auto track_id = bb_info.first;
            auto bb = bb_info.second;
            auto bb_predicted = predictor.Predict(last_frame, frame, bb);
            state.AddToCurrentState(bb_predicted.tl, bb_predicted.br, track_id);

            // Draw the state on the current frame
            const auto color = (track_id == 1) ? cv::Scalar(255, 0, 0) : cv::Scalar(0, 255, 0);
            cv::rectangle(frame, bb_predicted.tl, bb_predicted.br, color);
        }
        frame.copyTo(last_frame);

        cv::imshow("Frame", frame);
        int key = cv::waitKey(5) & 0xff;

        if (key == 27) break;
    }
}
