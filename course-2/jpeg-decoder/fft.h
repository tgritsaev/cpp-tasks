#pragma once

#include <cstddef>
#include <vector>
#include <glog/logging.h>
#include <fftw3.h>
// #include </usr/local/include/fftw3.h>

class DctCalculator {
public:
    // input and output are width by width matrices, first row, then
    // the second row.
    DctCalculator(size_t width, std::vector<double>* input, std::vector<double>* output);

    void Rebuild(std::vector<double>* input, std::vector<double>* output);

    ~DctCalculator() {
        fftw_destroy_plan(plan_);
    }

    void Inverse();

private:
    static constexpr double kSqrt2 = 1.41421356237;
    size_t width_;
    fftw_plan plan_;
    std::vector<double>* input_;
    std::vector<double>* output_;
};
