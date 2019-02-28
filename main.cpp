#include <opencv2/opencv.hpp>
#include <cstdint>

int main(int argc, char const *argv[])
{
    const char *path;
    if (argc == 2)
        path = argv[1];
    else
        path = "k.jpg";


    cv::Mat image = cv::imread(path);
    double hist_L[256] = {0};
    double hist_R[256] = {0};
    double hist_G[256] = {0};
    double hist_B[256] = {0};

    if (image.channels() == 3)
    {
        for (int i = 0; i < image.rows; i++)
            for (int j = 0; j < image.cols; j++)
            {
                cv::Vec3b intensity = image.at<cv::Vec3b>(i, j);
                int R = intensity.val[2];
                hist_R[R] += 1;
                int G = intensity.val[1];
                hist_G[G] += 1;
                int B = intensity.val[0];
                hist_B[B] += 1;
            }
    }
    // printf("%d\n", argc);

    cv::Mat1b grey_image;
    cv::cvtColor (image, grey_image, cv::COLOR_BGR2GRAY);

    for (int i = 0; i < grey_image.rows; ++i)
        for (int j = 0; j < grey_image.cols; ++j)
        {
            uint8_t L = grey_image.at<uint8_t>(i, j);
            // printf("%d\n", L);
            hist_L[L] += 1;
        }

    double max = 0;

    for (int i = 0; i < 256; ++i)
    {
        if (hist_L[i] > max)
            max = hist_L[i];
        if (hist_R[i] > max)
            max = hist_R[i];
        if (hist_G[i] > max)
            max = hist_G[i];
        if (hist_B[i] > max)
            max = hist_B[i];
    }

    for (int i = 0; i < 256; ++i)
    {
        hist_L[i] = (512 * hist_L[i] / max) * 2 / 3;
        hist_R[i] = (512 * hist_R[i] / max) * 2 / 3;
        hist_G[i] = (512 * hist_G[i] / max) * 2 / 3;
        hist_B[i] = (512 * hist_B[i] / max) * 2 / 3;
    }
    cv::Mat plot(512, 512, CV_8UC3, cv::Scalar(0, 0, 0));
    for (int i = 0; i < 512; i++)
    {
        int j = i / 2;
        line(plot, cv::Point(i, 512), cv::Point(i, 512 - hist_L[j]), cv::Scalar(127, 127, 127), 1, 8, 0);
        line(plot, cv::Point(i, 512), cv::Point(i, 512 - hist_R[j]), cv::Scalar(  0,   0, 255), 1, 8, 0);
        line(plot, cv::Point(i, 512), cv::Point(i, 512 - hist_G[j]), cv::Scalar(  0, 255,   0), 1, 8, 0);
        line(plot, cv::Point(i, 512), cv::Point(i, 512 - hist_B[j]), cv::Scalar(255,   0,   0), 1, 8, 0);
    }
    cv::namedWindow("Histogram");
    cv::imshow("Histogram", plot);
    cv::waitKey(0);
    return 0;
}