#include <windows.h>
#include <vector>
#include <string>

int main() {
    // 1. Получаем хэндл стандартного вывода
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return 1;

    // Скрываем курсор для чистоты анимации
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    const int WIDTH = 20;
    const int HEIGHT = 10;

    // Создаем буфер экрана (20x10)
    std::vector<CHAR_INFO> buffer(WIDTH * HEIGHT);

    // Инициализация буфера пробелами (черный фон)
    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        buffer[i].Char.UnicodeChar = L' ';
        buffer[i].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // Белый на черном
    }

    // Область вывода (координаты в консоли, начиная с 0,0)
    SMALL_RECT writeRegion = { 0, 0, WIDTH - 1, HEIGHT - 1 };
    COORD bufferSize = { WIDTH, HEIGHT };
    COORD bufferCoord = { 0, 0 };

    // 2. Цикл анимации: снизу вверх (от y = 9 до y = 0)
    for (int y = HEIGHT - 1; y >= 0; --y) {
        // Вычисляем ширину треугольника для текущей строки.
        // Для 10 строк и ширины 20: внизу (y=9) ширина 2, вверху (y=0) ширина 20.
        // Формула: 2 * (10 - y) дает 2, 4, 6... 20.
        // Если нужна инверсия (внизу широко, вверху узко), меняем логику:
        // В условии сказано "начиная с нижней (y=9) ... на верхней (y=0) только центр".
        // Значит, внизу широко, вверху узко.
        // Ширина = 20 - 2 * y. При y=9 ширина 2, при y=0 ширина 20. 
        // *Но* для правильного треугольника с вершиной вверху нужно, чтобы ширина уменьшалась с ростом y.
        // Для выполнения условия (внизу 20, вверху 1) при 10 шагах:
        // Ширина = 2 * (10 - y) - 1 (нечетные числа: 1, 3, 5... 19) или просто инверсия.
        // Ниже реализован вариант, где внизу широко, вверху узко (как в задании).

        int width = 20 - (y * 2); // y=9 -> 2, y=0 -> 20. (Если нужно наоборот, поменяйте формулу).
        if (width < 1) width = 1;

        // Центрирование
        int startX = (WIDTH - width) / 2;
        int endX = startX + width;

        // Заполняем строку y
        for (int x = startX; x < endX; ++x) {
            if (x >= 0 && x < WIDTH) {
                // Используем символ закраски (например, '█' или '#')
                buffer[y * WIDTH + x].Char.UnicodeChar = L'█';
                // Можно менять цвет для красоты
                buffer[y * WIDTH + x].Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            }
        }

        // 3. Безопасный вывод всего буфера через WriteConsoleOutputW
        WriteConsoleOutputW(hConsole, buffer.data(), bufferSize, bufferCoord, &writeRegion);

        // Задержка для эффекта анимации
        Sleep(200);
    }

    // Ожидание ввода перед закрытием
    system("pause");
    return 0;
}