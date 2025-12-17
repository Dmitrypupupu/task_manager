#include "ui.h"
#include <iostream>

int main() {
    try {
        // Создаем менеджер заметок
        NoteManager noteManager;
        
        // Создаем интерфейс пользователя
        UI ui(noteManager);
        
        // Запускаем главный цикл
        ui.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
