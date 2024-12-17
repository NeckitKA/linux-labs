#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

FILE=$1
if [ ! -e "$FILE" ]; then
    echo "Error: File '$FILE' not found."
    exit 1
fi

stat --format=' Файл: %n
  Размер: %s         Блоков: %b         Блок В/В: %o   %F
Устройство: %Dh/%dd  Инода: %i        Ссылки: %h
Доступ: (%a/%A)  Uid: (%u/%U)   Gid: (%g/%G)
Доступ:        %x
Модифицирован: %y
Изменён:       %z
Создан:        %w' "$FILE"

echo "Содержимое файла в шестнадцатеричном виде:"
xxd "$FILE"
