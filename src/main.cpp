#include <opencv2/opencv.hpp>
// #include "opencv2/core/core.hpp"
// #include "opencv2/imgproc/imgproc.hpp"
// #include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <cstdlib>
#include <string>

#include "threshold.h"

using namespace cv;
using namespace std;


int main(int argc, char ** argv){
    if(argc != 2)
    {
        cout << "Usage: ./grey_lvl_threshold <path_to_image>" << endl;
        return -1;
    }
    string filename = argv[1];

    Mat img = imread(filename, 1);

    if(img.channels() != 1)
    {
        cout << "Image has more than one channel, converting to greyscale" << endl;
        cvtColor(img, img, COLOR_RGB2GRAY);
    }

    double sigma_min = 0.1;
    double sigma_max = 1.1;
    double delta = 0.2;
    double k = 2;

    double p_min = 0.2;
    double p;

    double sigma = sigma_min;
    bool spoof = false;

    Mat D, G, G_k;
    Mat thresholded;

    while (sigma <= sigma_max && !spoof){

        cout << "Choosing sigma " << sigma << endl;

        GaussianBlur(img, G, Size(9, 9), sigma);
        GaussianBlur(img, G_k, Size(9, 9), sigma * k);

        D = G - G_k;

        uint T = findThreshold(D);

        threshold(D, thresholded, T, 255, THRESH_BINARY);

        p = sum(thresholded)[0] / (thresholded.cols * thresholded.rows * 255);

        if (p < p_min)
            spoof = true;

        sigma += delta;
    }


    D.convertTo(D, CV_32FC1);
    cout << "D sum = " << sum(D)[0] << endl;
    log(D, D);

    double aspect_ratio = 1.0 * img.rows / img.cols;
    resize(img, img, Size(512, 512 * aspect_ratio), 0, 0, CV_INTER_CUBIC);
    resize(D, D, Size(512, 512 * aspect_ratio), 0, 0, CV_INTER_CUBIC);
    resize(thresholded, thresholded, Size(512, 512 * aspect_ratio), 0, 0, CV_INTER_CUBIC);

    imshow("OpenCV orig", img);
    imshow("D", D);
    imshow("OpenCV thresh", thresholded);

    waitKey(0);
    imwrite(filename + "_orig.png", img);
    imwrite(filename + "_bin.png", thresholded);
}