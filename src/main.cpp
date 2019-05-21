// // also please use the c++ headers, not the old c-ones !

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;


// int main()
// {

//     Mat image = imread("../pics/reddit.jpg", 0); // grayscale

//     imshow("start",image);

//     Mat fimage;

//     image.convertTo(fimage, CV_32FC1, 1.0 / 255); // also scale to [0..1] range (not mandatory)

//     // you're lucky with lena (512x512), for odd sizes, you have to
//     // pad it to pow2 size, or use dft() instead:
//     Mat dimage;
//     dct(fimage, dimage);
//     // process dimage,

//     imshow("inprogress", dimage);

//     // then same way back:
//     dct(dimage, dimage, DCT_INVERSE);
//     dimage.convertTo(dimage, CV_8U, 255); // maybe scale back to [0..255] range (depending on your processing)

//     imshow("result", image);
//     waitKey();
// }
//
//

int main()
{

    Mat source = imread("../pics/ba_1.png", 3);

    Mat bgr[3];
    split(source, bgr);

    imshow("blue", bgr[0]);
    imshow("red",  bgr[1]);
    int patch_size = 40;

    vector<Mat> patches;

    int w_steps, h_steps;
    w_steps = int(source.cols / patch_size);
    h_steps = int(source.rows / patch_size);

    for (int w_step=0; w_step < w_steps - 1; ++w_step)
    {
        for (int h_step=0; h_step < h_steps - 1; ++h_step)
        {
            Rect myROI(
                w_step * patch_size,
                h_step * patch_size,
                patch_size,
                patch_size
            );

            Mat cropped[3];

            Mat croppedRef_b(bgr[0], myROI);
            croppedRef_b.copyTo(cropped[0]);
            Mat croppedRef_r(bgr[2], myROI);
            croppedRef_r.copyTo(cropped[1]);
            Mat croppedRef_g(bgr[1], myROI); // green
            croppedRef_r.copyTo(cropped[2]);

            Mat cropped_dct[3];

            cropped[0].convertTo(cropped_dct[0], CV_32FC1, 1.0 / 255);
            cropped[1].convertTo(cropped_dct[1], CV_32FC1, 1.0 / 255);
            cropped[2].convertTo(cropped_dct[2], CV_32FC1, 1.0 / 255); // green

            dct(cropped_dct[0], cropped_dct[0]);
            dct(cropped_dct[1], cropped_dct[1]);
            dct(cropped_dct[2], cropped_dct[2]); // green

            Mat rb_dct;

            add(cropped_dct[0], cropped_dct[1], rb_dct);
            rb_dct /= 2;
            rb_dct -= cropped_dct[2];

            patches.push_back(rb_dct);
        }
    }

    Mat added_dct;

    add(patches[0], patches[1], added_dct);

    for (int i = 2; i < patches.size(); ++i)
    {
        added_dct += patches[i];
    }

    double min, max;
    minMaxLoc(added_dct, &min, &max);

    added_dct /= max;

    Mat added_dct_resized;
    resize(added_dct, added_dct_resized, Size(512, 512), 0, 0, CV_INTER_CUBIC);

    imshow("patches_sum", added_dct_resized);

    waitKey();
}