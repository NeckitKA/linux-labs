#!/bin/bash

# Переменные
projects_dir="$HOME/projects"
new_folder_name="${projects_dir}/project_$1"

# Проверка существования папки
if [ -d "$new_folder_name" ]; then
    echo "Папка $new_folder_name уже существует! Завершение скрипта."
    exit 1
fi

# Создаем папку ~/projects, если она еще не существует
mkdir -p "$projects_dir"
mkdir "$new_folder_name"

# Создаем README.md
text="
# Проект: project_$1

Всем привет! =) 
Вот вам значение второй переменной: $2
"
echo -e "$text" > "${new_folder_name}/README.md"

# Открываем README.md через nano для редактирования
nano "${new_folder_name}/README.md"

# Инициализация git-репозитория
cd "$new_folder_name"
git init
git add README.md
git commit -a -m "первый коммит"

# Создание SSH-ключей с алгоритмом Ed25519
ssh_key_path="$HOME/.ssh/id_ed25519"
if [ ! -f "$ssh_key_path" ]; then
    ssh-keygen -t ed25519 -f "$ssh_key_path" -N ""
    echo "Host github.com
        HostName github.com
        IdentityFile $ssh_key_path
        User git" » ~/.ssh/config
    echo "SSH ключи успешно созданы и добавлены в ~/.ssh/config"
else
    echo "SSH ключи уже существуют."
fi

# Информация о создании репозитория и публичный ключ
echo "Ваш новый репозиторий создан в $new_folder_name"
echo "Добавьте его на GitHub с помощью следующей команды:"
echo "git remote add origin git@github.com:ваш_юзернейм/project_$1.git"
echo "Ваш публичный ключ SSH:"
cat "${ssh_key_path}.pub"

exit 0
