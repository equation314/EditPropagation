#include <cstring>
#include <fstream>
#include <iostream>
#include <json/reader.h>
#include <json/writer.h>

#include "Config.h"

int Config::column_samples = 100;

double Config::omega_a = 0.2;

double Config::omega_s = 10;

double Config::omega_c = 0.2;

double Config::omega_p = 0.2;

double Config::apply_edits_coefficient = 50;

int Config::none_user_input_gray = 128;

bool Config::use_log_lab_color = true;

bool Config::use_brute_force = false;

double Config::kd_tree_eta = 0.5;

void Config::load(const Json::Value& config)
{
    column_samples = config["column_samples"].asInt();
    omega_a = config["omega_a"].asDouble();
    omega_s = config["omega_s"].asDouble();
    omega_c = config["omega_c"].asDouble();
    omega_p = config["omega_p"].asDouble();
    apply_edits_coefficient = config["apply_edits_coefficient"].asDouble();
    none_user_input_gray = config["none_user_input_gray"].asInt();
    use_log_lab_color = config["use_log_lab_color"].asBool();
    use_brute_force = config["use_brute_force"].asBool();
    kd_tree_eta = config["kd_tree_eta"].asDouble();
}

Json::Value Config::toJson()
{
    Json::Value config;
    config["column_samples"] = column_samples;
    config["omega_a"] = omega_a;
    config["omega_s"] = omega_s;
    config["omega_c"] = omega_c;
    config["omega_p"] = omega_p;
    config["apply_edits_coefficient"] = apply_edits_coefficient;
    config["use_log_lab_color"] = use_log_lab_color;
    config["use_brute_force"] = use_brute_force;
    config["kd_tree_eta"] = kd_tree_eta;
    return config;
}

void Config::save(const std::string& file)
{
    std::ofstream fout(file.c_str());
    fout << toJson() << std::endl;
    fout.close();
}

void Config::loadFrom(const std::string& file)
{
    Json::Value config;
    std::ifstream fin(file.c_str());
    if (!fin)
    {
        std::cerr << "ERROR: No such config file '" + file + "'" << std::endl;
        return;
    }
    fin >> config;
    fin.close();

    load(config);
}
