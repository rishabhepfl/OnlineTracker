#include "predictor.h"

static void AdjustBoundingBoxEdge(double & tlx, double & tly, double & brx, double & bry,
                                  const cv::Mat & image) {
    tlx = std::min(std::max(0.0, tlx), static_cast<double>(image.cols) - 1.0);
    tly = std::min(std::max(0.0, tly), static_cast<double>(image.rows) - 1.0);
    brx = std::min(std::max(0.0, brx), static_cast<double>(image.cols) - 1.0);
    bry = std::min(std::max(0.0, bry), static_cast<double>(image.rows) - 1.0);
}

BoundingBox Predictor::Predict(const cv::Mat & source_image,
                               const cv::Mat & search_image,
                               const BoundingBox & source_bb) {
    const cv::Point2d source_bb_tl = source_bb.tl;
    const cv::Point2d source_bb_br = source_bb.br;

    // Get source template
    const cv::Rect source_rect = cv::Rect(source_bb_tl, source_bb_br);
    const cv::Mat source_template = source_image(source_rect);

    // Get search image
    double source_bb_width = source_bb_br.x - source_bb_tl.x;
    double source_bb_height = source_bb_br.y - source_bb_tl.y;

    double search_space_width = source_bb_width * search_space_;
    double search_space_height = source_bb_height * search_space_;

    double search_space_tlx = source_bb_tl.x - search_space_width;
    double search_space_tly = source_bb_tl.y - search_space_height;
    double search_space_brx = source_bb_br.x + search_space_width;
    double search_space_bry = source_bb_br.y + search_space_height;

    AdjustBoundingBoxEdge(search_space_tlx, search_space_tly, search_space_brx, search_space_bry,
                          source_image);

    const cv::Rect search_rect = cv::Rect(cv::Point2d(search_space_tlx, search_space_tly),
                                          cv::Point2d(search_space_brx, search_space_bry));

    const cv::Mat search_space_image = search_image(search_rect);

    cv::Mat result;
    cv::matchTemplate(search_space_image, source_template, result, cv::TM_SQDIFF_NORMED);

    double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
    cv::Point loc = minLoc;

    double new_tlx = search_space_tlx + loc.x;
    double new_tly = search_space_tly + loc.y;
    double new_brx = search_space_tlx + loc.x + source_bb_width;
    double new_bry = search_space_tly + loc.y + source_bb_height;

    AdjustBoundingBoxEdge(new_tlx, new_tly, new_brx, new_bry, source_image);

    return {cv::Point2d(new_tlx, new_tly), cv::Point2d(new_brx, new_bry)};
}
