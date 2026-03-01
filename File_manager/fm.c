#include <stdio.h>          // Стандартный ввод-вывод
#include <stdlib.h>          // Стандартные функции (malloc, exit)
#include <unistd.h>          // Системные вызовы (getcwd)
#include <string.h>          // Работа со строками
#include <curses.h>          // Библиотека для текстового интерфейса (ncurses)
#include <locale.h>          // Локализация (для русского языка)
#include "intrface.h"        // Интерфейс

Panel left_panel;   // Определение
Panel right_panel;  // Определение

int main()
{
    setlocale(LC_ALL, "Russian");  // Устанавливаем русскую локаль для поддержки кириллицы

    int active_panel = 0;  // Активная панель (0 - левая, 1 - правая)
    int ch;  // Переменная для хранения нажатой клавиши

    initscr();          // Инициализация библиотеки ncurses
    noecho();           // Не отображать вводимые символы
    cbreak();           // Режим cbreak (немедленная передача символов)
    keypad(stdscr, TRUE);  // Включаем поддержку функциональных клавиш
    curs_set(0);        // Скрываем курсор

    // Инициализация цветов
    if (has_colors())   // Проверка поддержки цветов терминалом
    {
        start_color();  // Запуск цветного режима
        init_pair(1, COLOR_WHITE, COLOR_BLUE);    // Цвет 1: белый текст на синем фоне
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);  // Цвет 2: желтый текст на черном фоне
        init_pair(3, COLOR_CYAN, COLOR_BLUE);     // Цвет 3: голубой текст на синем фоне (для директорий)
        init_pair(4, COLOR_RED, COLOR_BLACK);     // Цвет 4: красный текст на черном фоне (активная рамка)
        init_pair(5, COLOR_WHITE, COLOR_BLACK);   // Цвет 5: белый текст на черном фоне (неактивная рамка)
    }
    else  // Если цвета не поддерживаются
    {
        endwin();  // Завершаем работу с ncurses
        printf("Терминал не поддерживает цвета\n");
        return 1;
    }

    char cwd[MAX_PATH]; // начальный путь
    getcwd(cwd, sizeof(cwd)); // получает абсолютный путь к текущей рабочей директории
    scan_directory(&left_panel, cwd);   // Сканируем текущую директорию в левую панель
    scan_directory(&right_panel, cwd);  // Сканируем текущую директорию в правую панель

    while (1)  // Бесконечный цикл обработки клавиш
    {
        clear();  // Очистка экрана
        draw_panel(&left_panel, 1, 1, active_panel == 0);   // Отрисовка левой панели
        draw_panel(&right_panel, PANEL_WIDTH + 3, 1, active_panel == 1);  // Отрисовка правой панели
        mvprintw(21, 2, "↑↓: перемещение | TAB: переключение панелей | Enter: открыть | q: выход"); // Подсказка

        // Отображение активной панели
        if (active_panel == 0) 
        {
            mvprintw(PANEL_HEIGHT + 3, 2, "Активная панель: ЛЕВАЯ");
        } else 
        {
            mvprintw(PANEL_HEIGHT + 3, 2, "Активная панель: ПРАВАЯ");
        }

        refresh();  // Обновление экрана

        ch = getch();  // Получение нажатой клавиши

        switch (ch)  // Обработка клавиш
        {
        case KEY_UP:  // Стрелка вверх
            if (active_panel == 0) {
                move_selection(&left_panel, -1);  // Движение вверх в левой панели
            } else {
                move_selection(&right_panel, -1); // Движение вверх в правой панели
            }
            break;

        case KEY_DOWN:  // Стрелка вниз
            if (active_panel == 0) {
                move_selection(&left_panel, 1);   // Движение вниз в левой панели
            } else {
                move_selection(&right_panel, 1);  // Движение вниз в правой панели
            }
            break;
        case 9:   // Табуляция
        case KEY_BTAB:  // Обратная табуляция
            active_panel = !active_panel;  // Переключение активной панели
            break;

        case 10:    // Enter (ASCII код)
        case KEY_ENTER:  // Enter (код ncurses)
            if (active_panel == 0) 
            {
                change_directory(&left_panel);  // Смена директории в левой панели
            } else {
                change_directory(&right_panel); // Смена директории в правой панели
            }
            break;

        case 'q':                 
        case 'Q':                 
            endwin();    // Завершение работы с ncurses
            return 0;    // Выход из программы
        }
        
    }

    return 0;
}