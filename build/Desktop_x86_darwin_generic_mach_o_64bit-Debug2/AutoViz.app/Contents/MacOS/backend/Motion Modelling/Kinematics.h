#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <Eigen/Dense>
#include <array>
#include <cmath>
#include <string>
#include <map>
#include <iostream>

class Kinematics {
public:
    struct ModuleData {
        double angle;    // Angle in radians (0 = forward, pi/2 = left)
        double velocity; // Velocity in m/s
    };

    struct Output {
        double v_x;    // Linear velocity in X (forward)
        double v_y;    // Linear velocity in Y (left)
        double omega;  // Angular velocity (rad/s counterclockwise)
    };

    Kinematics(double width, double length)
        : width(width), length(length) {
        modulePositions["LB"] = Eigen::Vector2d(-width / 2.0, -length / 2.0);
        modulePositions["LF"] = Eigen::Vector2d(-width / 2.0,  length / 2.0);
        modulePositions["RB"] = Eigen::Vector2d( width / 2.0, -length / 2.0);
        modulePositions["RF"] = Eigen::Vector2d( width / 2.0,  length / 2.0);
    }

    Output estimate(const std::array<ModuleData, 4>& modules) {
        Eigen::Matrix<double, 8, 3> A;
        Eigen::Matrix<double, 8, 1> b;

        static const std::array<std::string, 4> keys = {"LB", "LF", "RB", "RF"};

        for (int i = 0; i < 4; ++i) {
            const auto& module = modules[i];
            const auto& pos = modulePositions[keys[i]];

            double vx = -module.velocity * std::sin(module.angle);
            double vy = module.velocity * std::cos(module.angle);

            // Row for x-component
            A(2 * i, 0) = 1;
            A(2 * i, 1) = 0;
            A(2 * i, 2) = -pos.y();
            b(2 * i) = vx;

            // Row for y-component
            A(2 * i + 1, 0) = 0;
            A(2 * i + 1, 1) = 1;
            A(2 * i + 1, 2) = pos.x();
            b(2 * i + 1) = vy;
        }

        Eigen::Vector3d x = A.bdcSvd(Eigen::ComputeFullU | Eigen::ComputeFullV).solve(b);

        constexpr double epsilon = 1e-12;
        for (int i = 0; i < 3; ++i)
            if (std::abs(x(i)) < epsilon)
                x(i) = 0.0;

        return {x(0), x(1), x(2)};
    }


private:
    double width;
    double length;

    std::map<std::string, Eigen::Vector2d> modulePositions;
};

#endif // KINEMATICS_H
