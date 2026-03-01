#include "intrface.h"
#include <stdio.h> 
#include <string.h>


void change_directory(Panel *panel)  // Переходит в выбранную директорию
{
    if (panel->file_count <= 0 || !panel->files[panel->selected].is_dir)
        return;  // Выходим, если нет файлов или выбранный элемент не директория
    
    char new_path[MAX_PATH];  // Буфер для нового пути
    
    if (strcmp(panel->files[panel->selected].name, "..") == 0) // Если выбрана родительская директория
    {
        // Более безопасный способ получить родительский путь
        char *last_slash = strrchr(panel->current_path, '/');  // Ищем последний слэш в пути
        
        if (last_slash == NULL) 
        {
            // Нет слеша - текущая директория
            strcpy(new_path, "..");
        }
        else if (last_slash == panel->current_path) 
        {
            // Корневая директория
            strcpy(new_path, "/");
        }
        else 
        {
            // Копируем всё до последнего слеша
            size_t len = last_slash - panel->current_path;
            strncpy(new_path, panel->current_path, len);
            new_path[len] = '\0';
        }
    }
    else  // Обычная директория
    {
        snprintf(new_path, MAX_PATH, "%s/%s", panel->current_path, 
                 panel->files[panel->selected].name);  // Формируем новый путь
    }
    
    scan_directory(panel, new_path);  // Сканируем новую директорию
}