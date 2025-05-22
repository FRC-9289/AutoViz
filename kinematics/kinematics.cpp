#include <iostream>
#include "kinematics.h"

int main() {
    // Example robot params
    double width = 0.6;   // meters
    double length = 0.8;  // meters
    double mass = 50.0;   // kg
    double friction = 0.8;
    double dt = 0.02;     // 20 ms control loop

    kinematics kin(width, length, mass, friction, dt);

    // Simulated wheel states (angle in radians, velocity in m/s, previous velocity)
    std::array<kinematics::ModuleData, 4> modules = {
        kinematics::ModuleData{0.0, 1.0, Eigen::Vector2d(0, 0)},
        kinematics::ModuleData{0.0, 1.0, Eigen::Vector2d(0, 0)},
        kinematics::ModuleData{0.0, 1.0, Eigen::Vector2d(0, 0)},
        kinematics::ModuleData{0.0, 1.0, Eigen::Vector2d(0, 0)},
    };

    auto output = kin.estimate(modules);

    std::cout << "Estimated linear velocity: (" 
              << output.linearVelocity.x() << ", " << output.linearVelocity.y() << ") m/s\n";
    std::cout << "Estimated angular velocity: " << output.omega << " rad/s\n";

    for (int i = 0; i < 4; ++i) {
        std::cout << "Module " << i << " slipping: " << (output.isSlipping[i] ? "YES" : "NO") << "\n";
    }

    return 0;
}
