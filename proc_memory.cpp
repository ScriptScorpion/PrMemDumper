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

void read_process_memory(const pid_t &pid) {
    
    int fd = open(std::string("/proc/" + std::to_string(pid) + "/mem").c_str(), O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }
    
    off_t size = lseek(fd, 0, SEEK_END);
    off_t start = 0;

    if (size == -1) {
        std::string temp_start {};
        std::string temp_end {};
        std::cerr << "Cannot get file size of the process, you can try specify start and end offset yourself.\n";
        std::cout << "Enter start offset: ";
        // you can enter hex numbers but without '\x' or '0x' symbols 
        std::cin >> temp_start;
        if (!std::cin) {
            perror("input error");
        }
        std::cout << "Enter end offset: ";
        std::cin >> temp_end;
        if (!std::cin) {
            perror("input error");
        }
        try {
            start = std::stoll(temp_start.c_str(), nullptr, 16);
            size = std::stoll(temp_end.c_str(), nullptr, 16);
        } catch (std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            close(fd);
            return;
        }
        if (size < start) {
            std::cerr << "Error: end position is lesser than the start position" << std::endl;
            close(fd);
            return;
        }
        else if (start < 0 || size < 0) {
            std::cerr << "Error: start and end positions cannot be negative" << std::endl;
            close(fd);
            return;
        }
    }

    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
        perror("ptrace attach");
        close(fd);
        return;
    }
    
    waitpid(pid, NULL, 0);
    
    char *buff = new char[1];

    if (!buff || buff == nullptr) {
        perror("allocation failed");
        close(fd);
        return;
    }

    for (off_t i = start; i < size; ++i) {
        memset(buff, 0, 1);
        pread(fd, buff, 1, i);    
        if (std::isprint(buff[0])) {
            std::cout << buff[0];
        }
        else {
            std::cout << std::hex << (reinterpret_cast<int*>(static_cast<int>(buff[0])));
        }
        std::cout << " ";

    }
    std::cout << std::endl;
    ptrace(PTRACE_DETACH, pid, NULL, NULL);
    delete [] buff;
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
        read_process_memory(PID);
    }
    else {
        std::cout << "Usage: " << argv[0] << " <PID>" << std::endl; 
        return 0;
    }
}
