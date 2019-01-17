#include <cstdio>
#include <iostream>

#include "Config.h"
#include "EditPropagation.h"

using namespace std;

EditPropagation::EditPropagation(const char* origImage, const char* userInput)
    : m_orig_img(cv::imread(origImage)), m_user_input(cv::imread(userInput)),
      m_h(m_orig_img.rows), m_w(m_orig_img.cols), m_n(m_h * m_w)
{
    assert(m_user_input.rows == m_h);
    assert(m_user_input.cols == m_w);

    m_init_feature_vectors();
    m_init_user_w_g();
}

EditPropagation::~EditPropagation()
{
}

cv::Mat EditPropagation::apply_edits(const DoubleArray& e)
{
    cv::Mat lab_img, output_img;
    cv::cvtColor(m_orig_img, lab_img, CV_BGR2Lab);

    double ave = 0;
    for (auto i : e)
        ave += i;
    ave /= e.size();

    for (int i = 0; i < m_h; i++)
        for (int j = 0; j < m_w; j++)
        {
            cv::Vec3b& lab = lab_img.at<cv::Vec3b>(i, j);

            int delta = (e[i * m_w + j] - ave) * 500;
            int new_light = lab[0] + delta;

            lab[0] = min(max(new_light, 0), 255);
        }

    cv::cvtColor(lab_img, output_img, CV_Lab2BGR);
    return output_img;
}

void EditPropagation::m_init_feature_vectors()
{
    const int DIR[9][2] = {
        {0, 0},
        {-1, -1},
        {-1, 0},
        {-1, 1},
        {0, -1},
        {0, 1},
        {1, -1},
        {1, 0},
        {1, 1},
    };

    cv::Mat lab_img;
    cv::cvtColor(m_orig_img, lab_img, CV_BGR2Lab);

    m_fv.clear();
    for (int i = 0; i < m_h; i++)
        for (int j = 0; j < m_w; j++)
        {
            cv::Vec3f color;
            cv::Vec2f pos(1.0f * i / m_h, 1.0f * j / m_w);

            int s = 0;
            for (int k = 0; k < 9; k++)
            {
                int x = i + DIR[k][0], y = j + DIR[k][0];
                if (0 <= x && x < m_w && 0 <= y && y < m_w)
                {
                    cv::Vec3f lab = lab_img.at<cv::Vec3b>(x, y);
                    lab /= 255;
                    if (Config::use_log_lab_color)
                        color += cv::Vec3f(log(lab[0] + 1e-4), log(lab[1] + 1e-4), log(lab[2] + 1e-4));
                    else
                        color += lab;
                    s++;
                }
            }

            FeatureVec fv(color / s, pos);
            m_fv.push_back(fv);
        }
}

void EditPropagation::m_init_user_w_g()
{
    m_user_w.clear();
    m_user_g.clear();

    for (int i = 0; i < m_h; i++)
        for (int j = 0; j < m_w; j++)
        {
            cv::Vec3b color = m_user_input.at<cv::Vec3b>(i, j);
            double w = color[0] == Config::none_user_input_gray ? 0 : 1;
            double g = color[0] / 255.0;

            m_user_w.push_back(w);
            m_user_g.push_back(g);
        }
}

cv::Mat EditPropagation::array2image(const DoubleArray& array, int height, int width)
{
    double min = *min_element(array.begin(), array.end());
    double max = *max_element(array.begin(), array.end());
    double scalar = max - min;
    cv::Mat img(height, width, CV_8UC1);
    for (int i = 0, t = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            img.at<uchar>(i, j) = (array[t++] - min) / scalar * 255;
    return img;
}
