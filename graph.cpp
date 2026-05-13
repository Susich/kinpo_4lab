#include "graph.h"
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <QSet>

Graph::Graph(const QString& dotData) {
    // Регулярное выражение для поиска ребер вида: node1 -> node2 [label="dist,weight,height"]
    QRegularExpression edgeRegex("(\\w+)\\s*->\\s*(\\w+)\\s*\\[label=\"([^\"]+)\"\\]");
    QRegularExpressionMatchIterator it = edgeRegex.globalMatch(dotData);

    // В соответствии с требованиями используем флаг вместо break
    bool dataParsed = false;
    if (it.hasNext()) {
        dataParsed = true;
    }

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString u = match.captured(1);
        QString v = match.captured(2);
        QStringList params = match.captured(3).split(',');

        if (params.size() != 3) {
            throw Error(ErrorType::ParseError, "Некорректные параметры ребра (ожидается dist,weight,height): " + match.captured(0));
        }

        bool okD, okW, okH;
        Edge edge;
        edge.targetNode = v;
        edge.distance = params[0].toDouble(&okD);
        edge.maxWeight = params[1].toDouble(&okW);
        edge.maxHeight = params[2].toDouble(&okH);

        if (!okD || !okW || !okH) {
            throw Error(ErrorType::ParseError, "Ошибка числового формата в ребре: " + match.captured(0));
        }

        adjacencyList[u].append(edge);
    }

    if (!dataParsed) {
        throw Error(ErrorType::ParseError, "В DOT-файле не найдено ни одного валидного описания ребра.");
    }
}

QPair<RouteStatus, QStringList> Graph::findShortestPath(const Truck& truck) {
    // 1. Пытаемся найти путь с учетом ограничений
    QPair<double, QStringList> filteredResult = runDijkstra(truck.startNode, truck.endNode, truck.weight, truck.height);

    RouteStatus finalStatus = RouteStatus::NoRouteExists;
    QStringList finalPath;

    if (filteredResult.first >= 0) {
        finalStatus = RouteStatus::Success;
        finalPath = filteredResult.second;
    } else {
        // 2. Если не нашли, ищем вообще любой путь (без ограничений)
        QPair<double, QStringList> simpleResult = runDijkstra(truck.startNode, truck.endNode, 0, 0);

        if (simpleResult.first >= 0) {
            finalStatus = RouteStatus::RouteImpossible;
        } else {
            finalStatus = RouteStatus::NoRouteExists;
        }
    }

    return qMakePair(finalStatus, finalPath);
}

QPair<double, QStringList> Graph::runDijkstra(const QString& start, const QString& end, double weightLimit, double heightLimit) {
    QMap<QString, double> distances;
    QMap<QString, QString> predecessors;
    QSet<QString> visited;

    distances[start] = 0;

    bool targetReached = false;
    // Цикл продолжается, пока есть непосещенные узлы и цель не достигнута
    while (!targetReached) {
        QString current;
        double minDistance = -1;

        // Поиск узла с минимальным расстоянием (вместо break используем логику выбора)
        QMapIterator<QString, double> i(distances);
        while (i.hasNext()) {
            i.next();
            if (!visited.contains(i.key()) && (minDistance < 0 || i.value() < minDistance)) {
                minDistance = i.value();
                current = i.key();
            }
        }

        // Если не нашли подходящий узел или дошли до конца
        if (current.isEmpty() || current == end) {
            targetReached = true;
        }

        if (!targetReached) {
            visited.insert(current);

            // Релаксация ребер
            QList<Edge> edges = adjacencyList.value(current);
            for (int j = 0; j < edges.size(); ++j) {
                Edge e = edges[j];

                // Проверка габаритов (если лимиты заданы)
                bool passWeight = (weightLimit <= 0 || e.maxWeight >= weightLimit);
                bool passHeight = (heightLimit <= 0 || e.maxHeight >= heightLimit);

                if (passWeight && passHeight) {
                    double newDist = distances[current] + e.distance;
                    if (!distances.contains(e.targetNode) || newDist < distances[e.targetNode]) {
                        distances[e.targetNode] = newDist;
                        predecessors[e.targetNode] = current;
                    }
                }
            }
        }
    }

    // Сборка пути
    QStringList path;
    double totalDist = -1;
    if (distances.contains(end)) {
        totalDist = distances[end];
        QString step = end;
        while (!step.isEmpty()) {
            path.prepend(step);
            step = predecessors.value(step);
        }
    }

    return qMakePair(totalDist, path);
}
