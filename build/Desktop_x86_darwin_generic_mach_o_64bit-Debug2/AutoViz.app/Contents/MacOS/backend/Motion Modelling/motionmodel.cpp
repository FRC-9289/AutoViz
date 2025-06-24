#include "Kinematics.h"
#include <iostream>
#include <cmath>

int main(int argc, char *argv[]) {

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

    Kinematics kinematics(width, length); // Width = 0.6m, Length = 0.8m

    std::array<Kinematics::ModuleData, 4> modules = {
        Kinematics::ModuleData{/*angle*/ LB_angle, /*velocity*/ LB_velocity},    // LB
        Kinematics::ModuleData{/*angle*/ LF_angle, /*velocity*/ LF_velocity},    // LF
        Kinematics::ModuleData{/*angle*/ RB_angle, /*velocity*/ RB_velocity},    // RB
        Kinematics::ModuleData{/*angle*/ RF_angle, /*velocity*/ RF_velocity}     // RF
    };

    Kinematics::Output output = kinematics.estimate(modules);
    std::cout << "" << output.v_x << " " << output.v_y << " " << output.omega << std::endl;
}