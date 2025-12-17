#include "ui.h"
#include "validation.h"
#include <iostream>
#include <limits>

UI::UI(NoteManager& manager) : noteManager(manager) {}

void UI::run() {
    // Загружаем данные при запуске
    noteManager.loadFromFile();
    
    bool running = true;
    
    while (running) {
        displayMainMenu();
        
        int choice = getIntInput("Выберите пункт меню: ");
        
        if (!validateMenuChoice(choice, 1, 6)) {
            continue;
        }
        
        std::cout << std::endl;
        
        switch (choice) {
            case 1:
                handleCreateNote();
                break;
            case 2:
                handleShowAllNotes();
                break;
            case 3:
                handleSearchByCategory();
                break;
            case 4:
                handleOpenNote();
                break;
            case 5:
                handleDeleteNote();
                break;
            case 6:
                std::cout << "Выход из программы. До свидания!" << std::endl;
                running = false;
                break;
        }
        
        if (running) {
            std::cout << "\nНажмите Enter для продолжения...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

void UI::displayMainMenu() const {
    clearScreen();
    std::cout << "=== СИСТЕМА УПРАВЛЕНИЯ ЗАМЕТКАМИ ===" << std::endl;
    std::cout << "1. Создать новую заметку" << std::endl;
    std::cout << "2. Показать все заметки" << std::endl;
    std::cout << "3. Поиск по теме" << std::endl;
    std::cout << "4. Открыть заметку" << std::endl;
    std::cout << "5. Удалить заметку" << std::endl;
    std::cout << "6. Выход" << std::endl;
    std::cout << std::endl;
}

void UI::handleCreateNote() {
    std::cout << "=== СОЗДАНИЕ НОВОЙ ЗАМЕТКИ ===" << std::endl;
    
    std::string title;
    do {
        title = getInput("Введите название заметки: ");
    } while (!validateNoteTitle(title));
    
    std::string category;
    do {
        category = getInput("Введите тему заметки: ");
    } while (!validateNoteCategory(category));
    
    std::string content;
    do {
        content = getInput("Введите текст заметки: ");
    } while (!validateNoteContent(content));
    
    if (noteManager.addNote(title, category, content)) {
        std::cout << "\nЗаметка успешно создана!" << std::endl;
    } else {
        std::cout << "\nНе удалось создать заметку." << std::endl;
    }
}

void UI::handleShowAllNotes() {
    noteManager.displayAllNotes();
}

void UI::handleSearchByCategory() {
    std::string category = getInput("Введите тему для поиска: ");
    noteManager.searchByCategory(category);
}

void UI::handleOpenNote() {
    if (noteManager.getNoteCount() == 0) {
        std::cout << "Нет доступных заметок для отображения." << std::endl;
        return;
    }
    
    int id = getIntInput("Введите ID заметки: ");
    noteManager.displayNote(id);
}

void UI::handleDeleteNote() {
    if (noteManager.getNoteCount() == 0) {
        std::cout << "Нет доступных заметок для удаления." << std::endl;
        return;
    }
    
    int id = getIntInput("Введите ID заметки для удаления: ");
    
    if (!noteManager.noteExists(id)) {
        std::cout << "Заметка с ID " << id << " не найдена." << std::endl;
        return;
    }
    
    if (confirmAction("Вы уверены, что хотите удалить эту заметку?")) {
        if (noteManager.deleteNote(id)) {
            std::cout << "Заметка успешно удалена!" << std::endl;
        } else {
            std::cout << "Не удалось удалить заметку." << std::endl;
        }
    } else {
        std::cout << "Удаление отменено." << std::endl;
    }
}

std::string UI::getInput(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

int UI::getIntInput(const std::string& prompt) {
    std::cout << prompt;
    int value;
    
    while (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка: введите число." << std::endl;
        std::cout << prompt;
    }
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}
