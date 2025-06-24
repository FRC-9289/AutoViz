#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

struct VelocityData {
    double v_x;
    double v_y;
    double omega;
    double ts;
};

std::vector<VelocityData> readCSV(const std::string& filePath) {
    std::vector<VelocityData> results;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return results;
    }

    std::string line;
    std::getline(file, line);  // Read header

    std::unordered_map<std::string, int> columnIndices;
    std::istringstream headerStream(line);
    std::string column;
    int colIndex = 0;
    while (std::getline(headerStream, column, ',')) {
        columnIndices[column] = colIndex++;
    }

    // Check that required columns exist
    if (columnIndices.find("v_x") == columnIndices.end() ||
        columnIndices.find("v_y") == columnIndices.end() ||
        columnIndices.find("omega") == columnIndices.end() ||
        columnIndices.find("timestamp") == columnIndices.end()) {
        std::cerr << "Required columns v_x, v_y, omega not found in header." << std::endl;
        return results;
    }

    // Parse the rest of the rows
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::vector<std::string> values;
        std::string value;

        while (std::getline(lineStream, value, ',')) {
            values.push_back(value);
        }

        try {
            VelocityData data;
            data.v_x = std::stod(values[columnIndices["v_x"]]);
            data.v_y = std::stod(values[columnIndices["v_y"]]);
            data.omega = std::stod(values[columnIndices["omega"]]);
            data.ts = std::stod(values[columnIndices["timestamp"]]);
            results.push_back(data);
        } catch (const std::exception& e) {
            std::cerr << "Error parsing line: " << e.what() << std::endl;
        }
    }

    return results;
}

int main(int argc, char* argv[]) {

    std::string commandType = argv[1];

    if (commandType == "process") {

        std::string projectName = argv[2];

        std::string command = "cd Preprocessing && python3 preproc.py " + projectName;
        std::system(command.c_str());
    }
    else if (commandType == "get") {
        if (argc < 3) {
            std::cerr << "Usage: ./controller get [projectName]" << std::endl;
            return EXIT_FAILURE;
        }

        std::string projectName = argv[2];
        std::string filePath = projectName + ".csv";

        std::vector<VelocityData> velocities = readCSV(filePath);

        for (const auto& v : velocities) {
            std::cout << v.v_x << " " << v.v_y << " " << v.omega << " " << v.ts << std::endl;
        }
    }
    else if (commandType == "stop") {
        std::system("./stopjava");
    }

    return EXIT_SUCCESS;
}
