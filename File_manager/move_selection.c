#include "intrface.h"
#include <stdio.h> 

void move_selection(Panel *panel,int delta)  // Перемещает выделение в панели
{
    if(panel->file_count == 0) return;  // Выходим, если нет файлов

    panel->selected += delta;  // Изменяем индекс выделения на delta (+1 вниз, -1 вверх)

    // Проверка выхода за границы
    if(panel->selected < 0) panel->selected = 0;  // Не даем уйти выше первого
    if(panel->selected >= panel->file_count) panel->selected = panel->file_count -1; // Не даем уйти ниже последнего

    // Проверка выхода прокрутки за границы
    if(panel->selected < panel->top) panel->top = panel->selected;   // Если ушли выше видимой области
    if(panel->selected >= panel->top + PANEL_HEIGHT -2)              // Если ушли ниже видимой области
        panel->top = panel->selected - (PANEL_HEIGHT - 3);           // Сдвигаем видимую область
}