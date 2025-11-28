#include "stm32f10x.h"
#include "ssd1306.h"


int main(void) {
    // Инициализация
    SPI1_Init();
    SSD1306_Init();
    
    // Демонстрация всех паттернов
    SSD1306_Demo();
    
    while(1) {
        // Бесконечный цикл
    }
}
