#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QList>
#include "datatypes.h"

/**
 * @file utils.h
 * @author Сапунков А.Р.
 * @brief Вспомогательные функции согласно Внутренней спецификации.
 */

Error readFile(const QString& filename, QString& content);
Error writeToFile(const QString& filename, const QString& content);
Error parseTruckData(const QString& content, Truck& truck, int& startNode, int& endNode);
QString generateDotRoute(const Graph& graph, const QList<int>& path, double totalLen);

#endif // UTILS_H
