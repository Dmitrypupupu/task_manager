#ifndef UI_H
#define UI_H

#include "note.h"

// Класс пользовательского интерфейса
class UI {
private:
    NoteManager& noteManager;
    
public:
    explicit UI(NoteManager& manager);
    
    // Основное меню
    void run();
    
private:
    // Отображение меню
    void displayMainMenu() const;
    
    // Обработчики команд меню
    void handleCreateNote();
    void handleShowAllNotes();
    void handleSearchByCategory();
    void handleOpenNote();
    void handleDeleteNote();
    
    // Вспомогательные функции ввода
    std::string getInput(const std::string& prompt);
    int getIntInput(const std::string& prompt);
};

#endif // UI_H
