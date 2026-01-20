#ifndef INTRFACE_H
#define INTRFACE_H

#define MAX_STR 10 
#define MAX_NUMBER 12

struct abonent 
{
    char name[MAX_STR];
    char second_name[MAX_STR];
    char tel[MAX_NUMBER];
    struct abonent* next;
    struct abonent* prev;
};

struct abonents
{
    struct abonent* head;
    struct abonent* tail;
    int count;
};

void display_menu();
void display_all(struct abonents *abs);
void display_all_reverse(struct abonents *abs);
void my_strncpy(char* New_abonent, const char* parametrs, int size);
struct abonent* create_abonent(const char* name, const char* second_name, const char* tel);
void add_abonent(struct abonents *abs);
void delete_abonent(struct abonents *abs);
int my_strcmp(const char* str1, const char* str2);
void search_by_name(struct abonents *abs);
void clear_directory(struct abonents *abs);
#endif
