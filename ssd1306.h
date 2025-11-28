#ifndef SSD1306_H
#define SSD1306_H

#include "stm32f10x.h"


// Простая функция задержки
void Delay(uint32_t count);

// Прототипы функций SPI
void SPI1_Init(void);
void SPI1_Write(uint8_t data);


// Прототипы функций дисплея
void SSD1306_Init(void);
void SSD1306_WriteCommand(uint8_t cmd);
void SSD1306_WriteData(uint8_t data);
void SSD1306_Clear(void);
void SSD1306_Update(void);


// Тестовые паттерны
void SSD1306_Gradient(void);
void SSD1306_Crosshair(void);
void SSD1306_Borders(void);
void SSD1306_Stripers(void);
void SSD1306_ChessBoard(void);
void SSD1306_Demo(void);

#endif