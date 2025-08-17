#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <Eigen/Dense>
#include <array>
#include <cmath>
#include <string>
#include <map>
#include <iostream>
#include "KalmanFilter.h"

class Kinematics {
public:
    struct ModuleData {
        double angle;    // radians
        double velocity; // m/s
    };

    struct Output {
        double v_x;
        double v_y;
        double omega;
    };

    Kinematics(double width, double length)
        : width(width), length(length), kf(0.01) // dt = 0.01
    {
        modulePositions["LB"] = Eigen::Vector2d(-width/2, -length/2);
        modulePositions["LF"] = Eigen::Vector2d(-width/2,  length/2);
        modulePositions["RB"] = Eigen::Vector2d( width/2, -length/2);
        modulePositions["RF"] = Eigen::Vector2d( width/2,  length/2);
    }

    Output estimate(const std::array<ModuleData,4>& modules) {
        Eigen::Matrix<double, 8, 3> A;
        Eigen::Matrix<double, 8, 1> b;

        static const std::array<std::string,4> keys = {"LB","LF","RB","RF"};

        for (int i = 0; i < 4; ++i) {
            const auto& module = modules[i];
            const auto& pos = modulePositions[keys[i]];

            double vx = -module.velocity * std::sin(module.angle);
            double vy =  module.velocity * std::cos(module.angle);

            // x-component row
            A(2*i,0) = 1; A(2*i,1) = 0; A(2*i,2) = -pos.y();
            b(2*i) = vx;

            // y-component row
            A(2*i+1,0) = 0; A(2*i+1,1) = 1; A(2*i+1,2) = pos.x();
            b(2*i+1) = vy;
        }

        Eigen::Vector3d x = A.bdcSvd(Eigen::ComputeFullU | Eigen::ComputeFullV).solve(b);

        constexpr double eps = 1e-12;
        for(int i=0;i<3;i++)
            if(std::abs(x(i))<eps) x(i)=0.0;

        // Kalman prediction & update
        Eigen::Vector3d procState = kf.predict(x(0), x(1), x(2));
        kf.update(procState);

        return { x(0), x(1), x(2) };
    }

private:
    double width;
    double length;
    KalmanFilter2DRobot kf;
    std::map<std::string, Eigen::Vector2d> modulePositions;
};

#endif // KINEMATICS_H
