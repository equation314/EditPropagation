#include <cstdio>
#include <iostream>

#include "Config.h"
#include "EditPropagation.h"

using namespace std;

EditPropagation::EditPropagation(const char* origImage, const char* userInput, bool isVideo)
    : m_user_input(cv::imread(userInput)),
      m_h(m_user_input.rows), m_w(m_user_input.cols), m_frame_count(1),
      m_is_video(isVideo)
{
    if (isVideo)
    {
        m_video = cv::VideoCapture(origImage);
        m_frame_count = m_video.get(CV_CAP_PROP_FRAME_COUNT);
        assert(m_video.get(CV_CAP_PROP_FRAME_HEIGHT) == m_h);
        assert(m_video.get(CV_CAP_PROP_FRAME_WIDTH) == m_w);
    }
    else
    {
        m_orig_img = cv::imread(origImage);
        assert(m_orig_img.rows == m_h);
        assert(m_orig_img.cols == m_w);
    }
    m_n = m_w * m_h * m_frame_count;

    m_init_feature_vectors();
    m_init_user_w_g();
}

EditPropagation::~EditPropagation()
{
    m_video.release();
}

FrameArray EditPropagation::applyEdits(const DoubleArray& e)
{
    FrameArray output;

    for (int t = 0; t < m_frame_count; t++)
    {
        cv::Mat frame, tmp_img, output_img;
        if (m_is_video)
            m_video.read(frame);
        else
            frame = m_orig_img;

        if (Config::apply_edits_color_space == "lab")
            cv::cvtColor(frame, tmp_img, CV_BGR2Lab);
        else if (Config::apply_edits_color_space == "hsv")
            cv::cvtColor(frame, tmp_img, CV_BGR2HSV);
        else
            assert(false);

        double ave = 0;
        for (auto i : e)
            ave += i;
        ave /= e.size();
        // ave = 0.5;

        for (int i = 0; i < m_h; i++)
            for (int j = 0; j < m_w; j++)
            {
                cv::Vec3b& color = tmp_img.at<cv::Vec3b>(i, j);

                int delta = (e[t * m_w * m_h + i * m_w + j] - ave) * Config::apply_edits_coefficient;
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

        output.push_back(output_img);
    }

    m_video.set(CV_CAP_PROP_POS_FRAMES, 0);
    return output;
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

    m_fv.clear();
    for (int t = 0; t < m_frame_count; t++)
    {
        cv::Mat frame, lab_img;
        if (m_is_video)
            assert(m_video.read(frame));
        else
            frame = m_orig_img;
        cv::cvtColor(frame, lab_img, CV_BGR2Lab);

        for (int i = 0; i < m_h; i++)
            for (int j = 0; j < m_w; j++)
            {
                cv::Vec3f color;
                cv::Vec2f pos(1.0f * i / m_h, 1.0f * j / m_w);

                int s = 0;
                for (int k = 0; k < 1; k++)
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

                FeatureVec fv(color / s, pos, 1.0 * t / m_frame_count);
                m_fv.push_back(fv);
            }
    }
    m_video.set(CV_CAP_PROP_POS_FRAMES, 0);
}

void EditPropagation::m_init_user_w_g()
{
    m_user_w.clear();
    m_user_g.clear();

    for (int t = 0; t < m_frame_count; t++)
        for (int i = 0; i < m_h; i++)
            for (int j = 0; j < m_w; j++)
            {
                cv::Vec3b color = m_user_input.at<cv::Vec3b>(i, j);
                double w = t == 0 && color[0] != Config::none_user_input_gray ? 1 : 0;
                double g = color[0] / 255.0;

                m_user_w.push_back(w);
                m_user_g.push_back(g);
            }
}

FrameArray EditPropagation::array2image(const DoubleArray& array, int height, int width, bool normalize)
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

    FrameArray output;
    int frame_count = array.size() / height / width;
    for (int t = 0, p = 0; t < frame_count; t++)
    {
        cv::Mat img(height, width, CV_8UC1);
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                img.at<uchar>(i, j) = (array[p++] - min) / scalar * 255;
        cv::cvtColor(img, img, CV_GRAY2BGR);
        output.push_back(img);
    }
    return output;
}

void EditPropagation::saveFrameArray(const char* name, const FrameArray& frames)
{
    string fname = name;
    if (frames.size() == 1)
    {
        cv::imwrite((fname + ".png").c_str(), frames[0]);
        cv::imshow(name, frames[0]);
    }
    else
    {
        int h = frames[0].rows;
        int w = frames[0].cols;
        cv::VideoWriter writer;
        writer.open((fname + ".mp4").c_str(), CV_FOURCC('X', '2', '6', '4'), 24, cv::Size(w, h));
        for (auto img : frames)
            writer << img;
    }
}
