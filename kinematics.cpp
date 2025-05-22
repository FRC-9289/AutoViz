#include "kinematics.h"
#include <Eigen/Dense>
#include <cmath>

// Constructor definition
kinematics::kinematics(double width, double length, double mass, double frictionCoefficient, double deltaTime)
    : W(width), L(length), m(mass), mu(frictionCoefficient), dt(deltaTime), g(9.81)
{
    modulePositions[0] = Eigen::Vector2d(+L / 2.0, +W / 2.0); // Front Left
    modulePositions[1] = Eigen::Vector2d(+L / 2.0, -W / 2.0); // Front Right
    modulePositions[2] = Eigen::Vector2d(-L / 2.0, +W / 2.0); // Back Left
    modulePositions[3] = Eigen::Vector2d(-L / 2.0, -W / 2.0); // Back Right
}

// Full function definition
kinematics::Output kinematics::estimate(const std::array<ModuleData, 4>& modules) {
    Output output;
    output.isSlipping.fill(false);

    double Fmax = mu * (m / 4.0) * g; //For each module - F=μ*ma - dampening or increasing required forced - max force is weight of modules

    Eigen::MatrixXd A(8, 3); //Initialize matrix A - 8 value for v_xi, v_yi, v_omegai for each module i
    Eigen::VectorXd b(8);    //Known components of each module velocity v_xi, v_yi for each module i

    for (int i = 0; i < 4; ++i) {
        const auto& mod = modules[i];
        Eigen::Vector2d pos = modulePositions[i];

        Eigen::Vector2d v_wheel(mod.velocity * std::cos(mod.angle),
                                mod.velocity * std::sin(mod.angle));

        A(2 * i, 0) = 1;            // Coefficient for vx
        A(2 * i, 1) = 0;            // No vy in x-direction
        A(2 * i, 2) = -pos.y();     // ω × r contributes -y to x-component

        A(2 * i + 1, 0) = 0;        // No vx in y-direction
        A(2 * i + 1, 1) = 1;        // Coefficient for vy
        A(2 * i + 1, 2) = pos.x();  // ω × r contributes +x to y-component

        b(2 * i)     = v_wheel.x(); // x-component of module's measured velocity
        b(2 * i + 1) = v_wheel.y(); // y-component of module's measured velocity
    }

    Eigen::Vector3d x = A.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b); /*Solve for omega and linear velocity (estimate it) using Least Squares Method
                                                                                       - Reducing distance between vectors by changing parameters
                                                                                        */
    output.linearVelocity = x.head<2>();
    output.omega = x(2);

    for (int i = 0; i < 4; ++i) {
        Eigen::Vector2d pos = modulePositions[i];
        Eigen::Vector2d v_pred = output.linearVelocity + output.omega * Eigen::Vector2d(-pos.y(), pos.x()); // v_predi = v_i + ω_i * [-y_i,x_i] for each module i

        const auto& mod = modules[i];
        Eigen::Vector2d v_current(mod.velocity * std::cos(mod.angle), //Calculate current velocity [vcos theta, vsin theta] -> [v_x, v_y]
                                  mod.velocity * std::sin(mod.angle));
        Eigen::Vector2d acceleration = (v_current - mod.prevVelocity) / dt; //dv/dt
        Eigen::Vector2d force = (m / 4.0) * acceleration; // F=ma, where m is the mass of each module

        if (force.norm() > Fmax) { //If the force maagnitude is greater than the maximum force exerted by module i - Slipping failure
            output.isSlipping[i] = true;
        }
    }

    return output;
}
