# AutoViz

**AutoViz** is a desktop app developed by **FRC Team 9289 - Viking Robotics** (Houston, TX) to visualize and simulate robot behavior using live data from the robot's NetworkTables.

## 🔧 What It Does

- Shows real-time robot movement on a field
- Visualizes autonomous and teleop paths
- Displays module states (angle, speed, etc.)
- Supports slippage and pose estimation with a Kalman filter

## 🧠 How It Works

1. Your robot code sends data to NetworkTables.
2. A Python script (`extract.py`) reads that data and sends it over TCP as JSON.
3. A Qt-based C++ app receives the data and displays it in a field simulator.

## 🛠️ Tech Stack

- **Python** – handles data extraction
- **C++ / Qt** – runs the GUI and field simulation
- **Java / WPILib** – robot code integration
- **Eigen** – for pose estimation and math
- **PathPlanner** – for autonomous path planning

## 🚀 How to Run

Download Qt App and Run

## Configuring your Robot Codebase

In order for AutoViz to work, your Robot codebase must report angle and velocity of each module to NetworkTables.
1. Create a NetworkTable utility through adding the following:
```
field = new Field2d();
this.Table = NetworkTableInstance.getDefault();
this.Table.startServer();
this.swerveTable = Table.getTable("SwerveDrive");
```
in the ```Drivetrain``` constructor. Add respective values outside for Java syntax
2. Add an updateNetworkTable method to ```SwerveModule.java``` or Similar
```
public void updateNetworkTable(NetworkTable table) {
    table.getEntry(name + "/angle").setDouble(currentState.angle.getDegrees());
    table.getEntry(name + "/velocity").setDouble(currentState.speedMetersPerSecond);
  }
private double getCurrentAngleRadians() {
    return Math.toRadians(turningMotor.getEncoder().getPosition());
  }
```

3. Add ```currentState = new SwerveModuleState(0.0, new Rotation2d(0.0));``` to ```SwerveModule``` contructor, or a similar class that handles the logic of each Swerve Module
4. Update the ```currentState``` continuously by adding ```currentState = SwerveModuleState.optimize(desiredState, new Rotation2d(getCurrentAngleRadians()));``` to your ```setDesiredState()``` method, or a similar method that updates SwerveModule states to their desired state.
5. Report to NetworkTables through calling ```updateNetworkTable``` in your Drivetrain ```periodic()``` method by adding the following: ```[nameOfSwerveModuleObject].updateNetworkTable(swerveTable);```

Note that any non-mandatory named methods mentioned here is referenced to [NetworkTablesDrivetrain](https://github.com/Aditya-2204/NetworkTables-for-Swerve), which is a demo repository for AutoViz.
