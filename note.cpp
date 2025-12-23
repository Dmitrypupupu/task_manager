#include "note.h"
#include "validation.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

#ifdef _WIN32
    #include <direct.h>
    #define mkdir(path, mode) _mkdir(path)
#else
    #include <sys/stat.h>
#endif

// Константы
const std::string METADATA_FILE = "notes_metadata.dat";
const std::string NOTES_DIR = "notes";

NoteManager::NoteManager() : head(nullptr), tail(nullptr), noteCount(0), nextId(1) {
    // Создаем директорию для заметок если она не существует
    mkdir(NOTES_DIR.c_str(), 0755);
}

NoteManager::~NoteManager() {
    clearList();
}

void NoteManager::clearList() {
    NoteNode* current = head;
    while (current != nullptr) {
        NoteNode* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    tail = nullptr;
    noteCount = 0;
}

NoteNode* NoteManager::findNode(int id) const {
    NoteNode* current = head;
    while (current != nullptr) {
        if (current->data.id == id) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

bool NoteManager::addNote(const std::string& title, const std::string& category, const std::string& content) {
    // Проверка уникальности названия
    NoteNode* current = head;
    while (current != nullptr) {
        if (current->data.title == title) {
            std::cout << "Ошибка: заметка с таким названием уже существует" << std::endl;
            return false;
        }
        current = current->next;
    }
    
    // Создаем новую заметку
    Note newNote;
    newNote.id = nextId++;
    newNote.title = title;
    newNote.category = category;
    newNote.content = content;
    newNote.creationDate = getCurrentDate();
    newNote.filePath = generateFilePath(newNote.id, newNote.title);
    
    // Сохраняем заметку в файл
    try {
        saveNoteToFile(newNote);
    } catch (const std::exception& e) {
        std::cout << "Ошибка при сохранении файла: " << e.what() << std::endl;
        return false;
    }
    
    // Создаем новый узел
    NoteNode* newNode = new NoteNode(newNote);
    
    // Добавляем в конец списка
    if (tail == nullptr) {
        // Список пуст
        head = tail = newNode;
    } else {
        // Добавляем в конец
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
    noteCount++;
    
    // Обновляем метаданные
    saveToFile();
    
    return true;
}

bool NoteManager::deleteNote(int id) {
    NoteNode* node = findNode(id);
    if (node == nullptr) {
        std::cout << "Ошибка: заметка с ID " << id << " не найдена" << std::endl;
        return false;
    }
    
    // Удаляем файл заметки
    if (remove(node->data.filePath.c_str()) != 0) {
        std::cout << "Предупреждение: не удалось удалить файл заметки" << std::endl;
    }
    
    // Удаляем узел из списка
    if (node->prev != nullptr) {
        node->prev->next = node->next;
    } else {
        // Удаляем голову списка
        head = node->next;
    }
    
    if (node->next != nullptr) {
        node->next->prev = node->prev;
    } else {
        // Удаляем хвост списка
        tail = node->prev;
    }
    
    delete node;
    noteCount--;
    
    // Обновляем метаданные
    saveToFile();
    
    return true;
}

void NoteManager::displayAllNotes() const {
    if (noteCount == 0) {
        std::cout << "\nЗаметки не найдены\n" << std::endl;
        return;
    }
    
    std::cout << "\n=== СПИСОК ЗАМЕТОК ===" << std::endl;
    std::cout << "№  | Название                | Тема           | Дата создания" << std::endl;
    std::cout << "---+------------------------+----------------+--------------" << std::endl;
    
    NoteNode* current = head;
    while (current != nullptr) {
        std::cout.width(2);
        std::cout << std::left << current->data.id << " | ";
        
        std::string title = current->data.title;
        if (title.length() > 22) {
            title = title.substr(0, 19) + "...";
        }
        std::cout.width(22);
        std::cout << std::left << title << " | ";
        
        std::string category = current->data.category;
        if (category.length() > 14) {
            category = category.substr(0, 11) + "...";
        }
        std::cout.width(14);
        std::cout << std::left << category << " | ";
        
        std::cout << current->data.creationDate << std::endl;
        
        current = current->next;
    }
    std::cout << std::endl;
}

void NoteManager::displayNote(int id) const {
    NoteNode* node = findNode(id);
    if (node == nullptr) {
        std::cout << "Ошибка: заметка с ID " << id << " не найдена" << std::endl;
        return;
    }
    
    const Note& note = node->data;
    
    std::cout << "\n=== ЗАМЕТКА #" << note.id << " ===" << std::endl;
    std::cout << "Название: " << note.title << std::endl;
    std::cout << "Тема: " << note.category << std::endl;
    std::cout << "Дата: " << note.creationDate << std::endl;
    std::cout << "\nТекст:" << std::endl;
    std::cout << note.content << std::endl;
    std::cout << std::endl;
}

void NoteManager::searchByCategory(const std::string& category) const {
    bool foundAny = false;
    
    std::cout << "\n=== РЕЗУЛЬТАТЫ ПОИСКА: " << category << " ===" << std::endl;
    std::cout << "№  | Название                | Тема           | Дата создания" << std::endl;
    std::cout << "---+------------------------+----------------+--------------" << std::endl;
    
    // Ищем и сразу выводим заметки по категории (обход списка)
    NoteNode* current = head;
    while (current != nullptr) {
        if (current->data.category == category) {
            foundAny = true;
            
            std::cout.width(2);
            std::cout << std::left << current->data.id << " | ";
            
            std::string title = current->data.title;
            if (title.length() > 22) {
                title = title.substr(0, 19) + "...";
            }
            std::cout.width(22);
            std::cout << std::left << title << " | ";
            
            std::string cat = current->data.category;
            if (cat.length() > 14) {
                cat = cat.substr(0, 11) + "...";
            }
            std::cout.width(14);
            std::cout << std::left << cat << " | ";
            
            std::cout << current->data.creationDate << std::endl;
        }
        current = current->next;
    }
    
    if (!foundAny) {
        std::cout << "\nЗаметки с темой \"" << category << "\" не найдены" << std::endl;
    }
    
    std::cout << std::endl;
}

void NoteManager::loadFromFile() {
    std::ifstream file(METADATA_FILE);
    if (!file.is_open()) {
        // Файл не существует - это нормально при первом запуске
        return;
    }
    
    // Очищаем текущий список
    clearList();
    
    std::string line;
    
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        Note note;
        
        // Парсим строку: id|title|category|date|filepath
        if (std::getline(ss, token, '|')) {
            note.id = std::stoi(token);
            if (note.id >= nextId) {
                nextId = note.id + 1;
            }
        }
        if (std::getline(ss, note.title, '|') &&
            std::getline(ss, note.category, '|') &&
            std::getline(ss, note.creationDate, '|') &&
            std::getline(ss, note.filePath)) {
            
            // Загружаем содержимое из файла
            note.content = loadNoteContent(note.filePath);
            
            // Создаем новый узел и добавляем в конец списка
            NoteNode* newNode = new NoteNode(note);
            if (tail == nullptr) {
                head = tail = newNode;
            } else {
                tail->next = newNode;
                newNode->prev = tail;
                tail = newNode;
            }
            noteCount++;
        }
    }
    
    file.close();
}

void NoteManager::saveToFile() const {
    std::ofstream file(METADATA_FILE);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл метаданных для записи");
    }
    
    NoteNode* current = head;
    while (current != nullptr) {
        file << current->data.id << "|"
             << current->data.title << "|"
             << current->data.category << "|"
             << current->data.creationDate << "|"
             << current->data.filePath << std::endl;
        current = current->next;
    }
    
    file.close();
}

bool NoteManager::noteExists(int id) const {
    return findNode(id) != nullptr;
}

int NoteManager::findNoteIndex(int id) const {
    // Для совместимости с тестами возвращаем индекс узла в списке
    NoteNode* current = head;
    int index = 0;
    while (current != nullptr) {
        if (current->data.id == id) {
            return index;
        }
        current = current->next;
        index++;
    }
    return -1;
}

std::string NoteManager::generateFilePath(int id, const std::string& title) const {
    // Создаем безопасное имя файла
    std::string safeName = title;
    const std::string invalidChars = " /<>|\\:\"*?";
    
    for (size_t i = 0; i < safeName.length(); i++) {
        if (invalidChars.find(safeName[i]) != std::string::npos) {
            safeName[i] = '_';
        }
    }
    
    std::stringstream ss;
    ss << NOTES_DIR << "/" << id << "_" << safeName << ".txt";
    return ss.str();
}

void NoteManager::saveNoteToFile(const Note& note) const {
    std::ofstream file(note.filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Невозможно создать файл заметки");
    }
    
    file << "Название: " << note.title << std::endl;
    file << "Тема: " << note.category << std::endl;
    file << "Дата: " << note.creationDate << std::endl;
    file << std::endl;
    file << note.content;
    
    file.close();
}

std::string NoteManager::loadNoteContent(const std::string& filePath) const {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::string content;
    std::string line;
    bool contentStarted = false;
    
    // Пропускаем заголовочные строки и пустую строку
    for (int i = 0; i < 4; i++) {
        std::getline(file, line);
    }
    
    // Читаем содержимое
    while (std::getline(file, line)) {
        if (contentStarted) {
            content += "\n";
        }
        content += line;
        contentStarted = true;
    }
    
    file.close();
    return content;
}
