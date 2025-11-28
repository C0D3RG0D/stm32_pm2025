#include "ssd1306.h"

// Буфер дисплея 128x64 = 1024
static uint8_t display_buffer[1024];


// Простая функция задежрки
void Delay(uint32_t count) {
    for(uint32_t i = 0; i < count; i++) {
        __asm__("nop");
    }
}


// Инициализация SPI1
void SPI1_Init(void) {
     // Включение тактирования
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN;
    
    // Настройка пинов SPI1 (PA5=SCK, PA7=MOSI)
    GPIOA->CRL &= ~(GPIO_CRL_CNF5 | GPIO_CRL_MODE5 | 
                    GPIO_CRL_CNF7 | GPIO_CRL_MODE7);
    GPIOA->CRL |= GPIO_CRL_CNF5_1 | GPIO_CRL_MODE5_1 | GPIO_CRL_MODE5_0 |
                  GPIO_CRL_CNF7_1 | GPIO_CRL_MODE7_1 | GPIO_CRL_MODE7_0;
    
    // Настройка управляющих пинов (PA1=DC, PA2=RES, PA4=CS)
    GPIOA->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_MODE1 |
                    GPIO_CRL_CNF2 | GPIO_CRL_MODE2 |
                    GPIO_CRL_CNF4 | GPIO_CRL_MODE4);
    GPIOA->CRL |= GPIO_CRL_MODE1_0 | GPIO_CRL_MODE2_0 | GPIO_CRL_MODE4_0;
    
    // Настройка SPI1
    SPI1->CR1 = SPI_CR1_CPOL | SPI_CR1_CPHA |   // Режим 3
                SPI_CR1_MSTR |                  // Режим мастера
                SPI_CR1_BR_2 | SPI_CR1_BR_1 |   // Делитель /256
                SPI_CR1_SSM | SPI_CR1_SSI |     // Программный CS
                SPI_CR1_SPE;                    // Включение SPI
}


// Отправка данных по SPI
void SPI1_Write(uint8_t data) {
    while(!(SPI1->SR & SPI_SR_TXE));  // Ждем готовности
    SPI1->DR = data;                  // Отправка данных
    while(SPI1->SR & SPI_SR_BSY);     // Ждем окончания передачи
}


// Запись команды в дисплей
void SSD1306_WriteCommand(uint8_t cmd) {
    GPIOA->BSRR = GPIO_BSRR_BR4;  // CS = 0
    GPIOA->BSRR = GPIO_BSRR_BR1;  // DC = 0 (команда)
    
    SPI1_Write(cmd);
    
    GPIOA->BSRR = GPIO_BSRR_BS4;  // CS = 1
}


// Запись данных в дисплей
void SSD1306_WriteData(uint8_t data) {
    GPIOA->BSRR = GPIO_BSRR_BR4;  // CS = 0
    GPIOA->BSRR = GPIO_BSRR_BS1;  // DC = 1 (данные)
    
    SPI1_Write(data);
    
    GPIOA->BSRR = GPIO_BSRR_BS4;  // CS = 1
}


void SSD1306_Init(void) {
    // Сброс дисплея
    GPIOA->BSRR = GPIO_BSRR_BR2;  // RESET = 0
    for(volatile int i = 0; i < 10000; i++); // Задержка
    GPIOA->BSRR = GPIO_BSRR_BS2;  // RESET = 1
    for(volatile int i = 0; i < 10000; i++); // Задержка
    
    // Последовательность команд инициализации
    SSD1306_WriteCommand(0xAE); // Display OFF
    SSD1306_WriteCommand(0x20); // Memory addressing mode
    SSD1306_WriteCommand(0x00); // Horizontal addressing mode
    SSD1306_WriteCommand(0x21); // Set column address
    SSD1306_WriteCommand(0x00); // Start column = 0
    SSD1306_WriteCommand(0x7F); // End column = 127
    SSD1306_WriteCommand(0x22); // Set page address
    SSD1306_WriteCommand(0x00); // Start page = 0
    SSD1306_WriteCommand(0x07); // End page = 7
    SSD1306_WriteCommand(0x40); // Set start line = 0
    SSD1306_WriteCommand(0xA1); // Segment remap (0xA0 - normal, 0xA1 - remapped)
    SSD1306_WriteCommand(0xC8); // COM output scan direction (0xC0 - normal, 0xC8 - remapped)
    SSD1306_WriteCommand(0xDA); // COM pins hardware configuration
    SSD1306_WriteCommand(0x12); // Alternative COM pin configuration
    SSD1306_WriteCommand(0x81); // Set contrast
    SSD1306_WriteCommand(0xCF); // Contrast value
    SSD1306_WriteCommand(0xA6); // Set normal display (0xA7 - inverse)
    SSD1306_WriteCommand(0xA4); // Entire display ON (0xA5 - all pixels ON)
    SSD1306_WriteCommand(0xD5); // Set display clock divide ratio/oscillator frequency
    SSD1306_WriteCommand(0x80); // Default value
    SSD1306_WriteCommand(0x8D); // Charge pump
    SSD1306_WriteCommand(0x14); // Enable charge pump
    SSD1306_WriteCommand(0xAF); // Display ON
}


// Очистка дисплея
void SSD1306_Clear(void) {
     for(int i = 0; i < 1024; i++) {
        display_buffer[i] = 0x00;
    }
}


// Обноваление дисплея (отправка буфера)
void SSD1306_Update(void) {
    SSD1306_WriteCommand(0x21); // Set column address
    SSD1306_WriteCommand(0x00); // Start column = 0
    SSD1306_WriteCommand(0x7F); // End column = 127
    
    SSD1306_WriteCommand(0x22); // Set page address  
    SSD1306_WriteCommand(0x00); // Start page = 0
    SSD1306_WriteCommand(0x07); // End page = 7
    
    // Отправка всего буфера
    for(int i = 0; i < 1024; i++) {
        SSD1306_WriteData(display_buffer[i]);
    }
}



// Функция демонстрации всех паттернов
void SSD1306_Demo(void){

    // 1. Градиент
    SSD1306_Clear();
    SSD1306_Gradient();
    SSD1306_Update();
    Delay(500000000);
    
    // 2. Перекрестие
    SSD1306_Clear();
    SSD1306_Crosshair();
    SSD1306_Update();
    Delay(500000000);
    
    // 3. Рамка
    SSD1306_Clear();
    SSD1306_Borders();
    SSD1306_Update();
    Delay(500000000);
    
    // 4. Чередующиеся строки
    SSD1306_Clear();
    SSD1306_Stripers();
    SSD1306_Update();
    Delay(500000000); 
    
    // 5. Шахматная доска (финальный паттерн)
    SSD1306_Clear();
    SSD1306_ChessBoard();
    SSD1306_Update();
}



void SSD1306_Gradient(void){
    for(int page = 0; page < 8; page++) {

        for(int col = 0; col < 128; col++) {
            // Вертикальные полосы разной ширины
            uint8_t pattern = 0;

            if(col % 16 < 8) pattern = 0xFF;  // Широкая белая полоса

            else if(col % 8 < 4) pattern = 0xAA;  // Средняя

            else if(col % 4 < 2) pattern = 0x55;  // Узкая

            else pattern = 0x00;  // Черная
            
            display_buffer[page * 128 + col] = pattern;
        }
    }
}


void SSD1306_Crosshair(void){
    // Вертикальная линия
    for(int page = 0; page < 8; page++) {

        display_buffer[page * 128 + 64] = 0xFF;

    }

    // Горизонтальная линия
    for(int col = 0; col < 128; col++) {

        display_buffer[3 * 128 + col] = 0xFF;
        display_buffer[4 * 128 + col] = 0xFF;

    }
}


void SSD1306_Borders(void){
    // Верх и низ
    for(int col = 0; col < 128; col++) {

        display_buffer[0 * 128 + col] = 0xFF;
        display_buffer[7 * 128 + col] = 0xFF;

    }

    // Лево и право
    for(int page = 0; page < 8; page++) {

        display_buffer[page * 128 + 0] = 0xFF;
        display_buffer[page * 128 + 1] = 0xFF;
        display_buffer[page * 128 + 126] = 0xFF;
        display_buffer[page * 128 + 127] = 0xFF;

    }
}


void SSD1306_Stripers(void){
    for(int page = 0; page < 8; page++) {

        uint8_t pattern = (page % 2 == 0) ? 0xFF : 0x00;

        for(int col = 0; col < 128; col++) {

            display_buffer[page * 128 + col] = pattern;

        }
    }
}


void SSD1306_ChessBoard(void){
    for(int page = 0; page < 8; page++) {
        for(int col = 0; col < 128; col++) {
            // Квадраты 8x8 пикселей
            int square_x = col / 8;
            int square_y = page;  // Каждая страница = 8 пикселей по вертикали
            
            if((square_x + square_y) % 2 == 0) {
                display_buffer[page * 128 + col] = 0xFF; // Белый квадрат
            } else {
                display_buffer[page * 128 + col] = 0x00; // Черный квадрат
            }
        }
    }
}
