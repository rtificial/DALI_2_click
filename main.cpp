#include <wiringPi.h>
#include <iostream>
#include <csignal>
#include <sys/time.h>
#include <thread>
#include <chrono>
#include "qqqDALI.h"

#define DALI_TX_PIN 5 // GPIO 5
#define DALI_RX_PIN 6 // GPIO 6

Dali dali; // Global instance of Dali

// Inverted functions
uint8_t bus_is_high() {
    return !digitalRead(DALI_RX_PIN); // Inverted logic
}

void bus_set_low() {
    digitalWrite(DALI_TX_PIN, HIGH); // Inverted logic
}

void bus_set_high() {
    digitalWrite(DALI_TX_PIN, LOW); // Inverted logic
}

void signalHandler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    exit(signum);
}

void decodeAndPrintStatus(uint8_t status) {
    std::cout << "Ballast Status: " << std::hex << (int)status << std::dec << std::endl;
    std::cout << "Status Details:" << std::endl;
    std::cout << "Lamp Failure: " << ((status & 0x80) ? "Yes" : "No") << std::endl;
    std::cout << "Lamp On: " << ((status & 0x40) ? "Yes" : "No") << std::endl;
    std::cout << "Limit Error: " << ((status & 0x20) ? "Yes" : "No") << std::endl;
    std::cout << "Fade Running: " << ((status & 0x10) ? "Yes" : "No") << std::endl;
    std::cout << "Reset State: " << ((status & 0x08) ? "Yes" : "No") << std::endl;
    std::cout << "Missing Short Address: " << ((status & 0x04) ? "Yes" : "No") << std::endl;
    std::cout << "Power Failure: " << ((status & 0x02) ? "Yes" : "No") << std::endl;
}

void timerHandler(int signum) {
    dali.timer();
}

void setupTimer() {
    struct itimerval timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 104; // 104 microseconds for 9600 Hz
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 104;

    signal(SIGALRM, timerHandler);
    setitimer(ITIMER_REAL, &timer, nullptr);
}

int main() {
    wiringPiSetupGpio(); // Initialize WiringPi using the Broadcom GPIO pin numbers
    pinMode(DALI_TX_PIN, OUTPUT);
    pinMode(DALI_RX_PIN, INPUT);

    // Initialize Dali object and start the timer
    dali.begin(bus_is_high, bus_set_low, bus_set_high);

    // Setup the timer to call the timer function every 104.167 microseconds
    setupTimer();

    // Register signal handler for clean exit
    signal(SIGINT, signalHandler);

    while (true) {
        int16_t response = dali.cmd(DALI_QUERY_STATUS, 0xFF);
        if (response >= 0) {
            decodeAndPrintStatus(response);
        } else {
            std::cout << "No response or error: " << response << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
