#include "opencv_helper.h"

#include <cstdio>


#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

int i = 0;

struct Data {
    cv::Mat nbc_mask;
    cv::Mat other_mask;
    cv::Mat other_mask2;
};

void* init_data() {
    Data* data = new Data;
    data->nbc_mask = cv::imread("nbcBetterMask.png", cv::IMREAD_UNCHANGED);
    data->other_mask = cv::imread("nbcOtherMask.png", cv::IMREAD_UNCHANGED);
    data->other_mask2 = cv::imread("nbcOther2Mask.png", cv::IMREAD_UNCHANGED);

    return (void*) data;
}

float helper(unsigned char* rgb_data, void* info) {
    Data* data = (Data*) info;

    i += 1;

    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;

    cv::Mat image(720, 1280, CV_8UC3, rgb_data);

    cv::Mat grey_image;
    cv::cvtColor(image, grey_image, CV_BGR2GRAY);

    cv::Mat grad_x, grad_y, grad;
    cv::Mat abs_grad_x, abs_grad_y;

    /// Gradient X
    cv::Sobel( grey_image, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT );
    cv::convertScaleAbs( grad_x, abs_grad_x );

    /// Gradient Y
    cv::Sobel( grey_image, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
    cv::convertScaleAbs( grad_y, abs_grad_y );

    /// Total Gradient (approximate)
    cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

    cv::Mat threshold_grad;
    cv::adaptiveThreshold(grad, threshold_grad, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, -3);

    float num_incorrect = 0;
    float num = 0;

    float num_incorrect_other = 0;
    float num_other = 0;


    float num_incorrect_other2 = 0;
    float num_other2 = 0;

    for (int y = 0; y < 720; y++) {
        for (int x = 0; x < 1280; x++) {
            int a = data->nbc_mask.at<cv::Vec4b>(y,x)[3];
            int value = data->nbc_mask.at<cv::Vec4b>(y,x)[0];

            int current_value = threshold_grad.at<unsigned char>(y, x);
            if (a == 255) {
                num += 1;
                if (value != current_value) {
                    num_incorrect += 1;
                }
            } 


            int other_a = data->other_mask.at<cv::Vec4b>(y,x)[3];
            int other_value = data->other_mask.at<cv::Vec4b>(y,x)[0];

            if (other_a == 255) {
                num_other += 1;
                if (other_value != current_value) {
                    num_incorrect_other += 1;
                }
            } 

            int other2_a = data->other_mask2.at<cv::Vec4b>(y,x)[3];
            int other2_value = data->other_mask2.at<cv::Vec4b>(y,x)[0];

            if (other2_a == 255) {
                num_other2 += 1;
                if (other2_value != current_value) {
                    num_incorrect_other2 += 1;
                }
            } 
        }
    }

    if (i % 100 == 0) {
        printf("I am helping\n");

        cv::imwrite("hope.png", threshold_grad );    
    }

    return std::min(
        std::min(num_incorrect / num, num_incorrect_other / num_other),
        num_incorrect_other2 / num_other2);

}