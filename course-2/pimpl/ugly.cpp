#include "bad.h"
#include "ugly.h"

struct SplineImpl {
    std::vector<double> x;
    std::vector<double> y;
    double a;
    double b;
};

Spline::Spline(const std::vector<double>& x, const std::vector<double>& y, double a, double b)
    : impl_(std::make_unique<SplineImpl>(SplineImpl{x, y, a, b})) {
}

double Spline::Interpolate(double x) {
    double* y = new double[impl_->x.size()];
    mySplineSnd(impl_->x.data(), impl_->y.data(), impl_->x.size(), impl_->a, impl_->b, y);
    double ans;
    mySplintCube(impl_->x.data(), impl_->y.data(), y, impl_->x.size(), x, &ans);
    delete[] y;
    return ans;
}