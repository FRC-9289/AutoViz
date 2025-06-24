#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <Eigen/Dense>
#include <array>
#include <cmath>
#include <string>
#include <map>

class Kinematics {
public:
    struct ModuleData {
        double angle;    // Angle in radians (0 = forward, pi/2 = left)
        double velocity; // Velocity in m/s
    };

    struct Output {
        double v_x;    // Linear velocity in X (forward) direction
        double v_y;    // Linear velocity in Y (left) direction
        double omega;  // Angular velocity in rad/s
    };

    Kinematics(double width, double length) : width(width), length(length) {
        // Define module positions relative to robot center
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

            // Decompose velocity vector
            double vx = module.velocity * std::sin(-module.angle);
            double vy = module.velocity * std::cos(-module.angle);

            // Fill matrix A and vector b
            A(2 * i, 0) = 1;
            A(2 * i, 1) = 0;
            A(2 * i, 2) = -pos.y();

            A(2 * i + 1, 0) = 0;
            A(2 * i + 1, 1) = 1;
            A(2 * i + 1, 2) = pos.x();

            b(2 * i) = vx;
            b(2 * i + 1) = vy;
        }

        // Solve Ax = b using QR decomposition (Householder)
        Eigen::Vector3d x = A.householderQr().solve(b);

        double db = 1e-15; //Deadband

        if (std::abs(x(2)) < db) x(2) = 0;
        if (std::abs(x(1)) < db) x(1) = 0;
        if (std::abs(x(0)) < db) x(0) = 0;

        return {x(0), x(1), x(2)};
    }

private:
    double width;
    double length;
    std::map<std::string, Eigen::Vector2d> modulePositions;
};

#endif // KINEMATICS_H
