#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

// утилиты

// очистка буфера ввода
void clearBuffer() {  // Итог сложность: 5n + 1
    int c; 
    while ((c = getchar()) != '\n' && c != EOF);  // (5) * n + (1)
}


// безопасный ввод числа в диапазоне
int getValidIntInRange(const char* prompt, int min, int max) { // Итог сложность: n * (7 + 5n) = 5n^2 + 7n
    int value;
    while (1) {  // n
        printf("%s", prompt); // 1
        if (scanf("%d", &value) != 1) {  // 3
            clearBuffer(); // 5n + 1 + 1
            printf(">> ОШИБКА: Ожидается целое число!\n"); // 1
        }
        else if (value < min || value > max) { // 3
            printf(">> ОШИБКА: Число должно быть в диапазоне [%d, %d]!\n", min, max); // 1
        }
        else {
            clearBuffer(); // 5n + 1 + 1
            return value; // 1
        }
    }
}

// ввод строки
void getStringInput(const char* prompt, char* buffer, int max_len) { // Итог сложность: 2n + 6
    printf("%s", prompt); // 1
    if (fgets(buffer, max_len, stdin) != NULL) { // n + 1 + 1 
        buffer[strcspn(buffer, "\n")] = '\0'; // n + 1 + 1 + 1 
    }
}

// заполнение

// чтение из файла (поддерживает динамическое расширение)
bool fillArrayFromFile(int **arr, int *size, int *capacity, const char* filename, bool allowResize) { 
    // Итог сложность: 10 + 2n* (n+26) = 2n^2 + 52n + 11
    FILE *f = fopen(filename, "r"); // 2
    if (!f) { // 1
        printf(">> ОШИБКА: Невозможно открыть файл '%s'.\n", filename); // 1
        return false; // 1
    }
    int count = 0; // 1
    int num;

    while (fscanf(f, "%d", &num) == 1) { // n*2 
        // если достигли лимита
        if (count >= *capacity) { // 2
            if (!allowResize) { // 1
                printf(">> ОШИБКА: превышен размер массива (%d)\n", *capacity); // 2
                fclose(f); // 1 
                return false; // 1
            }
            // увеличиваем массив
            *capacity *= 2; // 3 
            int *temp = realloc(*arr, (*capacity) * sizeof(int)); // n + 1 + 1 + 1 + 1 + 1 = n + 5
            if (!temp) { // 1 
                printf("Ошибка realloc\n"); // 1 
                fclose(f); // 1
                return false; // 1 
            }
            *arr = temp; // 2 
        }
        (*arr)[count++] = num; // 1 + 1 + 1 + 2 = 5 
    }
    fclose(f); // 1
    *size = count; // 2

    printf(">> Файл считан. Элементов: %d\n", count); // 1 
    return true; // 1
}

// ручной ввод (одной строкой)
bool fillArrayManualAuto(int **arr, int *size, int *capacity, bool isDynamic) { 
    // Итог сложность: 2n + 6 + n * (n+3 + 2 + 1 + 1 + 3 + 1 + 1 + 3 + 1 + 2 + 1 + 2n + 21) = 3n^2 + 42n + 7 
    char buffer[10000];

    printf("Введите числа через пробел:\n"); // 1

    if (!fgets(buffer, sizeof(buffer), stdin)) { // n + 1 + 1 = n + 2
        return false; // 1
    }

    char *token = strtok(buffer, " \t\n"); // n + 1 + 1 = n + 2 

    while (token != NULL) {  // n*1

        char *end;
        long val = strtol(token, &end, 10); // 2 + n + 1 = n + 3 

        // проверка: это число?
        if (*end != '\0') { // 2 
            printf("Ошибка: '%s' не число\n", token); // 1 
            return false; // 1 
        }

        // проверка диапазона int
        if (val < INT_MIN || val > INT_MAX) { // 3 
            printf("Ошибка: '%s' выходит за диапазон int\n", token); // 1 
            return false; // 1 
        }

        // проверка на переполнение массива
        if (*size >= *capacity) { // 3 

            if (!isDynamic) { // 1 
                printf(">> Превышен лимит (%d)\n", *capacity); // 2 
                return false; // 1 
            }

            *capacity *= 2; // 3 

            int *temp = realloc(*arr, (*capacity) * sizeof(int)); // n + 1 + 1 + 1 + 1 + 1 = n + 5 
            if (!temp) { // 1 
                printf("Ошибка realloc\n"); // 1 
                return false; // 1 
            }

            *arr = temp; // 2
        }

        (*arr)[(*size)++] = (int)val; // 1 + 1 + 4 + 1 + 1 = 8 

        token = strtok(NULL, " \t\n"); // n + 1
    }

    return true; // 1
}

// случайное заполнение
void fillArrayRandom(int arr[], int size) { // Итог сложность: 7n + 2
    for (int i = 0; i < size; i++) { // 1 + (n + 1) + 2n 
        arr[i] = rand() % 1000; // 4
    }
}

// вывод

void printArray(const int arr[], int size, const char* message) { // Итог сложность: 7 + 5n
    if (size > 30) { // 1
        printf("%s [Размер массива (%d) слишком велик для вывода]\n", message, size); // 1 
        return; // 1
    }

    printf("%s ", message); // 1 

    for (int i = 0; i < size; i++) { // 1 + 2(n + 1) + 2n 
        printf("%d ", arr[i]); // 1 + 1 = 2 
    }

    printf("\n"); // 1 
}

void saveArrayToFile(const int arr[], int size, const char* filename) { 
    // Итог сложность: 6  + 5n + 3 = 5n + 9  
    FILE *f = fopen(filename, "w"); // 2 

    if (!f) { // 1 
        printf(">> ОШИБКА: не удалось создать файл '%s'\n", filename); // 1 
        return; // 1
    }

    for (int i = 0; i < size; i++) { // 1 + (n + 1) + 2n 
        fprintf(f, "%d ", arr[i]); // 2
    }

    fclose(f); // 1 
    printf(">> Массив сохранён в файл '%s'\n", filename); // 1 
}

// алгоритмы

// проверка отсортирован ли массив
int isSorted(int arr[], int n) { // Итог сложность: 1 + 1 + 3n -2 + 5(n-1) = 8n - 5
    for (int i = 1; i < n; i++) { // 1 + n + 2(n-1) 
        if (arr[i - 1] > arr[i]) { // 4
            return 0; // 1 
        }
    }
    return 1; // 1 
}

// сортировка выбором
void selectionSort(int arr[], int n) {
    // Итог сложность: 1 + 3n - 2 + n + n*(2 + n + 2(n-1) + 4n) + 8n = 7n^2 + 12n - 1  
    for (int i = 0; i < n - 1; i++) { // 1 + (n) + 2(n-1)

        int min_idx = i; // 1 

        for (int j = i + 1; j < n; j++) { // (2) + (n - i)(в худшем случае n) + 2(n-1) (в худшем случае) 
            if (arr[j] < arr[min_idx]) { // 3 
                min_idx = j; // 1 
            }
        }

        if (min_idx != i) { // 1
            int temp = arr[min_idx]; // 2
            arr[min_idx] = arr[i]; // 3
            arr[i] = temp; // 2
        }
    }
}

// интерполяционный поиск
int interpolationSearch(int arr[], int n, int target) {
// Итог сложность: 39n + 6
    if (n <= 0) return -1; // 2 

    int low = 0; // 1 
    int high = n - 1; // 2 

    while (low <= high && target >= arr[low] && target <= arr[high]) { // 7

        if (arr[low] == arr[high]) { // 3 
            if (arr[low] == target) // 2
                return low; // 1 
            else
                return -1; // 1 
        }

        double part = (double)(target - arr[low]) / (arr[high] - arr[low]); // 8

        int pos = low + (int)(part * (high - low)); // 5 

        if (pos < 0 || pos >= n) // 3
            break; // 1 

        if (arr[pos] == target) // 2
            return pos; // 1 

        if (arr[pos] < target) // 2
            low = pos + 1; // 2
        else
            high = pos - 1; // 2 
    }

    return -1; // 1 
}

// главная функция

int main() {
    srand(time(NULL)); // 2 

    int *arr = NULL; // 1  
    int staticArr[1000]; 

    int size = 0; // 1 
    int capacity = 0; // 1 

    int type, fType;

    int target, idx;

    char name[256];
    bool ok = false; // 1 

    struct timespec start, end;
    double time_spent;

    printf("====================================================\n"); // 1 
    printf("   ПРОГРАММА СОРТИРОВКИ И ИНТЕРПОЛЯЦИОННОГО ПОИСКА\n"); // 1  
    printf("====================================================\n"); // 1 
// Итог по блоку: 9 

    // выбор памяти
    type = getValidIntInRange("Тип памяти (1-Статический, 2-Динамический): ", 1, 2); // 5n^2 + 7n + 2 

    if (type == 1) { // 1 
        arr = staticArr; // 1 
        capacity = 1000; // 1 
    } else {
        capacity = 10; // 1 
        arr = malloc(capacity * sizeof(int));  // 4 

        if (!arr) { // 1 
            printf("Ошибка памяти\n"); // 1 
            return 1; // 1 
        }
    }

// Итог по блоку:  5n^2 + 7n + 2  + 8 = 5n^2 + 7n + 10 

    // выбор способа заполнения
    fType = getValidIntInRange("Способ (1-Ручной, 2-Рандом, 3-Файл): ", 1, 3); // 5n^2 + 7n + 2 

    if (fType == 1) { // 1

        ok = fillArrayManualAuto(&arr, &size, &capacity, type == 2); 
        // 1 + 1 + 1 + 1 + 1 + 1 + 3n^2 + 42n + 7 = 3n^2 + 42n + 13

    } else if (fType == 2) { // 1 
        long long temp; 
        printf("Сколько чисел сгенерировать: "); // 1 
        if (scanf("%lld", &temp) != 1) { // 3
            clearBuffer(); // 5n + 2
            printf("Ошибка ввода\n"); // 1 
            return 1; // 1
        }
        clearBuffer(); // 5n + 2
        if (temp <= 0 || temp > INT_MAX) { // 3 
            printf("Ошибка диапазона\n"); // 1 
            return 1; // 1 
        }
        int n = (int)temp; // 2 
        if (type == 1 && n > capacity) { // 3 
            printf("Превышен лимит (%d)\n", capacity); // 1 
            return 1; // 1 
        }
        if (type == 2 && n > capacity) { // 3 
            int *tempArr = realloc(arr, n * sizeof(int)); // 1 + 1 + 1 = 3 
            if (!tempArr) { // 1 
                printf("Ошибка realloc\n"); // 1 
                return 1; // 1 
            }
            arr = tempArr; // 1 
            capacity = n; // 1 
        }
        fillArrayRandom(arr, n); // 7n + 3
        size = n; // 1
    }
    else {

        while (!ok) { // 1  (от k вводов)
            getStringInput("Имя файла: ", name, 256); // 1 

            ok = fillArrayFromFile(&arr, &size, &capacity, name, type == 2); // 2n^2 + 52n + 17
        }
    }

// Итог по блоку: 5n^2 + 7n + 2 + 3n^2 + 42n + 13 + 1 = 8n^2 + 49n + 16
    
    // защита от пустого массива
    if (size == 0) { // 1 
        printf("Массив пуст\n"); // 1 
        if (type == 2) free(arr); // 2 
        return 1; // 1 
    }
    printArray(arr, size, "\nИсходный:"); // 8 + 5n
    if (size > 30) { // 1 
        char ch;

        printf("Массив большой. Сохранить в файл input.txt? (y/n): "); // 1 

        if (scanf(" %c", &ch) == 1) { // 3
            if (ch == 'y' || ch == 'Y') { // 3
                saveArrayToFile(arr, size, "input.txt"); // 5n + 10
            }
        }

        clearBuffer(); // 5n + 2
    }    

// Итог по блоку: 8 + 5n + 8 + 5n + 10 + 5n + 2 = 15n + 28 

    // сортировка
    clock_gettime(CLOCK_MONOTONIC, &start); // 2 
    selectionSort(arr, size); // 7n^2 + 12n
    clock_gettime(CLOCK_MONOTONIC, &end); // 2 

    time_spent =(double)(end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0; // 7

// Итог по блоку: = 7n^2 + 12n + 11 
    
    printArray(arr, size, "Отсортированный:"); // 8 + 5n
    if (size > 30) { // 1
        char ch;

        printf("Сохранить отсортированный массив в sorted.txt? (y/n): "); // 1 

        if (scanf(" %c", &ch) == 1) { // 3
            if (ch == 'y' || ch == 'Y') { // 3
                saveArrayToFile(arr, size, "sorted.txt"); // 5n + 10
            }
        }

        clearBuffer(); // 5n + 2
    }
    printf("Время сортировки: %.6f мс\n", time_spent); // 1 

    if (!isSorted(arr, size)) { // 1 + 1 + 8n - 5 = 8n - 3
        printf("Ошибка сортировки\n"); // 1 
        if (type == 2) free(arr); // 2 
        return 1; // 1 
    }

// Итог по блоку: 8 + 5n + 5n + 10 + 5n + 2 + 8n - 3 + 13 = 23n + 30 
    
    // поиск
    printf("\nРежим поиска\n"); // 1 

    while (1) { // 1 (k вводов)
        printf("Введите число (при вводе буквы, программа завершит свою работу): "); // 1 

        if (scanf("%d", &target) != 1) { // 3
            clearBuffer(); // 5n + 2
            break; // 1 
        }

        clock_gettime(CLOCK_MONOTONIC, &start); // 2 
        idx = interpolationSearch(arr, size, target); // 1 + 39n + 6 + 1 = 39n + 8
        clock_gettime(CLOCK_MONOTONIC, &end); // 2 

        time_spent = (double)(end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0; // 7

        if (idx != -1) // 1 
            printf("Найдено: индекс %d (позиция %d)\n", idx, idx+1); // 2 
        else
            printf("Не найдено\n"); // 1 

        printf("Время поиска: %.6f мс\n\n", time_spent); // 1 
    }

// Итог по блоку: k *(39n + 28) + 5n + 9 

    if (type == 2) { // 1 
        free(arr); // 1 
        arr = NULL; // (1)
        printf(">> Память очищена успешно.\n"); // (1)
    }

    return 0; // 1 
}

// Итог по блоку: 5

// Суммируем и получаем итоговый полином: 20n^2 + 111n + 114 + k(39n+28)
// Итоговая сложность: О(n^2)