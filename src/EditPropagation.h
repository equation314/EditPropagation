#ifndef EDIT_PROPAGATION_H
#define EDIT_PROPAGATION_H

#include <cmath>
#include <vector>
#include <opencv/cv.hpp>

#include "Common.h"
#include "Config.h"

class EditPropagation
{
public:
    EditPropagation(const char* origImage, const char* userInput, bool isVideo = false);
    virtual ~EditPropagation();

    virtual FrameArray getEditedImage(int outputId) = 0;

    FrameArray applyEdits(const DoubleArray& e);

    static void saveFrameArray(const char* name, const FrameArray& frames);

    static FrameArray array2image(const DoubleArray& array, int height, int width, bool normalize = true);

private:
    cv::Mat m_orig_img;
    cv::Mat m_user_input;
    cv::VideoCapture m_video;

    void m_init_feature_vectors();
    void m_init_user_w_g();

protected:
    int m_h, m_w, m_n, m_frame_count;
    bool m_is_video;

    FeatureVecArray m_fv;
    DoubleArray m_user_w;
    DoubleArray m_user_g;
};

#endif // EDIT_PROPAGATION_H
