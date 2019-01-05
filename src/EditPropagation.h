#ifndef EDIT_PROPAGATION_H
#define EDIT_PROPAGATION_H

#include <cmath>
#include <vector>
#include <opencv/cv.hpp>

struct FeatureVec
{
    cv::Vec3f f;
    cv::Vec2f x;

    static constexpr double OMEGA_A = 0.2;
    static constexpr double OMEGA_S = 10;

    FeatureVec(const cv::Vec3f& color, const cv::Vec2f& pos)
        : f(color), x(pos) {}

    double affinity_with(const FeatureVec& b) const
    {
        return exp(-cv::norm(f - b.f, cv::NORM_L2SQR) / OMEGA_A) * exp(-cv::norm(x - b.x, cv::NORM_L2SQR) / OMEGA_S);
    }
};

typedef std::vector<double> DoubleArray;
typedef std::vector<FeatureVec> FeatureVecArray;

class EditPropagation
{
public:
    EditPropagation(const char* origImage, const char* userInput);
    virtual ~EditPropagation();

    virtual void work() = 0;

    static cv::Mat array2image(const DoubleArray& array, int height, int width);

private:
    cv::Mat m_orig_img;
    cv::Mat m_user_input;

    void m_init_feature_vectors();
    void m_init_user_w_g();

protected:
    int m_h, m_w, m_n;

    FeatureVecArray m_fv;
    DoubleArray m_user_w;
    DoubleArray m_user_g;
};

#endif // EDIT_PROPAGATION_H
