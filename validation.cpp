#include "validation.h"
#include <iostream>
#include <cctype>
#include <ctime>
#include <sstream>
#include <iomanip>

bool validateNoteTitle(const std::string& title) {
    // Проверка длины
    if (title.empty() || title.length() > 100) {
        std::cout << "Ошибка: название должно содержать от 1 до 100 символов" << std::endl;
        return false;
    }
    
    // Проверка на наличие хотя бы одного видимого символа
    bool hasVisibleChar = false;
    for (char c : title) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            hasVisibleChar = true;
            break;
        }
    }
    
    if (!hasVisibleChar) {
        std::cout << "Ошибка: название не может состоять только из пробелов" << std::endl;
        return false;
    }
    
    return true;
}

bool validateNoteCategory(const std::string& category) {
    // Проверка длины
    if (category.empty() || category.length() > 50) {
        std::cout << "Ошибка: тема должна содержать от 1 до 50 символов" << std::endl;
        return false;
    }
    
    // Проверка на наличие хотя бы одного видимого символа
    bool hasVisibleChar = false;
    for (char c : category) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            hasVisibleChar = true;
            break;
        }
    }
    
    if (!hasVisibleChar) {
        std::cout << "Ошибка: тема не может состоять только из пробелов" << std::endl;
        return false;
    }
    
    return true;
}

bool validateNoteContent(const std::string& content) {
    // Проверка длины
    if (content.empty() || content.length() > 10000) {
        std::cout << "Ошибка: текст должен содержать от 1 до 10000 символов" << std::endl;
        return false;
    }
    
    return true;
}

bool validateMenuChoice(int choice, int min, int max) {
    if (choice < min || choice > max) {
        std::cout << "Ошибка: выберите пункт от " << min << " до " << max << std::endl;
        return false;
    }
    return true;
}

void clearScreen() {
    // Использование безопасных методов очистки экрана
    #ifdef _WIN32
        // Для Windows можно использовать system, но с проверкой
        int result = system("cls");
        (void)result;  // Игнорируем результат
    #else
        // Для Unix/Linux используем ANSI escape коды как более безопасную альтернативу
        std::cout << "\033[2J\033[1;1H";
        std::cout.flush();
    #endif
}

bool confirmAction(const std::string& message) {
    std::cout << message << " (y/n): ";
    std::string response;
    std::getline(std::cin, response);
    
    return (response == "y" || response == "Y" || response == "yes" || 
            response == "д" || response == "Д" || response == "да");
}

std::string getCurrentDate() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    
    std::stringstream ss;
    ss << std::setfill('0') 
       << std::setw(4) << (localTime->tm_year + 1900) << "-"
       << std::setw(2) << (localTime->tm_mon + 1) << "-"
       << std::setw(2) << localTime->tm_mday;
    
    return ss.str();
}
