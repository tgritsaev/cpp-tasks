#include "fft.h"
// #include <fftw3.h>

DctCalculator::DctCalculator(size_t width, std::vector<double> *input,
                             std::vector<double> *output) {
    if (width * width != input->size() || width * width != output->size()) {
        throw std::invalid_argument(
            "expression '' width * width != input->size() || width * width != output->size() '' is "
            "true");
    }

    width_ = width;
    input_ = input;
    output_ = output;
    plan_ = fftw_plan_r2r_2d(width_, width_, input->data(), output->data(), FFTW_REDFT01,
                             FFTW_REDFT01, FFTW_ESTIMATE);
}

void DctCalculator::Rebuild(std::vector<double> *input, std::vector<double> *output) {
    input_ = input;
    output_ = output;
}

void DctCalculator::Inverse() {
    for (size_t i = 0; i < width_; ++i) {
        (*input_)[i] *= kSqrt2;
        (*input_)[i * width_] *= kSqrt2;
    }
    double mul_num = width_ * width_ / 4.;
    for (size_t i = 0; i < width_ * width_; ++i) {
        (*input_)[i] *= mul_num;
    }
    fftw_execute(plan_);
    double del_num = 1. / (4 * width_ * width_);
    for (size_t i = 0; i < width_ * width_; ++i) {
        (*output_)[i] *= del_num;
    }
}
