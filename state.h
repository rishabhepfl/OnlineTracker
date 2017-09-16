#ifndef STATE_H_
#define STATE_H_

#include <opencv2/opencv.hpp>

struct BoundingBox {
    cv::Point2d tl;
    cv::Point2d br;
};

class State {
private:
    std::map<int, BoundingBox> state_;  // Map: trackId, bounding box
public:
    State() {

    }
    ~State() {

    }
    void AddToCurrentState(cv::Point2d tl, cv::Point2d br, int track_id) {
        BoundingBox bounding_box;
        bounding_box.tl = tl;
        bounding_box.br = br;
        state_[track_id] = bounding_box;
    }

    void Clear() {
        state_.clear();
    }

    std::map<int, BoundingBox> GetState() { return state_;}
};

#endif
