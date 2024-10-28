/**
 * @brief A simple CLI application to help debug and setup the arduino program.
 * @author Kevin Orbie
 */

#define ARDUINO_CLI_VERSION 0

/* ========================== Include ========================== */
/* Standard C Libraries */
#include <unistd.h>  // getopt

/* Standard C++ Libraries */
#include <iostream>
#include <cstdlib>
#include <memory>
#include <string>

/* Third Party Libraries */
// None

/* Custom C++ Includes */
#include "robot/arduino_driver.h"


/* =========================== Globals ========================= */
std::unique_ptr<ArduinoDriver> arduino_driver = nullptr;


/* ===================== Argument Functions ==================== */
static void help() {
    std::string msg = "";

    /* Usage. */
    msg += "usage: arduino_cli [options]\n";

    /* Explanation. */
    msg += "\nOptions:\n";
    msg += "  -h              display this help message\n";
    msg += "  -d <device>     set Arduino device file\n";
    msg += "  -b <baud>       set Arduino connection baudrate\n";
    
    msg += "\n";

    fprintf(stderr, "%s", msg.c_str());
};

static void summary(std::string device, int baudrate) {
    LOGI("--------- Summary ---------");
    LOGI("  > Device   : %s", device.c_str());
    LOGI("  > Baudrate : %d", baudrate);
    LOGI("---------------------------");
};


/* ======================= CLI Functions ======================= */
namespace cli {

static void help() {
    std::string msg = "";

    /* Explanation. */
    msg += "Commands:\n";
    msg += "  help             show this message\n";
    msg += "  cali acc         calibrates the IMU acceleration and gyroscope\n";
    msg += "  cali mag         calibrates the IMU magnetometer\n";
    msg += "  baud <baud>      set IMU connection baudrate\n";
    msg += "  rate <rate>      set IMU packet output rate\n";
    msg += "  cont <c1, ...>   specify what data the IMU outputs\n";
    msg += "  driv <hex>       send the given hex as drive command\n";
    msg += "  exit|quit        stop this application\n";
    
    msg += "\n";

    fprintf(stderr, "%s", msg.c_str());
}

/**
 * @brief Process the given CLI line.
 * @returns True if the CLI application should exit, false otherwise.
 */
static bool process(std::string line) {
    if (line.size() < 4) { return false; }
    std::string command = line.substr(0, 4);
    
    if (command == "exit" || command == "quit") {
        return true;
 
    } else if (command == "baud") {
        std::string value = line.substr(4);
        int baud = std::stoi(value);
        switch (baud) {
            case 9600: arduino_driver->setIMUBaudrate(arduino::BaudRate::BD_9600); break;
            case 19200: arduino_driver->setIMUBaudrate(arduino::BaudRate::BD_19200); break;
            case 38400: arduino_driver->setIMUBaudrate(arduino::BaudRate::BD_38400); break;
            case 115200: arduino_driver->setIMUBaudrate(arduino::BaudRate::BD_115200); break;
            
            default:
                LOGE("Baudrate not supported: %s", value.c_str());
                break;
        }

    } else if (command == "rate") {
        std::string value = line.substr(4);
        int rate = std::stoi(value);
        switch (rate) {
            case 2: arduino_driver->setIMUOutputRate(arduino::OutputRate::OR_02HZ); break; // 0.2HZ
            case 10: arduino_driver->setIMUOutputRate(arduino::OutputRate::OR_10HZ); break;
            case 50: arduino_driver->setIMUOutputRate(arduino::OutputRate::OR_50HZ); break;
            case 100: arduino_driver->setIMUOutputRate(arduino::OutputRate::OR_100HZ); break;
            case 200: arduino_driver->setIMUOutputRate(arduino::OutputRate::OR_200HZ); break;
            
            default:
                LOGE("Output rate not supported: %s", value.c_str());
                break;
        }

    } else if (command == "cont") {
        int32_t contents = 0;
        if (line.find("ACC") != std::string::npos) { contents |= static_cast<int32_t>(arduino::Content::CNT_ACC); }  
        if (line.find("GYR") != std::string::npos) { contents |= static_cast<int32_t>(arduino::Content::CNT_GYRO); }  
        if (line.find("ANG") != std::string::npos) { contents |= static_cast<int32_t>(arduino::Content::CNT_ANGLE); }  
        if (line.find("MAG") != std::string::npos) { contents |= static_cast<int32_t>(arduino::Content::CNT_MAG); }  
        arduino_driver->setIMUContent(contents);

    } else if (command == "driv") {
        if (line.size() >= 7) {
            std::string value = line.substr(4);
            uint8_t cmd = std::strtol(value.c_str(), nullptr, 16);
            arduino_driver->sendDriveCmd(&cmd);
            LOGI("Sent Driv command: %X", static_cast<int>(cmd));
        } else {
            LOGE("Invalid Command Given!");
        }

    } else if (command == "cali") {
        if (line == "cali acc") {
            arduino_driver->calibrateAccGyro();
        } else if (line == "cali mag") {
            arduino_driver->calibrateMag();
        } else {
            cli::help();
        }

    } else {
        cli::help();
    }

    return false;
}

} // namespace cli


/* ======================== Entry Point ======================== */
int main(int argc, char *argv[]) {
    /* ------------------ Default Values ------------------ */
    std::string device = "/dev/ttyACM0";
    int baudrate = 9600;

    /* ----------------- Parse User Input ----------------- */
    int option;
    while ((option = getopt(argc, argv, "d:b:h")) != -1) {
        switch (option) {
            case 'd':
                device = std::string(optarg);
                break;
            case 'b': 
                baudrate = std::stoi(std::string(optarg));
                break;
            default: 
                help();
                return EXIT_SUCCESS;
        }
    }

    /* ---------------- Post Process Values --------------- */    
    /* Notify user of used settings. */
    LOGI("Arduino CLI: Version %d", ARDUINO_CLI_VERSION);
    summary(device, baudrate);

    /* ---------------- Setup & Run System ---------------- */
    arduino_driver = std::make_unique<ArduinoDriver>();

    /* Setup & Start Arduino Driver. */
    arduino_driver->thread();

    /* CLI Interface. */
    bool finished = false;
    for (std::string line; !finished && std::cout << "CLI > " && std::getline(std::cin, line);) {
        if (!line.empty()) { 
            finished = cli::process(line); 
        }
    }
    
    /* Command threads to finnish. */
    arduino_driver->stop();

    return EXIT_SUCCESS;
}
