#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include "datatypes.h"

/**
 * @file utils.h
 * @author Сапунков А.Р.
 * @brief Декларация утилитных функций ввода-вывода, парсинга и генерации отчетов.
 */

class Graph;

/**
 * @brief Читает текстовый файл с диска в строку
 * @param[in] filename Путь к файлу на диске
 * @param[out] content Строка, в которую будет записано всё содержимое файла
 * @return Error Объект ошибки со статусом NoError или InputFileNotFound
 */
Error readFile(const QString& filename, QString& content);

/**
 * @brief Сохраняет переданную строку в текстовый файл на диск
 * @details Принудительно использует кодировку UTF-8 для корректного сохранения кириллицы.
 * @param[in] filename Путь для создания/перезаписи файла
 * @param[in] content Текстовая строка для записи
 * @return Error Объект ошибки со статусом NoError или OutputCreateFail
 */
Error writeToFile(const QString& filename, const QString& content);

/**
 * @brief Выполняет синтаксический анализ и валидацию файла параметров автомобиля
 * @details Парсит строку, проверяет количество токенов, типы данных и физические диапазоны.
 * @param[in] content Сырой текст из файла truck.txt
 * @param[out] truck Объект для сохранения извлеченных массы и высоты машины
 * @param[out] startNode Переменная для записи номера начального пункта
 * @param[out] endNode Переменная для записи номера конечного пункта
 * @return Error Статус успешности парсинга (NoError, NotANumber или OutOfRange)
 */
Error parseTruckData(const QString& content, Truck& truck, int& startNode, int& endNode);

/**
 * @brief Формирует итоговый текст выходного файла в формате языка DOT
 * @details Генерирует граф, добавляет блок информации (длина или текст ошибки)
 * и динамически подсвечивает ребра маршрута красным цветом.
 * @param[in] graph Ссылка на объект исходного графа дорожной сети
 * @param[in] path Список пунктов, входящих в кратчайший маршрут
 * @param[in] totalLen Вычисленная суммарная длина пути
 * @param[in] status Финальный статус поиска (влияет на тип информационной плашки)
 * @param[in] errorText Текст обнаруженной ошибки (используется, если статус не равен PathFound)
 * @return QString Готовая валидная строка на языке DOT для визуализации графа
 */
QString generateDotRoute(const Graph& graph, const QList<int>& path, double totalLen,
                         RouteStatus status, const QString& errorText = "");

#endif // UTILS_H
