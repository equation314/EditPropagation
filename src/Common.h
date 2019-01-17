#ifndef COMMON_H
#define COMMON_H

#include <opencv/cv.hpp>

#include "Config.h"

const double EPS = 1e-9;
const int DIM = 5;

typedef double VectorK[DIM];

struct FeatureVec
{
    cv::Vec3f f;
    cv::Vec2f x;

    FeatureVec(const VectorK vec)
        : f(vec[0], vec[1], vec[2]), x(vec[3], vec[4]) {}
    FeatureVec(const cv::Vec3f& color, const cv::Vec2f& pos)
        : f(color / Config::omega_c), x(pos / Config::omega_p) {}

    double affinity_with(const FeatureVec& b) const
    {
        return exp(-cv::norm(f - b.f, cv::NORM_L2SQR)) * exp(-cv::norm(x - b.x, cv::NORM_L2SQR));
        // return exp(-cv::norm(f - b.f, cv::NORM_L2SQR) / Config::omega_a) * exp(-cv::norm(x - b.x, cv::NORM_L2SQR) / Config::omega_s);
    }
};

struct Point
{
    Point() {}
    Point(const double* _x)
    {
        for (int i = 0; i < DIM; i++)
            x[i] = _x[i];
    }

    VectorK x;

    bool operator<(const Point& p) const
    {
        for (int i = 0; i < DIM; i++)
        {
            if ((x[i] - p.x[i]) < -EPS)
                return true;
            if ((x[i] - p.x[i]) > EPS)
                return false;
        }
        return false;
    }

    bool operator==(const Point& p) const
    {
        for (int i = 0; i < DIM; i++)
        {
            if (abs(x[i] - p.x[i]) > EPS)
                return false;
        }
        return true;
    }
};

class PixelPoint : public Point
{
public:
    PixelPoint(const double* _x, int index, bool is_user_edits)
        : Point(_x), m_index(index), m_is_user_edits(is_user_edits) {}

    int index() const { return m_index; }
    bool isUserEdits() const { return m_is_user_edits; }

private:
    int m_index;
    bool m_is_user_edits;
};

typedef std::vector<double> DoubleArray;
typedef std::vector<FeatureVec> FeatureVecArray;

#endif // COMMON_H
