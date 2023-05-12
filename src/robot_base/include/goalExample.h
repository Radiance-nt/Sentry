#include <string>

#define OTHER_LENGTH 40
#define COLOR BLUE

#include <unordered_map>
#include <vector>


std::vector<std::unordered_map<std::string, float>> get_goals(std::string &color) {

    std::unordered_map<std::string, float> origin = {{"pos_x",     0.0},
                                                     {"pos_y",     0.0},
                                                     {"ori_z",     0.0},
                                                     {"ori_w",     0.0},
                                                     {"wait_time", 0.0},
    };
    std::unordered_map<std::string, float> forward, backward, health, enemy, left_forward, left_backward;
    if (color == "blue") {
        // 4.16 13:49
//        forward = {{"pos_x",     4.577600},
//                   {"pos_y",     -2.0981},
//                   {"ori_z",     0.41938},
//                   {"ori_w",     0.90780},
//                   {"wait_time", 40.0},
//        };
        forward = {{"pos_x",     3.2443253900},
                   {"pos_y",     -2.425381},
                   {"ori_z",     0.233138},
                   {"ori_w",     0.9724},
                   {"wait_time", 40.0},
        };
        backward = {{"pos_x",     1.57354},
                    {"pos_y",     -1.9670},
                    {"ori_z",     0.35405},
                    {"ori_w",     0.935224},
                    {"wait_time", 40.0},
        };
        // 4.14 22:39
//        health = {{"pos_x",     -2.9518},
//                  {"pos_y",     3.155},
//                  {"ori_z",     -0.279744},
//                  {"ori_w",     0.9600},
//                  {"wait_time", 40.0},
//        };
        health = {{"pos_x",     -3.2018},
                  {"pos_y",     3.305},
                  {"ori_z",     -0.279744},
                  {"ori_w",     0.9600},
                  {"wait_time", 40.0},
        };
        enemy = {{"pos_x",     7.9254},
                 {"pos_y",     1.1770},
                 {"ori_z",     -0.9105},
                 {"ori_w",     0.395224},
                 {"wait_time", 40.0},
        };
        left_forward = {{"pos_x",     3.9254},
                        {"pos_y",     3.810},
                        {"ori_z",     -0.5205},
                        {"ori_w",     0.8499},
                        {"wait_time", 40.0},
        };
    } else {
        // 4.16 13:49
//        forward = {{"pos_x",     1.1051154},
//                   {"pos_y",     3.130869},
//                   {"ori_z",     -0.909032},
//                   {"ori_w",     0.41650},
//                   {"wait_time", 40.0},
//        };
        forward = {{"pos_x",     2.381154},
                   {"pos_y",     3.430869},
                   {"ori_z",     -0.909032},
                   {"ori_w",     0.41650},
                   {"wait_time", 40.0},
        };
        backward = {{"pos_x",     3.9305},
                    {"pos_y",     2.90441},
                    {"ori_z",     -0.955073},
                    {"ori_w",     0.285256},
                    {"wait_time", 40.0},
        };
        // 4.14 22:39
//        health = {{"pos_x",     8.41342},
//                  {"pos_y",     -2.0846},
//                  {"ori_z",     3.0},
//                  {"ori_w",     0.98117},
//                  {"wait_time", 0.14257},
//        };
        health = {{"pos_x",     8.71342},
                  {"pos_y",     -2.3846},
                  {"ori_z",     3.0},
                  {"ori_w",     0.98117},
                  {"wait_time", 0.14257},
        };
    }

    std::vector<std::unordered_map<std::string, float>> goal_vector = {origin, health,
                                                                       backward, forward, enemy, left_forward
    };
    return goal_vector;
}

struct Goal {
    int seq = 0;
    int index = 0;
    float pos_x = 0;
    float pos_y = 0;
    float ori_z = 0;
    float ori_w = 0;
    int wait_time = 0;
};
