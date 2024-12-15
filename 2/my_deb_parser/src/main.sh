#!/bin/bash

# Проверка на наличие аргумента (название пакета)
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <package-name>"
    exit 1
fi

PACKAGE_NAME=$1

# Проверка наличия пакета
if ! dpkg -l | grep -q "^ii  $PACKAGE_NAME"; then
    echo "Package $PACKAGE_NAME is not installed or not found."
    exit 1
fi

# Определение информации о пакете с помощью apt
PACKAGE_INFO=$(apt show "$PACKAGE_NAME" 2>/dev/null)

# Проверка, что пакет существует в apt
if [ -z "$PACKAGE_INFO" ]; then
    echo "No information found for $PACKAGE_NAME."
    exit 1
fi

# Создание директории для хранения отчетов
REPORT_DIR="./reports/${PACKAGE_NAME}"
mkdir -p "$REPORT_DIR"

# Генерация ASCII-арт для названия пакета
ASCII_ART=$(echo "$PACKAGE_NAME" | figlet)

# Генерация отчета с автоматически извлеченной информацией
{
    echo "$ASCII_ART"
    echo
    echo "$PACKAGE_INFO"
    echo "Bugs: https://bugs.launchpad.net/ubuntu/+filebug"
    
    # Извлечение информации о зависимостях и других параметрах
    echo "APT-Manual-Installed: $(echo "$PACKAGE_INFO" | grep 'APT-Manual-Installed:' | awk '{print $3}')"
    echo "APT-Sources: $(echo "$PACKAGE_INFO" | grep 'APT-Sources:' | awk '{print $2, $3, $4, $5}')"
} > "$REPORT_DIR/${PACKAGE_NAME}.txt"

# Проверка на успешное создание отчета
if [ $? -eq 0 ]; then
    echo "Report generated at: $REPORT_DIR/${PACKAGE_NAME}.txt"
else
    echo "Failed to generate the report."
    exit 1
fi
