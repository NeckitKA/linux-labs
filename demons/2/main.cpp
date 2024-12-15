#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/param.h> // Для определения NAME_MAX
#include <cstdio> // Для использования popen()

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

// Функция для вызова утилиты cowsay или figlet и получения вывода
std::string runCommand(const std::string &command) {
    char buffer[128];
    std::string result = "";
    FILE *pipe = popen(command.c_str(), "r");  // Открываем процесс для чтения вывода
    if (!pipe) {
        std::cerr << "Ошибка при открытии канала для чтения!" << std::endl;
        return "";
    }
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;  // Собираем вывод команды
    }
    fclose(pipe);  // Закрываем канал
    return result;
}

// Функция для чтения содержимого файла
std::string readFile(const std::string &filePath) {
    std::ifstream file(filePath);
    std::string content;
    if (file.is_open()) {
        std::getline(file, content);  // Читаем только первую строку
        file.close();
    } else {
        std::cerr << "Ошибка при открытии файла." << std::endl;
    }
    return content;
}

// Функция для мониторинга файла с помощью inotify
void monitorFile(const std::string &filePath, const std::string &tool) {
    int fd = inotify_init();
    if (fd == -1) {
        std::cerr << "Ошибка инициализации inotify!" << std::endl;
        return;
    }

    int wd = inotify_add_watch(fd, filePath.c_str(), IN_MODIFY); // Следим за изменениями в файле
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

        // Проверяем, что событие связано с изменением файла
        for (int i = 0; i < length; i += sizeof(struct inotify_event) + ((struct inotify_event *) &buffer[i])->len) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (event->mask & IN_MODIFY) {
                std::string content = readFile(filePath);
                if (!content.empty()) {
                    std::string command = tool + " \"" + content + "\""; // Формируем команду для утилиты
                    std::string result = runCommand(command);  // Получаем результат выполнения команды
                    std::cout << result << std::endl;  // Выводим результат
                }
            }
        }
    }

    close(fd);
}

int main() {
    std::string filePath = "/home/nick/demons/2/1.txt";
    std::string tool = "cowsay";

    std::cout << "Начат мониторинг " << filePath << " с использованием " << tool << "..." << std::endl;
    monitorFile(filePath, tool);

    return 0;
}
