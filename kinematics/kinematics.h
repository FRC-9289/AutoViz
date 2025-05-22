#include <Eigen/Dense>
#include <array>
#include <cmath>

class kinematics {
public:
    struct ModuleData {
        double angle;                 // radians
        double velocity;              // m/s
        Eigen::Vector2d prevVelocity; // previous velocity vector (m/s)
    };

    struct Output {
        Eigen::Vector2d linearVelocity; // robot linear velocity (m/s)
        double omega;                   // robot rotational velocity (rad/s)
        std::array<bool, 4> isSlipping; // slip flags per module
    };

    kinematics(double width, double length, double mass, double frictionCoefficient, double deltaTime)
        : W(width), L(length), m(mass), mu(frictionCoefficient), dt(deltaTime), g(9.81)
    {
        // Module positions (x forward, y left)
        modulePositions[0] = Eigen::Vector2d(+L / 2.0, +W / 2.0); // Front Left
        modulePositions[1] = Eigen::Vector2d(+L / 2.0, -W / 2.0); // Front Right
        modulePositions[2] = Eigen::Vector2d(-L / 2.0, +W / 2.0); // Back Left
        modulePositions[3] = Eigen::Vector2d(-L / 2.0, -W / 2.0); // Back Right

    }

    Output estimate(const std::array<ModuleData, 4>& modules) {
        Output output;
        output.isSlipping.fill(false);

        double Fmax = mu * (m / 4.0) * g; // max traction force per wheel

        Eigen::MatrixXd A(8, 3);  // 4 wheels * 2 dims (x,y) rows, 3 unknowns: vx, vy, omega
        Eigen::VectorXd b(8);     // velocity components vector

        for (int i = 0; i < 4; ++i) {
            const auto& mod = modules[i];
            Eigen::Vector2d pos = modulePositions[i];

            // Wheel velocity vector in robot frame
            Eigen::Vector2d v_wheel(mod.velocity * std::cos(mod.angle),
                                    mod.velocity * std::sin(mod.angle));

            // Rows for x and y components
            A(2 * i, 0) = 1;           // vx coefficient
            A(2 * i, 1) = 0;           // vy coefficient
            A(2 * i, 2) = -pos.y();    // omega coefficient (-y for x-component)

            A(2 * i + 1, 0) = 0;       // vx coefficient
            A(2 * i + 1, 1) = 1;       // vy coefficient
            A(2 * i + 1, 2) = pos.x(); // omega coefficient (x for y-component)

            // b vector: measured wheel velocity components
            b(2 * i) = v_wheel.x();
            b(2 * i + 1) = v_wheel.y();
        }

        // Least squares solution for [vx, vy, omega]
        Eigen::Vector3d x = A.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);

        output.linearVelocity = x.head<2>(); // vx, vy
        output.omega = x(2);

        // Slippage detection per module:
        for (int i = 0; i < 4; ++i) {
            Eigen::Vector2d pos = modulePositions[i];
            // Calculate wheel velocity predicted by linear + rotational velocity
            Eigen::Vector2d v_pred = output.linearVelocity + output.omega * Eigen::Vector2d(-pos.y(), pos.x());

            // Acceleration approx: (current - prev)/dt
            const auto& mod = modules[i];
            Eigen::Vector2d v_current(mod.velocity * std::cos(mod.angle),
                                      mod.velocity * std::sin(mod.angle));
            Eigen::Vector2d acceleration = (v_current - mod.prevVelocity) / dt;

            // Force on this wheel: mass fraction * acceleration
            Eigen::Vector2d force = (m / 4.0) * acceleration;

            if (force.norm() > Fmax) {
                output.isSlipping[i] = true;
            }
        }

        return output;
    }

private:
    double W, L;           // width, length (m)
    double m;              // mass (kg)
    double mu;             // friction coefficient
    double dt;             // delta time (s)
    const double g;        // gravity (m/s^2)
    std::array<Eigen::Vector2d, 4> modulePositions;
};
