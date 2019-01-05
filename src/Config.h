#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <json/value.h>

struct Config
{
    static int column_samples;

    static double omega_a;
    static double omega_s;

    static int none_user_input_gray;

    static bool use_log_lab_color;
    static bool use_brute_force;

    static Json::Value toJson();
    static void load(const Json::Value& config);
    static void save(const std::string& file);
    static void loadFrom(const std::string& file);

private:
    Config();
};

#endif // CONFIG_H
