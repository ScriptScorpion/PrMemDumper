// locations to start you can find in '/proc/PID/maps' file
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <unistd.h>

void write_process_memory(const pid_t &pid) {
    
    int fd = open(std::string("/proc/" + std::to_string(pid) + "/mem").c_str(), O_WRONLY);
    if (fd == -1) {
        perror("open");
        return;
    }
    std::string temp_start {};
    off_t start;
    std::string value {};
    std::cout << "Enter offset to write: ";
    std::cin >> temp_start;
    if (!std::cin) {
        std::cerr << "Error: input error" << std::endl;
        close(fd);
        return;
    }
    try {
        if (temp_start.substr(0, 2) == "0x") {
            start = std::stoll(temp_start.c_str(), nullptr, 16);
        }
        else {
            start = std::stoll(temp_start.c_str(), nullptr, 10);
        }
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        close(fd);
        return;
    }

    if (start < 0) {
        std::cerr << "Error: position cannot be negative" << std::endl;
        close(fd);
        return;
    }
    std::cout << "Enter value to write: ";
    std::cin >> value;
    if (!std::cin) {
        std::cerr << "Error: input error" << std::endl;
        close(fd);
        return;
    }
    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
        perror("ptrace attach");
        close(fd);
        return;
    }
    
    waitpid(pid, NULL, 0);

    if (pwrite(fd, value.c_str(), value.length(), start) == -1) {
        std::cerr << "Error: write is unsuccessful of string: " << value << " at position: " << start << std::endl;
    }
    else {
        std::cout << "Writed successfully string: " << value << " at position: " << start << std::endl;
    }
    
    ptrace(PTRACE_DETACH, pid, NULL, NULL);
    close(fd);
}
int main (int argc, char *argv[]) {
    if (argc == 2) {
        pid_t PID;
        try {
            PID = std::stoi(argv[1], nullptr, 10);
        } catch (std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
        if (PID < 0) {
            std::cerr << "Error: PID cannot be negative" << std::endl;
            return 1;
        }
        write_process_memory(PID);
    }
    else {
        std::cout << "Usage: " << argv[0] << " <PID>" << std::endl; 
        return 0;
    }
}
