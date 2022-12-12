#include "blur_plugin.h"

QString BlurPlugin::name()
{
    return "Blur";
}

void BlurPlugin::edit(const cv::Mat &input, cv::Mat &output)
{
    cv::blur(input, output, cv::Size(8, 8));
}
