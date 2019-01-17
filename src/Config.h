#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <json/value.h>

struct Config
{
    static int column_samples;

    // for AppProp
    static double omega_a;
    static double omega_s;

    // for AppPropKDTree
    // omega_a = omega_c^2
    // omega_p = omega_s^2
    static double omega_c;
    static double omega_p;

    static int none_user_input_gray;

    static bool use_log_lab_color;
    static bool use_brute_force;

    static double kd_tree_eta;

    static Json::Value toJson();
    static void load(const Json::Value& config);
    static void save(const std::string& file);
    static void loadFrom(const std::string& file);

private:
    Config();
};

#endif // CONFIG_H
