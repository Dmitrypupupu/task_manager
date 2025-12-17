#include "note.h"
#include "validation.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>

// Константы
const std::string METADATA_FILE = "notes_metadata.dat";
const std::string NOTES_DIR = "notes";

NoteManager::NoteManager() : noteCount(0), nextId(1) {
    // Создаем директорию для заметок если она не существует
    #ifdef _WIN32
        _mkdir(NOTES_DIR.c_str());
    #else
        mkdir(NOTES_DIR.c_str(), 0755);
    #endif
}

bool NoteManager::addNote(const std::string& title, const std::string& category, const std::string& content) {
    // Проверка на максимальное количество заметок
    if (noteCount >= MAX_NOTES) {
        std::cout << "Ошибка: достигнуто максимальное количество заметок (" << MAX_NOTES << ")" << std::endl;
        return false;
    }
    
    // Проверка уникальности названия
    for (int i = 0; i < noteCount; i++) {
        if (notes[i].title == title) {
            std::cout << "Ошибка: заметка с таким названием уже существует" << std::endl;
            return false;
        }
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
    
    // Добавляем в массив
    notes[noteCount++] = newNote;
    
    // Обновляем метаданные
    saveToFile();
    
    return true;
}

bool NoteManager::deleteNote(int id) {
    int index = findNoteIndex(id);
    if (index == -1) {
        std::cout << "Ошибка: заметка с ID " << id << " не найдена" << std::endl;
        return false;
    }
    
    // Удаляем файл заметки
    if (remove(notes[index].filePath.c_str()) != 0) {
        std::cout << "Предупреждение: не удалось удалить файл заметки" << std::endl;
    }
    
    // Сдвигаем элементы массива
    for (int i = index; i < noteCount - 1; i++) {
        notes[i] = notes[i + 1];
    }
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
    
    for (int i = 0; i < noteCount; i++) {
        std::cout.width(2);
        std::cout << std::left << notes[i].id << " | ";
        
        std::string title = notes[i].title;
        if (title.length() > 22) {
            title = title.substr(0, 19) + "...";
        }
        std::cout.width(22);
        std::cout << std::left << title << " | ";
        
        std::string category = notes[i].category;
        if (category.length() > 14) {
            category = category.substr(0, 11) + "...";
        }
        std::cout.width(14);
        std::cout << std::left << category << " | ";
        
        std::cout << notes[i].creationDate << std::endl;
    }
    std::cout << std::endl;
}

void NoteManager::displayNote(int id) const {
    int index = findNoteIndex(id);
    if (index == -1) {
        std::cout << "Ошибка: заметка с ID " << id << " не найдена" << std::endl;
        return;
    }
    
    const Note& note = notes[index];
    
    std::cout << "\n=== ЗАМЕТКА #" << note.id << " ===" << std::endl;
    std::cout << "Название: " << note.title << std::endl;
    std::cout << "Тема: " << note.category << std::endl;
    std::cout << "Дата: " << note.creationDate << std::endl;
    std::cout << "\nТекст:" << std::endl;
    std::cout << note.content << std::endl;
    std::cout << std::endl;
}

void NoteManager::searchByCategory(const std::string& category) const {
    // Создаем временный массив для результатов
    Note results[MAX_NOTES];
    int resultCount = 0;
    
    // Ищем заметки по категории
    for (int i = 0; i < noteCount; i++) {
        if (notes[i].category == category) {
            results[resultCount++] = notes[i];
        }
    }
    
    if (resultCount == 0) {
        std::cout << "\nЗаметки с темой \"" << category << "\" не найдены\n" << std::endl;
        return;
    }
    
    std::cout << "\n=== РЕЗУЛЬТАТЫ ПОИСКА: " << category << " ===" << std::endl;
    std::cout << "№  | Название                | Тема           | Дата создания" << std::endl;
    std::cout << "---+------------------------+----------------+--------------" << std::endl;
    
    for (int i = 0; i < resultCount; i++) {
        std::cout.width(2);
        std::cout << std::left << results[i].id << " | ";
        
        std::string title = results[i].title;
        if (title.length() > 22) {
            title = title.substr(0, 19) + "...";
        }
        std::cout.width(22);
        std::cout << std::left << title << " | ";
        
        std::string cat = results[i].category;
        if (cat.length() > 14) {
            cat = cat.substr(0, 11) + "...";
        }
        std::cout.width(14);
        std::cout << std::left << cat << " | ";
        
        std::cout << results[i].creationDate << std::endl;
    }
    std::cout << std::endl;
}

void NoteManager::loadFromFile() {
    std::ifstream file(METADATA_FILE);
    if (!file.is_open()) {
        // Файл не существует - это нормально при первом запуске
        return;
    }
    
    noteCount = 0;
    std::string line;
    
    while (std::getline(file, line) && noteCount < MAX_NOTES) {
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
            
            notes[noteCount++] = note;
        }
    }
    
    file.close();
}

void NoteManager::saveToFile() const {
    std::ofstream file(METADATA_FILE);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл метаданных для записи");
    }
    
    for (int i = 0; i < noteCount; i++) {
        file << notes[i].id << "|"
             << notes[i].title << "|"
             << notes[i].category << "|"
             << notes[i].creationDate << "|"
             << notes[i].filePath << std::endl;
    }
    
    file.close();
}

bool NoteManager::noteExists(int id) const {
    return findNoteIndex(id) != -1;
}

int NoteManager::findNoteIndex(int id) const {
    for (int i = 0; i < noteCount; i++) {
        if (notes[i].id == id) {
            return i;
        }
    }
    return -1;
}

std::string NoteManager::generateFilePath(int id, const std::string& title) const {
    // Создаем безопасное имя файла
    std::string safeName = title;
    for (size_t i = 0; i < safeName.length(); i++) {
        if (safeName[i] == ' ') {
            safeName[i] = '_';
        } else if (safeName[i] == '/' || safeName[i] == '\\' || safeName[i] == ':') {
            safeName[i] = '-';
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
