#ifndef VALIDATION_H
#define VALIDATION_H

#include <string>

// Функции валидации данных
bool validateNoteTitle(const std::string& title);
bool validateNoteCategory(const std::string& category);
bool validateNoteContent(const std::string& content);
bool validateMenuChoice(int choice, int min, int max);

// Очистка экрана
void clearScreen();

// Запрос подтверждения
bool confirmAction(const std::string& message);

// Получение текущей даты
std::string getCurrentDate();

#endif // VALIDATION_H
