#include "frame_source.h"

bool FrameSource::GetFrame(cv::Mat & frame) {
    if (!video_.read(frame)) return false;
    return true;
}
