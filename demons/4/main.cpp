#include <iostream>
#include <sys/inotify.h>
#include <unistd.h>
#include <stdexcept> // Для исключений
#include <cstring>   // Для strerror()
#include <fstream>   // Для работы с файлами
#include <string>    // Для работы с строками
#include <syslog.h>  // Для работы с журналом

using namespace std;

// Функция для проверки содержания файла на наличие фразы "Умри"
bool check_for_kill_phrase(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Не удалось открыть файл: " << filename << endl;
        return false;
    }

    string line;
    while (getline(file, line)) {
        if (line.find("Смерть") != string::npos) {
            return true; // Фраза найдена
        }
    }
    return false; // Фразы не найдены
}

int main(int argc, char *argv[]) {
    // Инициализация inotify
    int inotify_fd = inotify_init();
    if (inotify_fd == -1) {
        cerr << "Ошибка инициализации inotify: " << strerror(errno) << endl;
        syslog(LOG_ERR, "Ошибка инициализации inotify: %s", strerror(errno));
        return 1;
    }

    // Добавление наблюдателя для каталога /home/nick/demons/4/4.4/
    int watch_fd = inotify_add_watch(inotify_fd, "/home/nick/demons/4/4.4/", IN_MODIFY | IN_CREATE | IN_DELETE);
    if (watch_fd == -1) {
        cerr << "Ошибка добавления наблюдателя: " << strerror(errno) << endl;
        syslog(LOG_ERR, "Ошибка добавления наблюдателя: %s", strerror(errno));
        close(inotify_fd);
        return 1;
    }

    // Открываем syslog для записи сообщений
    openlog("FileWatcher", LOG_PID | LOG_CONS, LOG_USER);

    cout << "-=-=-=-=-=-" << endl
         << "ЗАПУСКАЮСЬ" << endl
         << "-=-=-=-=-=-" << endl;

    // Основной цикл обработки событий
    while (true) {
        char buffer[10000];
        int length = read(inotify_fd, buffer, sizeof(buffer));

        if (length == -1) {
            cerr << "Ошибка чтения событий: " << strerror(errno) << endl;
            syslog(LOG_ERR, "Ошибка чтения событий: %s", strerror(errno));
            break;
        }

        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];

            // Если событие связано с файлом, проверяем его на фразу "Умри"
            if (event->len > 0) {
                string filepath = "/home/nick/demons/4/4.4/" + string(event->name);

                if (event->mask & IN_CREATE) {
                    cout << "Файл создан: " << filepath << endl;
                    syslog(LOG_INFO, "Файл создан: %s", filepath.c_str());
                }
                if (event->mask & IN_DELETE) {
                    cout << "Файл удалён: " << filepath << endl;
                    syslog(LOG_INFO, "Файл удалён: %s", filepath.c_str());
                }
                if (event->mask & IN_MODIFY) {
                    cout << "Файл изменён: " << filepath << endl;
                    syslog(LOG_INFO, "Файл изменён: %s", filepath.c_str());
                    if (check_for_kill_phrase(filepath)) {
                        cerr << "Фраза 'Умри' найдена в файле: " << filepath << endl;
                        syslog(LOG_ERR, "Фраза 'Умри' найдена в файле: %s", filepath.c_str());
                        // Генерация исключения при нахождении фразы
                        throw runtime_error("Фраза 'Умри' найдена! Программа завершена.");
                    }
                }
            }

            i += sizeof(struct inotify_event) + event->len;
        }
    }

    // Удаление наблюдателя и закрытие дескриптора
    inotify_rm_watch(inotify_fd, watch_fd);
    close(inotify_fd);

    // Закрываем syslog
    closelog();

    return 0;
}

