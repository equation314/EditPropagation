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

cv::Mat EditPropagation::applyEdits(const DoubleArray& e)
{
    cv::Mat tmp_img, output_img;
    if (Config::apply_edits_color_space == "lab")
        cv::cvtColor(m_orig_img, tmp_img, CV_BGR2Lab);
    else if (Config::apply_edits_color_space == "hsv")
        cv::cvtColor(m_orig_img, tmp_img, CV_BGR2HSV);
    else
        assert(false);

    double ave = 0;
    for (auto i : e)
        ave += i;
    ave /= e.size();
    cout << ave << endl;
    // ave = 0.5;

    for (int i = 0; i < m_h; i++)
        for (int j = 0; j < m_w; j++)
        {
            cv::Vec3b& color = tmp_img.at<cv::Vec3b>(i, j);

            int delta = (e[i * m_w + j] - ave) * Config::apply_edits_coefficient;
            int new_value = color[Config::apply_edits_color_channel] + delta;

            if (Config::apply_edits_color_space == "hsv" && Config::apply_edits_color_channel == 0)
            {
                if (new_value > 180)
                    new_value -= 180;
                if (new_value < 0)
                    new_value += 180;
            }
            color[Config::apply_edits_color_channel] = min(max(new_value, 0), 255);
        }

    if (Config::apply_edits_color_space == "lab")
        cv::cvtColor(tmp_img, output_img, CV_Lab2BGR);
    else if (Config::apply_edits_color_space == "hsv")
        cv::cvtColor(tmp_img, output_img, CV_HSV2BGR);
    else
        assert(false);
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

cv::Mat EditPropagation::array2image(const DoubleArray& array, int height, int width, bool normalize)
{
    double min, max;
    if (normalize)
    {
        min = *min_element(array.begin(), array.end());
        max = *max_element(array.begin(), array.end());
        cout << min << ' ' << max << endl;
    }
    else
        min = 0, max = 1;
    double scalar = max - min;
    cv::Mat img(height, width, CV_8UC1);
    for (int i = 0, t = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            img.at<uchar>(i, j) = (array[t++] - min) / scalar * 255;
    return img;
}
