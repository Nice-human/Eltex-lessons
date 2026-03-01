#include "intrface.h"
#include <stdio.h>
#include <dirent.h>          // Работа с директориями (opendir, readdir)
#include <sys/stat.h>        // Информация о файлах (stat)
#include <string.h>          // Не хватает для strcpy, strcmp
#include <curses.h> 


void scan_directory(Panel *panel, const char *path)  // Заполняет панель файлами из указанного пути
{
    DIR* dirp;                 // Указатель на директорию
    struct dirent* entry;      // Структура с информацией о файле из директории
    struct stat st;            // Статус файла (информация о типе, правах и т.д.)
    char full_path[MAX_PATH];  // Полный путь к файлу

    panel->file_count = 0;     // Обнуляем счетчик файлов
    panel->selected = 0;       // Сбрасываем выбор на первый файл
    panel->top = 0;            // Сбрасываем прокрутку
    strcpy(panel->current_path, path);  // Сохраняем текущий путь

    dirp = opendir(path);      // Открываем директорию
    if (dirp == NULL)          // Проверка на ошибку открытия
    {
        printw("Error file\n"); // Выводим сообщение об ошибке
        return;
    }

    while ((entry = readdir(dirp)) != NULL)  // Читаем все записи в директории
    {
        if (strcmp(entry->d_name,".") == 0) continue; // Пропускаем текущую директорию
         
        snprintf(full_path, MAX_PATH, "%s/%s", path, entry->d_name); // Собираем полный путь
         
        if (stat(full_path, &st) == 0)        // Получаем статус файла
        {
            strcpy(panel->files[panel->file_count].name, entry->d_name); // Сохраняем имя
            panel->files[panel->file_count].is_dir = S_ISDIR(st.st_mode); // Проверка директория ли
            panel->file_count++;                // Увеличиваем счетчик файлов
        } 

        if (panel->file_count >= MAX_FILE) break; // Проверка на превышение лимита
    }
    
    closedir(dirp);           // Закрываем директорию
}