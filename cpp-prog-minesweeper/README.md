# Лабораторная работа №3. Сапёр (Qt)

Текущий статус тестирования GitHub Actions: [![CI/CD](../../actions/workflows/classroom.yml/badge.svg?branch=main&event=workflow_dispatch)](../../actions/workflows/classroom.yml).

## Структура проекта

Проект лабораторной работы состоит из **двух** частей: *проект* и набор *тестов* (GoogleTest). Проект состоит из заголовочных файлов и файлов с исходным кодом, включая [`main.cpp`](src/main.cpp), содержащий точку входа программы. Unit-тесты для тестирования проекта располагаются в файлах в директории [`test/`](test/), точка входа для запускат тестов располагается в файле [`tests.cpp`](test/tests.cpp).

| Директория             | Назначение                                                   |
| ---------------------- | ------------------------------------------------------------ |
| [`include/`](include/) | общие заголовочные файлы проекта (API или интерфейс проекта) |
| [`src/`](src/)         | исходные файлы проекта                                       |
| [`test/`](test/)       | вспомогательные и исходные файлы с тестами                   |

Вам необходимо настроить [QMake](https://doc.qt.io/qt-5/qmake-manual.html) [проект](minesweeper.pro).

*Примечание*. Файл [ресурсов](https://doc.qt.io/qt-5/resources.html) следует размещать в *корне* репозитория.

## Конфигурация и сборка проекта

[Конфигурация](https://doc.qt.io/qt-5/qmake-variable-reference.html#config) выбранной цели на сборку (тесты или основное решение).

```cmd
qmake minesweeper.pro CONFIG+=<target> GTEST_ROOT=<path/to/googletest>
```

где `target` - это либо `solution` (GUI), либо `tests` (тесты), `GTEST_ROOT` (необязателен в `solution`) указывает на *корень* динамической библиотеки GoogleTest:

* заголовочные файлы лежат в `<path/to/googletest>/include`;
* библиотека лежит в `<path/to/googletest>/lib`;
* (*Windows-only*) динамическая библиотека лежит в `<path/to/googletest>/bin`.

На Windows по умолчанию генерируются *Makefile*'ы на Release и Debug сборки. Сборка и запуск:

```cmd
nmake -f Makefile.Debug
.\debug\minesweeper.exe
REM Debug

nmake -f Makefile.Release
.\release\minesweeper.exe
REM Release
```

На Unix (Linux/MacOS) в конфигурации выбранной цели необходимо добавить `CONFIG+=release`, если это - Release-сборка, или `CONFIG+=debug` - Debug. Сборка и запуск:

```bash
make
./minesweeper
```
