#include "graph.h"
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <QQueue>
#include <QSet>

/**
 * @file graph.cpp
 * @author Сапунков А.Р.
 * @brief Реализация методов класса Graph для управления дорожной сетью.
 * @details Содержит логику парсинга графа из текстового DOT-формата
 * и реализацию алгоритма Дейкстры для поиска кратчайшего пути.
 */

/**
 * @brief Конструктор графа, совмещенный с синтаксическим парсером DOT.
 */
Graph::Graph(const QString& dotContent, QSet<Error>& errors) {
    // Проверка обязательного заголовка для неориентированного графа
    if (!dotContent.trimmed().startsWith("graph")) {
        errors.insert(Error(ErrorType::DotSyntaxError, "Ошибка: файл должен начинаться со слова «graph»."));
    }

    // Регулярное выражение для поиска строк описания ребер
    QRegularExpression edgeRegex("(\\d+)\\s*--\\s*(\\d+)\\s*\\[([^\\]]+)\\]");
    QRegularExpressionMatchIterator it = edgeRegex.globalMatch(dotContent);

    // Цикл обхода всех найденных ребер
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        int u = match.captured(1).toInt();
        int v = match.captured(2).toInt();
        QString attrsStr = match.captured(3);

        double length = -1, maxMass = -1, maxHeight = -1;
        QStringList attrs = attrsStr.split(',');

        bool hasLen = false, hasMass = false, hasHeight = false;

        for (int i = 0; i < attrs.size(); ++i) {
            QString attr = attrs[i].trimmed();
            if (attr.startsWith("length=")) {
                length = attr.mid(7).toDouble(&hasLen);
            } else if (attr.startsWith("max_mass=")) {
                maxMass = attr.mid(9).toDouble(&hasMass);
            } else if (attr.startsWith("max_height=")) {
                maxHeight = attr.mid(11).toDouble(&hasHeight);
            }
        }

        // Блок 1: Контроль наличия и формата обязательных параметров
        if (!hasLen) errors.insert(Error(ErrorType::DotSyntaxError, QString("Ошибка на участке %1--%2: пропущен обязательный атрибут «length» или это не число.").arg(u).arg(v)));
        if (!hasMass) errors.insert(Error(ErrorType::DotSyntaxError, QString("Ошибка на участке %1--%2: пропущен обязательный атрибут «max_mass» или это не число.").arg(u).arg(v)));
        if (!hasHeight) errors.insert(Error(ErrorType::DotSyntaxError, QString("Ошибка на участке %1--%2: пропущен обязательный атрибут «max_height» или это не число.").arg(u).arg(v)));

        // Блок 2: Контроль логического диапазона физических лимитов
        if (hasLen && (length < 1 || length > 100)) {
            errors.insert(Error(ErrorType::OutOfRange, QString("Ошибка на участке %1--%2: длина дороги выходит за рамки лимита (1 - 100).").arg(u).arg(v)));
        }
        if (hasMass && (maxMass < 0 || maxMass > 100000)) {
            errors.insert(Error(ErrorType::OutOfRange, QString("Ошибка на участке %1--%2: лимит массы вне диапазона (0 - 100000).").arg(u).arg(v)));
        }
        if (hasHeight && (maxHeight < 0 || maxHeight > 1000)) {
            errors.insert(Error(ErrorType::OutOfRange, QString("Ошибка на участке %1--%2: лимит высоты вне диапазона (0 - 1000).").arg(u).arg(v)));
        }

        // Блок 3: Добавление ребра (если все параметры на месте и в пределах лимитов)
        if (u != v && hasLen && hasMass && hasHeight &&
            length >= 1 && length <= 100 &&
            maxMass >= 0 && maxMass <= 100000 &&
            maxHeight >= 0 && maxHeight <= 1000) {
            addEdge(u, v, length, maxMass, maxHeight);
        }
    }
}
/**
 * @brief Метод занесения ребра в двумерную карту смежности.
 */
void Graph::addEdge(int from, int to, double len, double mMass, double mHeight) {
    // Запись ребра в оба направления для обеспечения неориентированности связи
    adjacencyMap[from][to] = Edge(len, mMass, mHeight);
    adjacencyMap[to][from] = Edge(len, mMass, mHeight);
}

/**
 * @brief Высокоуровневая функция двухпроходного поиска маршрута.
 */
RouteStatus Graph::findShortestPath(int startNode, int endNode, const Truck& truck, QList<int>& path, double& totalLen, double& requiredMass, double& requiredHeight) {
    // Первичная проверка физического присутствия стартовой и конечной точек на карте
    if (!adjacencyMap.contains(startNode) || !adjacencyMap.contains(endNode)) {
        return RouteStatus::NoRouteExists;
    }

    double pMass = 0, pHeight = 0;

    // Проход №1: Попытка найти кратчайший путь с жесткой фильтрацией по габаритам грузовика
    if (runDijkstra(startNode, endNode, path, totalLen, pMass, pHeight, &truck)) {
        return RouteStatus::PathFound;
    }

    // Проход №2: Если путь заблокирован, ищем геометрически кратчайший маршрут БЕЗ ограничений (truck = nullptr)
    if (runDijkstra(startNode, endNode, path, totalLen, requiredMass, requiredHeight, nullptr)) {
        return RouteStatus::RouteImpossible; ///< Путь есть, но машина не проходит по габаритам
    }

    // Если путь не найден даже без ограничений — вершины изолированы
    return RouteStatus::NoRouteExists;
}

/**
 * @brief Алгоритмическая реализация поиска кратчайшего пути (Алгоритм Дейкстры).
 */
bool Graph::runDijkstra(int startNode, int endNode, QList<int>& path, double& totalLen, double& pathMass, double& pathHeight, const Truck* truck) {
    QMap<int, double> distances; ///< Таблица текущих кратчайших расстояний до вершин
    QMap<int, int> previous;     ///< Таблица связей родительских узлов для восстановления пути
    QSet<int> visited;           ///< Набор посещенных (зафиксированных) вершин графа

    // Настройка начальных состояний: ставим всем вершинам расстояние "бесконечность" (-1)
    QList<int> nodes = adjacencyMap.keys();
    for (int i = 0; i < nodes.size(); ++i) {
        distances[nodes[i]] = -1;
    }
    distances[startNode] = 0; ///< Расстояние до стартовой точки равно нулю

    bool targetReached = false; ///< Управляющий флаг работы цикла (замена break)

    // Главный цикл обхода графа Дейкстры
    while (!targetReached) {
        int u = -1;              ///< Выбранный узел с минимальной дистанцией
        double minDistance = -1; ///< Значение минимального расстояния

        // Поиск среди непосещенных вершин узла с минимальным расстоянием
        for (int i = 0; i < nodes.size(); ++i) {
            int node = nodes[i];
            if (!visited.contains(node) && distances[node] != -1) {
                if (minDistance == -1 || distances[node] < minDistance) {
                    minDistance = distances[node];
                    u = node;
                }
            }
        }

        // Если доступных узлов не осталось или мы достигли финиша — останавливаемся
        if (u == -1 || u == endNode) {
            targetReached = true;
        }

        // Шаг релаксации ребер для текущего выбранного узла u
        if (!targetReached) {
            visited.insert(u); ///< Фиксируем узел как пройденный

            QMap<int, Edge> neighbors = adjacencyMap.value(u);
            QList<int> adjacentNodes = neighbors.keys();

            // Перебор всех смежных вершин (соседей) текущего узла
            for (int i = 0; i < adjacentNodes.size(); ++i) {
                int v = adjacentNodes[i];
                Edge e = neighbors.value(v);

                bool pass = true; ///< Флаг проходимости текущего ребра

                // Фильтрация по габаритам, если передан указатель на реальный грузовик
                if (truck != nullptr) {
                    if (truck->mass > e.maxMass || truck->height > e.maxHeight) {
                        pass = false; ///< Автомобиль физически заблокирован лимитами дороги
                    }
                }

                // Если ребро проходимо, выполняем релаксацию (обновление минимума расстояния)
                if (pass) {
                    double newDist = distances[u] + e.length;
                    if (distances[v] == -1 || newDist < distances[v]) {
                        distances[v] = newDist;
                        previous[v] = u; ///< Запоминаем, что в узел v пришли из узла u
                    }
                }
            }
        }
    }

    // Проверка: удалось ли проложить маршрут до конечной точки
    if (!previous.contains(endNode) && startNode != endNode) {
        return false; ///< Пути нет
    }

    // Блок обратного восстановления маршрута от финиша к старту
    path.clear();
    int curr = endNode;
    bool pathFinished = false;

    // Инициализация значений "узкого места" маршрута
    pathMass = -1;
    pathHeight = -1;

    while (!pathFinished) {
        path.prepend(curr); ///< Добавляем узел в начало итогового списка

        if (curr == startNode) {
            pathFinished = true;
        } else {
            int prev = previous[curr];
            Edge e = adjacencyMap[prev][curr];

            // Накапливаем самые жесткие лимиты массы и высоты встреченные по пути
            if (pathMass == -1 || e.maxMass < pathMass) pathMass = e.maxMass;
            if (pathHeight == -1 || e.maxHeight < pathHeight) pathHeight = e.maxHeight;

            curr = prev; ///< Сдвигаемся назад к старту
        }
    }

    totalLen = distances[endNode]; ///< Запись итоговой протяженности маршрута
    return true;
}
