#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>
#include <fstream>

static char sourceFile[256] = "CMakeCache.txt";  // File to copy
static char password[128] = "";                         // Root password
static bool showPassword = false;                      // Show/hide password
static std::string statusMessage = "";                 // Success/error message

// Path for the systemd service
const std::string SERVICE_PATH = "/etc/systemd/system/myapp.service";
const std::string EXECUTABLE_PATH = "/path/to/your/app";  // Change this to your app

// Function to check if the service exists
bool isServiceEnabled() {
    return (system(("test -f " + SERVICE_PATH).c_str()) == 0);
}

// Function to enable autostart (create systemd service)
bool enableAutoStart(const std::string& password) {
    std::ofstream serviceFile("/tmp/myapp.service");
    if (!serviceFile) return false;

    // Write systemd service content
    serviceFile << "[Unit]\n";
    serviceFile << "Description=My App Auto Start\n";
    serviceFile << "After=network.target\n\n";
    serviceFile << "[Service]\n";
    serviceFile << "ExecStart=" << EXECUTABLE_PATH << "\n";
    serviceFile << "Restart=always\n";
    serviceFile << "User=root\n";
    serviceFile << "WorkingDirectory=" << EXECUTABLE_PATH.substr(0, EXECUTABLE_PATH.find_last_of('/')) << "\n\n";
    serviceFile << "[Install]\n";
    serviceFile << "WantedBy=multi-user.target\n";
    serviceFile.close();

    // Move service file to /etc/systemd/system/
    const std::string command_mv = "echo '" + password + "' | sudo -S mv /tmp/myapp.service " + SERVICE_PATH + " && sudo chmod 644 " + SERVICE_PATH + " && sudo systemctl daemon-reload && sudo systemctl enable myapp.service";

    // Execute the command
    int result_mv = system(command_mv.c_str());

    const std::string command_forgetPWD = "sudo -k";
    int result_forgetPWD = system(command_forgetPWD.c_str());
    if(result_forgetPWD != 0) printf("Failed to forget\n");

    return (result_mv == 0); // Return true if successful
}

// Function to disable autostart (remove systemd service)
bool disableAutoStart(const std::string& password) {
    std::string command = "echo '" + password + "' | sudo -S systemctl disable myapp.service && sudo rm -f " + SERVICE_PATH;
    return (system(command.c_str()) == 0);
}

#endif // UTILS_HPP