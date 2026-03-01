#include "intrface.h"
#include <stdio.h>
#include <curses.h>
#include <string.h>

void draw_panel(Panel *panel, int x, int y, int is_active) // Рисует панель на экране
{
    // Выбор цвета рамки в зависимости от активности панели
    if (is_active) {
        attron(COLOR_PAIR(2));  // Яркая рамка для активной панели
    } else {
        attron(COLOR_PAIR(5));  // Обычная рамка для неактивной панели
    }

    mvhline(y, x, ACS_HLINE, PANEL_WIDTH);                      // Верхняя горизонтальная линия
    mvhline(y + PANEL_HEIGHT -1, x, ACS_HLINE, PANEL_WIDTH);    // Нижняя горизонтальная линия
    mvvline(y, x, ACS_VLINE, PANEL_HEIGHT);                     // Левая вертикальная линия
    mvvline(y, x + PANEL_WIDTH -1, ACS_VLINE, PANEL_HEIGHT);    // Правая вертикальная линия
    
    mvaddch(y, x, ACS_ULCORNER);                                       // Верхний левый угол
    mvaddch(y, x + PANEL_WIDTH - 1, ACS_URCORNER);                     // Верхний правый угол
    mvaddch(y + PANEL_HEIGHT - 1, x, ACS_LLCORNER);                    // Нижний левый угол
    mvaddch(y + PANEL_HEIGHT - 1, x + PANEL_WIDTH - 1, ACS_LRCORNER);  // Нижний правый угол

    // Отключаем цвет рамки
    if (is_active) {
        attroff(COLOR_PAIR(2));
    } else {
        attroff(COLOR_PAIR(5));
    }

    // Заголовок панели с путем (обрезаем если слишком длинный)
    char header[MAX_PATH];
    if (strlen(panel->current_path) > PANEL_WIDTH - 4) 
    {
        snprintf(header, sizeof(header), " ...%s", 
                panel->current_path + strlen(panel->current_path) - (PANEL_WIDTH - 7));
    } else 
    {
        snprintf(header, sizeof(header), " %s ", panel->current_path);
    }
    mvprintw(0, x + 2, "%s", header); // Выводим заголовок

    // Включаем синий фон для внутренней области
    attron(COLOR_PAIR(1));
    
    // Закрашиваем внутреннюю область панели синим
    for (int i = 1; i < PANEL_HEIGHT - 1; i++)
    {
        for (int j = 1; j < PANEL_WIDTH - 1; j++)
        {
            mvaddch(y + i, x + j, ' ');
        }
    }

    // Рисуем рамку поверх синего фона
    attroff(COLOR_PAIR(1));

    // Отображаем файлы в панели
    for (int i = 0; i < PANEL_HEIGHT - 2; i++)
    {
        int file_index = i + panel->top;
        if(file_index >= panel->file_count) break;

        int line_y = y + i + 1;

        attron(COLOR_PAIR(1));

        if (file_index == panel->selected && is_active)
        {
            attron(A_REVERSE);  // Включаем инверсию для выделенного файла
        }

        if (panel->files[file_index].is_dir)
        {
            mvaddch(line_y, x + 2, '/');  // Пометка директории слэшем
        }
        else
        {
            mvaddch(line_y, x + 2, ' ');
        }

        char short_name[30];                // Буфер для короткого имени
        strncpy(short_name, panel->files[file_index].name, 28); // Копируем первые 28 символов
        short_name[28] = '\0';               // Гарантированно завершаем строку
        mvprintw(line_y, x + 4, "%-28s", short_name); // Выводим имя файла с выравниванием влево
        
        if (file_index == panel->selected && is_active)
        {
             attroff(A_REVERSE);  // Выключаем инверсию
        }
        attroff(COLOR_PAIR(1));
    }   
}