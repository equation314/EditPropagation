#ifndef EDIT_PROPAGATION_H
#define EDIT_PROPAGATION_H

#include <cmath>
#include <vector>
#include <opencv/cv.hpp>

struct FeatureVec
{
    cv::Vec3f f;
    cv::Vec2f x;

    static constexpr double OMEGA_A = 0.5;
    static constexpr double OMEGA_S = 0.5;

    FeatureVec(const cv::Vec3f& color, const cv::Vec2i& pos)
        : f(color / OMEGA_A), x(pos / OMEGA_S) {}

    double get_z(const FeatureVec& b) const
    {
        return exp(-cv::norm(f - b.f, cv::NORM_L2SQR)) * exp(-cv::norm(x - b.x, cv::NORM_L2SQR));
    }
};

class EditPropagation
{
public:
    EditPropagation(const char* origImage, const char* userInput);
    virtual ~EditPropagation();

    virtual void work() = 0;

private:
    cv::Mat m_orig_img;
    cv::Mat m_user_input;
    int m_w, m_h, m_n;

    std::vector<FeatureVec> m_fvs;
    std::vector<double> m_user_w;
    std::vector<double> m_user_g;

    void m_init_feature_vectors();
    void m_init_user_w_g();
};

#endif // EDIT_PROPAGATION_H
