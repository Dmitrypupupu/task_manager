#ifndef NOTE_H
#define NOTE_H

#include <string>

// Максимальное количество заметок
const int MAX_NOTES = 1000;

// Структура для хранения заметки
struct Note {
    int id;                      // Уникальный идентификатор заметки
    std::string title;           // Название заметки
    std::string category;        // Тема/категория заметки
    std::string content;         // Текст заметки
    std::string creationDate;    // Дата создания в формате ГГГГ-ММ-ДД
    std::string filePath;        // Путь к файлу заметки
};

// Класс для управления заметками
class NoteManager {
private:
    Note notes[MAX_NOTES];  // Массив заметок (вместо вектора)
    int noteCount;          // Текущее количество заметок
    int nextId;             // Следующий доступный ID

public:
    NoteManager();
    
    // Основные операции
    bool addNote(const std::string& title, const std::string& category, const std::string& content);
    bool deleteNote(int id);
    void displayAllNotes() const;
    void displayNote(int id) const;
    
    // Поиск и фильтрация
    void searchByCategory(const std::string& category) const;
    
    // Работа с данными
    void loadFromFile();
    void saveToFile() const;
    
    // Вспомогательные функции
    int getNoteCount() const { return noteCount; }
    bool noteExists(int id) const;
    int findNoteIndex(int id) const;
    
private:
    // Генерация пути к файлу заметки
    std::string generateFilePath(int id, const std::string& title) const;
    
    // Сохранение/загрузка отдельной заметки
    void saveNoteToFile(const Note& note) const;
    std::string loadNoteContent(const std::string& filePath) const;
};

#endif // NOTE_H
