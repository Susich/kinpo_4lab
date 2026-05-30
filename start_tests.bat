@echo off
:: Включаем поддержку переменных внутри циклов и UTF-8 для кириллицы
setlocal enabledelayedexpansion
chcp 65001 > nul

set APP=lab4_sapunkov_kinpo.exe
set MAP=test_map.dot
set TRUCK=test_truck.txt
set OUT=test_result.dot
set REPORT=test_report.txt

echo ========================================================
echo   КОМПЛЕКСНОЕ ТЕСТИРОВАНИЕ
echo ========================================================
echo.

:: --- ИНИЦИАЛИЗАЦИЯ ФАЙЛА ОТЧЕТА ---
echo === ОТЧЕТ О ТЕСТИРОВАНИИ === > %REPORT%
date /t >> %REPORT%
time /t >> %REPORT%
echo. >> %REPORT%

:: Проверка наличия исполняемого файла
if not exist %APP% (
    echo [!] ОШИБКА: Файл %APP% не найден в текущей папке.
    echo [!] ОШИБКА: Файл %APP% не найден. >> %REPORT%
    pause
    exit /b
)

:: Генерация тестового графа (map.dot)
echo graph RoadMap { > %MAP%
echo 1 -- 2 [length=10.5, max_mass=20, max_height=5]; >> %MAP%
echo 2 -- 3 [length=5.0, max_mass=15, max_height=4]; >> %MAP%
echo 1 -- 3 [length=40.0, max_mass=30, max_height=6]; >> %MAP%
echo 4 -- 5 [length=10.0, max_mass=20, max_height=5]; >> %MAP%
echo } >> %MAP%

set PASSED=0
set TOTAL=0

:: --- БЛОК ЗАПУСКА СЦЕНАРИЕВ ---

:: ===== ТЕСТЫ С УСПЕШНЫМ ПРОХОЖДЕНИЕМ =====
echo 12.0 3.0 1 3 > %TRUCK%
call :run_test "Успешный маршрут 1 - 3" 0

echo 10.0 3.0 1 2 > %TRUCK%
call :run_test "Успешный маршрут 1 - 2" 0

echo 10.0 3.0 2 3 > %TRUCK%
call :run_test "Успешный маршрут 2 - 3" 0

echo 12.0 3.0 3 1 > %TRUCK%
call :run_test "Успешный маршрут 3 - 1" 0

echo 10.0 3.0 4 5 > %TRUCK%
call :run_test "Успешный маршрут 4 - 5" 0

echo 10.0 3.0 5 4 > %TRUCK%
call :run_test "Успешный маршрут 5 - 4" 0

echo 1.0 1.0 1 3 > %TRUCK%
call :run_test "Успешный маршрут 1 - 3" 0

echo 19.9 4.9 1 2 > %TRUCK%
call :run_test "Успешный маршрут 1 - 2" 0

echo 20.0 5.0 1 2 > %TRUCK%
call :run_test "Успешный маршрут 1 - 2" 0

echo 14.0 4.0 2 1 > %TRUCK%
call :run_test "Успешный маршрут 2 - 1" 0

echo 15.0 4.0 2 3 > %TRUCK%
call :run_test "Успешный маршрут 2 - 3" 0

echo 16.0 4.0 1 3 > %TRUCK%
call :run_test "Успешный маршрут 1 - 3" 0

:: ===== ТЕСТЫ ОШИБОК =====
echo 35.0 3.0 1 3 > %TRUCK%
call :run_test "Блокировка по габаритам - RouteImpossible" 1

echo 10.0 3.0 1 5 > %TRUCK%
call :run_test "Изолированные пункты - NoRouteExists" 1

echo 10kg 3.0 1 2 > %TRUCK%
call :run_test "Буквы вместо числа в файле машины" 1

echo 10.0 3.0 1 > %TRUCK%
call :run_test "Недостаток параметров в truck.txt" 1

echo 999999 3.0 1 2 > %TRUCK%
call :run_test "Выход параметров машины за рамки лимитов" 1

:: Подменяем имя карты на несуществующее
set TEMP_MAP=%MAP%
set MAP=missing_map.dot
echo 10.0 3.0 1 2 > %TRUCK%
call :run_test "Запуск с несуществующей картой дорог" 1
set MAP=%TEMP_MAP%

:: --- ИТОГИ ---
echo ========================================================
echo   ИТОГ: Успешно пройдено %PASSED% из %TOTAL% тестов.
echo   Подробный лог сохранен в файл: %REPORT%
echo ========================================================
echo.
echo === ИТОГОВОЕ ПОКРЫТИЕ === >> %REPORT%
echo Успешно пройдено %PASSED% из %TOTAL% тестов. >> %REPORT%

:: Уборка мусора (удаляем временные файлы)
del %MAP% %TRUCK% %OUT% 2>nul
pause
exit /b

:: --- ФУНКЦИЯ ПРОВЕРКИ ---
:run_test
set /a TOTAL+=1
echo --------------------------------------------------------
echo [ТЕСТ !TOTAL!] %~1
echo Входные данные:
type %TRUCK%

:: Запись заголовка теста в файл отчета
echo -------------------------------------------------------- >> %REPORT%
echo [ТЕСТ !TOTAL!] %~1 >> %REPORT%
echo Входные данные: >> %REPORT%
type %TRUCK% >> %REPORT%
echo Консольный вывод программы: >> %REPORT%

:: Запускаем программу. Весь вывод перенаправляется в REPORT
%APP% %MAP% %TRUCK% %OUT% >> %REPORT% 2>&1

:: Проверяем системный код возврата
if !ERRORLEVEL! EQU %~2 (
    echo -> Результат: [ ПРОЙДЕН ]
    echo СТАТУС: ПРОЙДЕН ^(Код завершения: !ERRORLEVEL!^) >> %REPORT%
    set /a PASSED+=1
) else (
    echo -> Результат: [ ОШИБКА ]
    echo СТАТУС: ПРОВАЛЕН ^(Ожидался код %~2, получен !ERRORLEVEL!^) >> %REPORT%
)
echo.
echo. >> %REPORT%
exit /b