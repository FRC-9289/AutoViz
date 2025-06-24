#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <regex>
#include <vector>
#include <cstdlib>  // for system()
#include <sstream>  // << ADD THIS

// Helper function to run a command and get its output as a string
std::string exec(const char* cmd) {
    std::array<char, 128> buffer{};
    std::string result;

    // Open pipe to file
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    // Read until end of process:
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

std::string get_java_process_id() {
    std::string output = exec("lsof -i");

    // Split output into lines
    std::istringstream stream(output);
    std::string line;

    std::regex number_regex(R"(\d+)");
    while (std::getline(stream, line)) {
        if (line.find("java") != std::string::npos) {
            // Find all numbers in the line
            std::sregex_iterator numbers_begin = std::sregex_iterator(line.begin(), line.end(), number_regex);
            std::sregex_iterator numbers_end = std::sregex_iterator();

            if (numbers_begin != numbers_end) {
                // Return first matched number (PID)
                return numbers_begin->str();
            }
        }
    }

    return ""; // No java process found
}

void kill_java_process() {
    std::string pid = get_java_process_id();
    if (!pid.empty()) {
        std::string command = "kill -9 " + pid;
        int ret = system(command.c_str());
        if (ret == 0) {
            std::cout << "Java process with PID " << pid << " has been killed.\n";
        } else {
            std::cerr << "Failed to kill Java process with PID " << pid << ".\n";
        }
    } else {
        std::cout << "No Java process found to kill.\n";
    }
}

int main() {
    kill_java_process();
    return 0;
}
