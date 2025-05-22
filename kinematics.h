#include <Eigen/Dense>

class kinematics {
public:
    struct ModuleData {
        double angle;
        double velocity;
        Eigen::Vector2d prevVelocity;
    };

    struct Output {
        Eigen::Vector2d linearVelocity;
        double omega;
        std::array<bool, 4> isSlipping;
    };

    kinematics(double width, double length, double mass, double frictionCoefficient, double deltaTime);

    Output estimate(const std::array<ModuleData, 4>& modules); // <--- Declaration only

private:
    double W /*Width*/, L /*Length*/, m /*Mass*/, mu /* friction coef μ*/, dt /*change in time or elapsed time*/;
    const double g /*g=9.81m/s^2*/;
    std::array<Eigen::Vector2d, 4> modulePositions;
};
