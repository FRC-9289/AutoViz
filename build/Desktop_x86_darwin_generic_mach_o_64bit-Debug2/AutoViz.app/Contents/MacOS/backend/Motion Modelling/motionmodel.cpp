#include "Kinematics.h"
#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
    if (argc != 11) {
        std::cerr << "Usage: ./motionmodel width length LB_angle LF_angle RB_angle RF_angle LB_vel LF_vel RB_vel RF_vel\n";
        return 1;
    }

    double width = std::stod(argv[1]);
    double length = std::stod(argv[2]);

    double LB_angle = std::stod(argv[3]);
    double LF_angle = std::stod(argv[4]);
    double RB_angle = std::stod(argv[5]);
    double RF_angle = std::stod(argv[6]);

    double LB_velocity = std::stod(argv[7]);
    double LF_velocity = std::stod(argv[8]);
    double RB_velocity = std::stod(argv[9]);
    double RF_velocity = std::stod(argv[10]);

    Kinematics kinematics(width, length);

    std::array<Kinematics::ModuleData, 4> modules = {
        Kinematics::ModuleData{LB_angle, LB_velocity},
        Kinematics::ModuleData{LF_angle, LF_velocity},
        Kinematics::ModuleData{RB_angle, RB_velocity},
        Kinematics::ModuleData{RF_angle, RF_velocity}
    };

    Kinematics::Output output = kinematics.estimate(modules);

    std::cout << output.v_x << " " << output.v_y << " " << output.omega;

    return 0;
}
