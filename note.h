#ifndef NOTE_H
#define NOTE_H

#include <string>

// Структура для хранения заметки
struct Note {
    int id;                      // Уникальный идентификатор заметки
    std::string title;           // Название заметки
    std::string category;        // Тема/категория заметки
    std::string content;         // Текст заметки
    std::string creationDate;    // Дата создания в формате ГГГГ-ММ-ДД
    std::string filePath;        // Путь к файлу заметки
};

// Узел двусвязного списка
struct NoteNode {
    Note data;                   // Данные заметки
    NoteNode* next;              // Указатель на следующий узел
    NoteNode* prev;              // Указатель на предыдущий узел
    
    NoteNode(const Note& note) : data(note), next(nullptr), prev(nullptr) {}
};

// Класс для управления заметками
class NoteManager {
private:
    NoteNode* head;             // Голова списка
    NoteNode* tail;             // Хвост списка
    int noteCount;              // Текущее количество заметок
    int nextId;                 // Следующий доступный ID

public:
    NoteManager();
    ~NoteManager();
    
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
    
    // Поиск узла по ID
    NoteNode* findNode(int id) const;
    
    // Очистка списка
    void clearList();
};

#endif // NOTE_H
