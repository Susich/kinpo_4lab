#include "utils.h"
#include "graph.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>

/**
 * @file utils.cpp
 * @author Сапунков А.Р.
 * @brief Реализация вспомогательных функций ввода-вывода и генерации отчетов.
 * @details Содержит логику чтения/записи файлов, парсинга параметров
 * грузовика из текста и формирование итогового графа в формате DOT.
 */

/**
 * @brief Функция безопасного чтения всего содержимого текстового файла.
 */
Error readFile(const QString& filename, QString& content) {
    QFile file(filename);
    // Попытка открытия файла в режиме «Только чтение»
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return Error(ErrorType::InputFileNotFound, "Ошибка: Файл «" + filename + "» не найден. Проверьте правильность пути.");
    }

    QTextStream in(&file);
    content = in.readAll(); ///< Вычитывание всего текстового потока в переменную
    file.close();

    return Error(ErrorType::NoError, "");
}

/**
 * @brief Функция записи сформированной строки в файл с фиксацией UTF-8.
 */
Error writeToFile(const QString& filename, const QString& content) {
    QFile file(filename);
    // Попытка открытия/создания файла в режиме «Только запись»
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return Error(ErrorType::OutputCreateFail, "Ошибка: Не удалось создать файл «" + filename + "». Отказано в доступе.");
    }

    QTextStream out(&file);
    out.setCodec("UTF-8"); ///< Принудительный выбор кодека для стабильного отображения кириллицы
    out << content;
    file.close();

    return Error(ErrorType::NoError, "");
}

/**
 * @brief Синтаксический анализатор текстовых данных файла характеристик автомобиля.
 */
Error parseTruckData(const QString& content, Truck& truck, int& startNode, int& endNode) {
    QString trimmed = content.trimmed();
    // Контроль пустых данных
    if (trimmed.isEmpty()) {
        return Error(ErrorType::NotANumber, "Ошибка чтения truck.txt: файл пуст. Введите массу, высоту и пункты маршрута.");
    }

    // Разбиение строки на слова (лексемы) по любым группам пробельных символов
    QStringList tokens = trimmed.split(QRegExp("\\s+"), QString::SkipEmptyParts);

    // Контроль точного количества параметров (должно быть ровно 4)
    if (tokens.size() < 4) {
        return Error(ErrorType::NotANumber, "Ошибка чтения truck.txt: ожидалось 4 параметра, а найдено " + QString::number(tokens.size()) + ".");
    }
    if (tokens.size() > 4) {
        return Error(ErrorType::NotANumber, "Ошибка чтения truck.txt: найдено больше 4 параметров. Уберите лишний текст.");
    }

    // Локальные флаги для фиксации успешности конвертации типов данных
    bool massOk, heightOk, startOk, endOk;

    // Валидация и парсинг параметра "масса авто"
    truck.mass = tokens[0].toDouble(&massOk);
    if (!massOk) return Error(ErrorType::NotANumber, "Ошибка чтения truck.txt: параметр «масса» должен быть числом. Вы ввели «" + tokens[0] + "».");
    if (truck.mass < 0 || truck.mass > 100000) return Error(ErrorType::OutOfRange, "Ошибка: масса автомобиля (" + tokens[0] + ") выходит за допустимый диапазон (0 - 100000).");

    // Валидация и парсинг параметра "высота авто"
    truck.height = tokens[1].toDouble(&heightOk);
    if (!heightOk) return Error(ErrorType::NotANumber, "Ошибка чтения truck.txt: параметр «высота» должен быть числом. Вы ввели «" + tokens[1] + "».");
    if (truck.height < 0 || truck.height > 1000) return Error(ErrorType::OutOfRange, "Ошибка: высота автомобиля (" + tokens[1] + ") выходит за допустимый диапазон (0 - 1000).");

    // Валидация и парсинг стартового пункта
    startNode = tokens[2].toInt(&startOk);
    if (!startOk) return Error(ErrorType::NotANumber, "Ошибка чтения truck.txt: номера пунктов должны быть целыми числами. Вы ввели «" + tokens[2] + "».");
    if (startNode < 0 || startNode > 100) return Error(ErrorType::OutOfRange, "Ошибка: пункт «" + tokens[2] + "» вне диапазона. Разрешены пункты от 0 до 100.");

    // Валидация и парсинг конечного пункта
    endNode = tokens[3].toInt(&endOk);
    if (!endOk) return Error(ErrorType::NotANumber, "Ошибка чтения truck.txt: номера пунктов должны быть целыми числами. Вы ввели «" + tokens[3] + "».");
    if (endNode < 0 || endNode > 100) return Error(ErrorType::OutOfRange, "Ошибка: пункт «" + tokens[3] + "» вне диапазона. Разрешены пункты от 0 до 100.");

    return Error(ErrorType::NoError, "");
}

/**
 * @brief Функция генерации выходной строки на языке описания графов DOT.
 */
QString generateDotRoute(const Graph& graph, const QList<int>& path, double totalLen, RouteStatus status, const QString& errorText) {
    QString dot = "graph RoadMap {\n";

    // Отрисовка информационной плашки в зависимости от успешности поиска
    if (status == RouteStatus::PathFound) {
        // Вывод узла с итоговой длиной пути при успехе
        dot += QString("    Total [label=\"Итоговая длина пути: %1\", shape=box, style=filled, fillcolor=lightblue];\n").arg(totalLen, 0, 'f', 2);
    } else {
        // Вывод розовой таблицы ошибки с красной каймой, если проезд невозможен/отсутствует
        QString safeErrorText = errorText;
        safeErrorText.replace("\n", "\\n"); ///< Преобразование обычного переноса в формат, понятный Graphviz
        dot += QString("    ErrorMsg [label=\"%1\", shape=box, style=filled, fillcolor=mistyrose, color=red];\n").arg(safeErrorText);
    }

    QMap<int, QMap<int, Edge>> map = graph.getAdjacencyMap();
    QList<int> nodes = map.keys();
    int firstNode = nodes.isEmpty() ? 0 : nodes.first(); ///< Защитная инициализация для пустого графа

    // Двойной итерационный цикл обхода элементов списка смежности
    for (int i = 0; i < nodes.size(); ++i) {
        int u = nodes[i];
        QMap<int, Edge> neighbors = map.value(u);
        QList<int> adjacentNodes = neighbors.keys();

        for (int j = 0; j < adjacentNodes.size(); ++j) {
            int v = adjacentNodes[j];

            // Математический фильтр u < v исключает дублирование неориентированных ребер в файле DOT
            if (u < v) {
                Edge e = neighbors.value(v);
                bool isPathEdge = false;

                // Поиск: принадлежит ли текущее ребро u--v рассчитанному массиву пути
                if (status != RouteStatus::NoRouteExists) {
                    for (int p = 0; p < path.size() - 1; ++p) {
                        if ((path[p] == u && path[p+1] == v) || (path[p] == v && path[p+1] == u)) {
                            isPathEdge = true;
                        }
                    }
                }

                // Базовое форматирование описания ребра с его характеристиками
                QString edgeStr = QString("    %1 -- %2 [label=\"length=%3, \\nmax_mass=%4, \\nmax_height=%5\"")
                                      .arg(u).arg(v).arg(e.length, 0, 'f', 2).arg(e.maxMass, 0, 'f', 1).arg(e.maxHeight, 0, 'f', 1);

                // Добавление атрибутов красной подсветки и толщины, если ребро входит в итоговый путь
                if (isPathEdge) {
                    edgeStr += ", color=red, penwidth=2.0];\n";
                } else {
                    edgeStr += "];\n";
                }
                dot += edgeStr;
            }
        }
    }

    // Связывание информационной плашки невидимой нитью [style=invis] для удержания её вверху картинки
    if (status == RouteStatus::PathFound) {
        dot += "    Total -- " + QString::number(path.isEmpty() ? firstNode : path.first()) + " [style=invis];\n";
    } else {
        dot += "    ErrorMsg -- " + QString::number(path.isEmpty() ? firstNode : path.first()) + " [style=invis];\n";
    }

    dot += "}\n";
    return dot;
}
