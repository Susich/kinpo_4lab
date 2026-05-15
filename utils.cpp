#include "utils.h"
#include "graph.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>

Error readFile(const QString& filename, QString& content) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Формируем ошибку согласно Таблице 1
        return Error(ErrorType::InputFileNotFound, "Ошибка: Файл «" + filename + "» не найден. Проверьте правильность пути.");
    }

    QTextStream in(&file);
    content = in.readAll();
    file.close();

    return Error(ErrorType::NoError, "");
}

Error writeToFile(const QString& filename, const QString& content) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return Error(ErrorType::OutputCreateFail, "Ошибка: Не удалось создать файл «" + filename + "». Отказано в доступе.");
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << content;
    file.close();

    return Error(ErrorType::NoError, "");
}

Error parseTruckData(const QString& content, Truck& truck, int& startNode, int& endNode) {
    QString trimmed = content.trimmed();
    if (trimmed.isEmpty()) {
        return Error(ErrorType::NotANumber, "Ошибка чтения truck.txt: файл пуст. Введите массу, высоту и пункты маршрута.");
    }

    // Разбиваем строку по любым пробельным символам
    QStringList tokens = trimmed.split(QRegExp("\\s+"), QString::SkipEmptyParts);

    if (tokens.size() < 4) {
        return Error(ErrorType::NotANumber, "Ошибка чтения truck.txt: ожидалось 4 параметра, а найдено " + QString::number(tokens.size()) + ".");
    }
    if (tokens.size() > 4) {
        return Error(ErrorType::NotANumber, "Ошибка чтения truck.txt: найдено больше 4 параметров. Уберите лишний текст.");
    }

    bool massOk, heightOk, startOk, endOk;

    truck.mass = tokens[0].toDouble(&massOk);
    if (!massOk) return Error(ErrorType::NotANumber, "Ошибка чтения truck.txt: параметр «масса» должен быть числом. Вы ввели «" + tokens[0] + "».");
    if (truck.mass < 0 || truck.mass > 100000) return Error(ErrorType::OutOfRange, "Ошибка: масса автомобиля (" + tokens[0] + ") выходит за допустимый диапазон (0 - 100000).");

    truck.height = tokens[1].toDouble(&heightOk);
    if (!heightOk) return Error(ErrorType::NotANumber, "Ошибка чтения truck.txt: параметр «высота» должен быть числом. Вы ввели «" + tokens[1] + "».");
    if (truck.height < 0 || truck.height > 1000) return Error(ErrorType::OutOfRange, "Ошибка: высота автомобиля (" + tokens[1] + ") выходит за допустимый диапазон (0 - 1000).");

    startNode = tokens[2].toInt(&startOk);
    if (!startOk) return Error(ErrorType::NotANumber, "Ошибка чтения truck.txt: номера пунктов должны быть целыми числами. Вы ввели «" + tokens[2] + "».");
    if (startNode < 0 || startNode > 100) return Error(ErrorType::OutOfRange, "Ошибка: пункт «" + tokens[2] + "» вне диапазона. Разрешены пункты от 0 до 100.");

    endNode = tokens[3].toInt(&endOk);
    if (!endOk) return Error(ErrorType::NotANumber, "Ошибка чтения truck.txt: номера пунктов должны быть целыми числами. Вы ввели «" + tokens[3] + "».");
    if (endNode < 0 || endNode > 100) return Error(ErrorType::OutOfRange, "Ошибка: пункт «" + tokens[3] + "» вне диапазона. Разрешены пункты от 0 до 100.");

    return Error(ErrorType::NoError, "");
}

QString generateDotRoute(const Graph& graph, const QList<int>& path, double totalLen) {
    QString dot = "graph RoadMap {\n";
    dot += QString("    Total [label=\"Итоговая длина пути: %1\", shape=box, style=filled, fillcolor=lightblue];\n").arg(totalLen, 0, 'f', 2);

    QMap<int, QMap<int, Edge>> map = graph.getAdjacencyMap();
    QList<int> nodes = map.keys();

    // Флаг для предотвращения использования continue/break
    for (int i = 0; i < nodes.size(); ++i) {
        int u = nodes[i];
        QMap<int, Edge> neighbors = map.value(u);
        QList<int> adjacentNodes = neighbors.keys();

        for (int j = 0; j < adjacentNodes.size(); ++j) {
            int v = adjacentNodes[j];

            // Чтобы не дублировать неориентированные ребра (1--2 и 2--1), пишем только если u < v
            if (u < v) {
                Edge e = neighbors.value(v);

                // Проверяем, принадлежит ли ребро найденному пути
                bool isPathEdge = false;
                for (int p = 0; p < path.size() - 1; ++p) {
                    if ((path[p] == u && path[p+1] == v) || (path[p] == v && path[p+1] == u)) {
                        isPathEdge = true;
                    }
                }

                QString edgeStr = QString("    %1 -- %2 [label=\"length=%3, \\nmax_mass=%4, \\nmax_height=%5\"").arg(u).arg(v).arg(e.length, 0, 'f', 2).arg(e.maxMass, 0, 'f', 1).arg(e.maxHeight, 0, 'f', 1);

                if (isPathEdge) {
                    edgeStr += ", color=red, penwidth=2.0];\n";
                } else {
                    edgeStr += "];\n";
                }
                dot += edgeStr;
            }
        }
    }

    dot += "    Total -- " + QString::number(path.isEmpty() ? 0 : path.first()) + " [style=invis];\n";
    dot += "}\n";
    return dot;
}
