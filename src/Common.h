#ifndef COMMON_H
#define COMMON_H

#include <opencv/cv.hpp>

#include "Config.h"

const int DIM = 5;

struct FeatureVec
{
    cv::Vec3f f;
    cv::Vec2f x;

    FeatureVec(const cv::Vec3f& color, const cv::Vec2f& pos)
        : f(color), x(pos) {}

    double affinity_with(const FeatureVec& b) const
    {
        return exp(-cv::norm(f - b.f, cv::NORM_L2SQR) / Config::omega_a) * exp(-cv::norm(x - b.x, cv::NORM_L2SQR) / Config::omega_s);
    }
};

typedef double VectorK[DIM];

struct Point
{
    Point(bool is_user_edits = false)
        : is_user_edits(is_user_edits) {}
    Point(const double* _x, bool is_user_edits = false)
        : is_user_edits(is_user_edits)
    {
        for (int i = 0; i < DIM; i++)
            x[i] = _x[i];
    }

    VectorK x;
    bool is_user_edits;
};

typedef std::vector<double> DoubleArray;
typedef std::vector<FeatureVec> FeatureVecArray;

#endif // COMMON_H
