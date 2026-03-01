#ifndef INTRFACE_H
#define INTRFACE_H

#define MAX_FILE 100              // Максимальное количество файлов в панели
#define MAX_NAME_FILE 256         // Максимальная длина имени файла
#define PANEL_WIDTH 50            // Ширина панели в символах
#define PANEL_HEIGHT 20           // Высота панели в символах
#define MAX_PATH 1024             // Максимальная длина пути      

typedef struct FileEntry          // Структура записи о файле
{
    char name[MAX_NAME_FILE];     // Имя файла
    int is_dir;                   // Флаг: 1 если директория, 0 если файл
} FileEntry;

typedef struct                    // Структура панели файлового менеджера
{
    FileEntry files[MAX_FILE];    // Массив файлов в панели
    int file_count;               // Количество файлов в панели
    int selected;                 // Индекс выбранного (активного) файла
    int top;                      // Индекс первого отображаемого файла (прокрутка)
    char current_path[MAX_PATH];  // Текущий путь панели
} Panel;

extern Panel left_panel;                 // Левая панель
extern Panel right_panel;                // Правая панель          

void scan_directory(Panel *panel, const char *path);            // Заполняет панель файлами из указанного пути
void draw_panel(Panel *panel, int x, int y, int is_active);     // Рисует панель на экране
void change_directory(Panel *panel);                            // Переходит в выбранную директорию
void move_selection(Panel *panel,int delta);                    // Перемещает выделение в панели

#endif