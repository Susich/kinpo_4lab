#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include "datatypes.h"

/**
 * @file utils.h
 * @author Сапунков А.Р.
 * @brief Вспомогательные функции для работы с файлами и парсинга данных.
 */

/**
 * @brief Читает всё содержимое файла.
 * @param filePath Путь к файлу.
 * @return Содержимое файла в виде строки.
 * @throw Error В случае ошибки открытия файла.
 */
QString readFile(const QString& filePath);

/**
 * @brief Записывает строку в файл.
 * @param filePath Путь к файлу.
 * @param data Строка для записи.
 * @throw Error В случае ошибки открытия файла для записи.
 */
void writeToFile(const QString& filePath, const QString& data);

/**
 * @brief Парсит строку с параметрами грузовика.
 * @param truckDataString Строка формата "StartNode EndNode Weight Height"
 * @return Заполненная структура Truck.
 * @throw Error В случае неверного формата строки или ошибки конвертации.
 */
Truck parseTruckData(const QString& truckDataString);

#endif // UTILS_H
