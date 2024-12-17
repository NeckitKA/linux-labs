#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/param.h>
#include <cstdio>
#include <syslog.h>
#include <ctime>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

std::string getCurrentTime() {
    std::time_t t = std::time(nullptr);
    std::tm* timePtr = std::localtime(&t);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%b %d %H:%M:%S", timePtr);
    return std::string(buffer);
}

std::string runCommand(const std::string &command) {
    char buffer[128];
    std::string result = "";
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "Ошибка при открытии канала для чтения!" << std::endl;
        return "";
    }
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    fclose(pipe);
    return result;
}

std::string readFile(const std::string &filePath) {
    std::ifstream file(filePath);
    std::string content;
    if (file.is_open()) {
        std::getline(file, content);
        file.close();
    } else {
        std::cerr << "Ошибка при открытии файла." << std::endl;
    }
    return content;
}

void logToFile(const std::string &message) {
    std::ofstream logFile("/home/nick/labs/demons/3/mydemon.log", std::ios_base::app);
    if (logFile.is_open()) {
        logFile << message << std::endl;
    } else {
        std::cerr << "Ошибка при записи в лог файл." << std::endl;
    }
}

void logToSyslog(const std::string &message) {
    syslog(LOG_NOTICE, "%s", message.c_str());
}

void monitorFile(const std::string &filePath, const std::string &tool) {
    int fd = inotify_init();
    if (fd == -1) {
        std::cerr << "Ошибка инициализации inotify!" << std::endl;
        return;
    }

    int wd = inotify_add_watch(fd, filePath.c_str(), IN_MODIFY);
    if (wd == -1) {
        std::cerr << "Ошибка добавления наблюдателя!" << std::endl;
        close(fd);
        return;
    }

    char buffer[BUF_LEN];

    while (true) {
        int length = read(fd, buffer, BUF_LEN);
        if (length == -1) {
            std::cerr << "Ошибка при чтении событий!" << std::endl;
            break;
        }

        for (int i = 0; i < length; i += sizeof(struct inotify_event) + ((struct inotify_event *) &buffer[i])->len) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (event->mask & IN_MODIFY) {
                std::string content = readFile(filePath);
                if (!content.empty()) {
                    std::string command = tool + " \"" + content + "\"";
                    std::string result = runCommand(command);

                    std::string timestamp = getCurrentTime();
                    std::string logMessage = timestamp + " /home/nick/labs/demons/3/1.txt: изменился файл";

                    // Выводим результат в stdout (для systemd)
                    std::cout << logMessage << std::endl;
                    std::cout << result << std::endl;

                    // Логируем в файл
                    logToFile(logMessage + "\n" + result);
                    // Логируем в системный журнал
                    logToSyslog(logMessage + "\n" + result);
                }
            }
        }
    }

    close(fd);
}

int main() {
    openlog("mydaemon", LOG_PID | LOG_CONS, LOG_DAEMON);

    std::string filePath = "/home/nick/labs/demons/3/1.txt";
    std::string tool = "/usr/games/cowsay";

    std::cout << "Начинаю мониторинг файла " << filePath << " с использованием утилиты " << tool << "..." << std::endl;
    logToSyslog("Начинаю мониторинг файла.");

    monitorFile(filePath, tool);

    closelog();
    return 0;
}
