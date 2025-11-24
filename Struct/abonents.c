#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXABONENT 10 
#define MAX_LENGTH 10

struct abonent
{
    char name[MAX_LENGTH];
    char second_name[MAX_LENGTH];
    char tel[MAX_LENGTH];
};

typedef struct abonent abonent; 
abonent abonents[MAXABONENT];

void Clear_abonent(abonent *ab)
{
    for (int i = 0; i < MAX_LENGTH; i++)
    {
        ab->name[i] = '\0';
        ab->second_name[i] = '\0';
        ab->tel[i] = '\0';
    }    
}

void Add_abonent(int* count)
{ 
    if (*count >= MAXABONENT)
    {
        printf("Ошибка справочник переполнин! Максимум %d абонентов \n\n", MAXABONENT);
        return;
    }
    
    printf("\n=== Добавление нового абонента ===\n");

    printf("Введите имя (макс. %d символов): ", MAX_LENGTH);
    scanf("%s", abonents[*count].name);
     
    printf("\n");

    printf("Введите фамилию (макс. %d символов): ", MAX_LENGTH);
    scanf("%s", abonents[*count].second_name);
    printf("\n");

    printf("Введите телефон без +7 (макс. %d символов): ", MAX_LENGTH);
    scanf("%s", abonents[*count].tel);
    printf("\n");

    (*count)++;
    printf("Абонент успешно добавлен! \n");
}

void Delet_abonent(int* count)
{
    printf("\n== Удаление абонента ==\n");

    printf("Выберите абонента для удаления (от 1 до %d) ", *count);

    int index;
    scanf("%d", &index);
    
    if (index < 1 || index > *count)
    {
        printf("Введите корректное число (от 1 до %d) \n", *count);
        return;
    }
    
    index--;
    
    Clear_abonent(&abonents[index]);
    
    for (int i = index; i < *count - 1; i++) 
    {
        for (int j = 0; j < MAX_LENGTH - 1; j++)
        {
            abonents[i].name[j] = abonents[i + 1].name[j];
            abonents[i].second_name[j] = abonents[i + 1].second_name[j];
            abonents[i].tel[j] = abonents[i + 1].tel[j];
        }     
    }
    
    Clear_abonent(&abonents[*count - 1]);
    (*count)--;
    
    printf("\nАбонент успешно удален!\n");
}

int Str_EQ(const char* str, const char* str2)
{
    if (str == NULL || str2 == NULL )
    {
        return 0;
    }

    for (char i = 0; str[i] != '\0' || str2[i] != '\0'; i++)
    {
        if (str[i] != str2[i])
        {
            return 0;
        }
    }

    return 1;
}

void Search_abonents(int count) // count - это сколько игроков добавлено
{
    printf("\n== Поиск абонентов ==\n");

    printf("Выберите абонента по имени: ");

    char search_name[MAX_LENGTH];
    scanf("%s",search_name);

    int value = 0;
    for (int i = 0; i < count; i++) 
    {
        if (Str_EQ(abonents[i].name, search_name) == 0)
        {
            continue;
        }
        else
        {
            printf("%-2d | %-8s | %-8s | %-8s\n", 
                i + 1, 
                abonents[i].name, 
                abonents[i].second_name, 
                abonents[i].tel);
            value++;
        }
    }

    if (value == 0) 
    {
        printf("Абоненты с именем '%s' не найдены.\n\n", search_name);
    } 
    else 
    {
        printf("Найдено абонентов: %d\n\n", value);
    }
}

void Print_abonent(int count)
{
    if (count == 0) 
    {
        printf("Справочник пуст!\n");
        return;
    }

    printf("=== Список всех абонентов от (%d до %d) === \n", count, MAXABONENT);
    printf("-------------------------------------------------\n");
    printf("№  | Имя      | Фамилия  | Телефон  \n");
    printf("-------------------------------------------------\n");

    for (int i = 0; i < count; i++) 
    {
        printf("%-2d | %-8s | %-8s | %-8s\n", 
               i + 1, 
               abonents[i].name, 
               abonents[i].second_name, 
               abonents[i].tel);
    }
    
    printf("-------------------------------------------------\n");
}

int Display_menu ()
{
    printf("\n=== АБОНЕНТСКИЙ СПРАВОЧНИК ===\n");
    printf("1) Добавить абонента \n");
    printf("2) Удалить абонента \n");
    printf("3) Поиск абонентов по имени \n");
    printf("4) Вывод всех записей \n");
    printf("5) Выход \n\n"); 
}

void Interface()
{
    static int count = 0;
    int number;

    while (1)
    { 
        printf("\nВыберите пункт меню: ");

        scanf("%d", &number);

        if (number < 1 || number > 5)
        {
            number = 5;
        }
        
        switch (number)
        {
        case 1:
            Add_abonent(&count);
            break; 
        case 2:
            Delet_abonent(&count);
            break;     
        case 3:
            Search_abonents(count);
            break;     
        case 4:
            Print_abonent(count);
            break;     
        case 5:
            printf("Выход из программы!\n");
            return;
            
        default:
        printf("Ошибка: неверный пункт меню! Введите число от 1 до 5.\n");
            break;
        }
    }
}

int main (void)
{
    for (int i = 0; i < MAXABONENT; i++)
    {
        Clear_abonent(&abonents[i]);
    }
    
    Display_menu();
    Interface();  

    return 0;
}