#ifndef PREDICTOR_H_
#define PREDICTOR_H_

#include <glog/logging.h>

#include "state.h"

class Predictor {
private:
    double search_space_;
public:
    Predictor(double search_space) :search_space_(search_space){}

    BoundingBox Predict(const cv::Mat & source_image,
                        const cv::Mat & search_image,
                        const BoundingBox & source_bb);
};

#endif
