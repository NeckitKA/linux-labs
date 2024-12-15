#include <iostream>
#include <fstream>
#include <string>
#include <sys/inotify.h>
#include <unistd.h>
#include <ctime>
#include <iostream>

#define BUF_LEN 1024  // Размер буфера для чтения событий inotify
#define MAX_FILE_SIZE 1024 * 1024  // 1MB максимальный размер файла для чтения содержимого

// Функция для чтения содержимого файла
std::string read_file_content(const std::string &file_path) {
    std::ifstream file(file_path, std::ios::ate);  // Открыть файл в режиме "в конец"
    if (!file.is_open()) {
        return "";
    }

    // Проверка размера файла перед чтением содержимого
    if (file.tellg() > MAX_FILE_SIZE) {
        return "Файл слишком велик для чтения.";
    }

    file.seekg(0);  // Вернуться в начало файла
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// Функция для логирования изменений файлов в консоль и в файл
void log_change(const std::string &file_path, const std::string &action, const std::string &content = "") {
    // Формируем строку для логирования
    std::string log_message = "Файл " + file_path + " " + action;

    if (!content.empty()) {
        // Если файл изменен, добавляем его содержимое
        log_message += "\nего новое содержимое:\n" + content;
    }

    // Логирование в консоль
    std::cout << log_message << std::endl;

    // Логирование в текстовый файл
    std::ofstream log_file("file_changes.log", std::ios::app);  // Открытие файла для добавления данных
    if (log_file.is_open()) {
        log_file << log_message << "\n\n";  // Записываем сообщение в файл
        log_file.close();  // Закрываем файл
    } else {
        std::cerr << "Не удалось открыть файл для записи!" << std::endl;
    }
}

int main() {
    const std::string watch_folder = "/home/nick/demons/1/dimon/";  // Укажите путь к папке, которую хотите мониторить

    // Создание inotify дескриптора
    int fd = inotify_init();
    if (fd == -1) {
        std::cerr << "Ошибка при инициализации inotify!" << std::endl;
        return 1;
    }

    // Добавление папки для отслеживания
    int wd = inotify_add_watch(fd, watch_folder.c_str(), IN_CREATE | IN_DELETE | IN_MODIFY);
    if (wd == -1) {
        std::cerr << "Ошибка при добавлении наблюдения за папкой!" << std::endl;
        close(fd);
        return 1;
    }

    char buffer[BUF_LEN];  // Буфер для чтения событий

    // Размер структуры inotify_event
    const size_t EVENT_SIZE = sizeof(struct inotify_event);

    // Бесконечный цикл для обработки событий (без тайм-аута)
    while (true) {
        int length = read(fd, buffer, BUF_LEN);  // Чтение событий
        if (length == -1) {
            std::cerr << "Ошибка при чтении события!" << std::endl;
            break;
        }

        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];

            std::string file_path = watch_folder + "/" + event->name;  // Полный путь к измененному файлу
            if (event->mask & IN_CREATE) {
                // Файл был создан
                log_change(file_path, "добавлен");
            }
            if (event->mask & IN_DELETE) {
                // Файл был удален
                log_change(file_path, "удален");
            }
            if (event->mask & IN_MODIFY) {
                // Файл был изменен
                std::string content = read_file_content(file_path);
                log_change(file_path, "изменился", content);
            }

            i += EVENT_SIZE + event->len;  // Переход к следующему событию
        }
    }

    // Закрытие inotify дескриптора
    close(fd);
    return 0;
}
