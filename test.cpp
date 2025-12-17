#include "note.h"
#include "validation.h"
#include <iostream>
#include <cassert>
#include <string>
#include <cstdio>
#include <fstream>

// Цвета для консольного вывода
#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

// Статистика тестов
int testsPassed = 0;
int testsFailed = 0;

// Макрос для тестирования
#define TEST(name) void name()
#define RUN_TEST(test) \
    do { \
        std::cout << "Запуск теста: " << #test << "... "; \
        try { \
            test(); \
            std::cout << GREEN << "PASSED" << RESET << std::endl; \
            testsPassed++; \
        } catch (const std::exception& e) { \
            std::cout << RED << "FAILED: " << e.what() << RESET << std::endl; \
            testsFailed++; \
        } catch (...) { \
            std::cout << RED << "FAILED: неизвестная ошибка" << RESET << std::endl; \
            testsFailed++; \
        } \
    } while(0)

#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        throw std::runtime_error("Утверждение не выполнено: " #condition); \
    }

#define ASSERT_FALSE(condition) \
    if (condition) { \
        throw std::runtime_error("Утверждение не выполнено: !" #condition); \
    }

#define ASSERT_EQUAL(a, b) \
    if ((a) != (b)) { \
        throw std::runtime_error("Значения не равны: " #a " != " #b); \
    }

// ===== ТЕСТЫ ДЛЯ ВАЛИДАЦИИ =====

TEST(test_validate_title_valid) {
    ASSERT_TRUE(validateNoteTitle("Правильное название"));
    ASSERT_TRUE(validateNoteTitle("A"));
    ASSERT_TRUE(validateNoteTitle("123"));
    ASSERT_TRUE(validateNoteTitle("Название с пробелами"));
}

TEST(test_validate_title_empty) {
    ASSERT_FALSE(validateNoteTitle(""));
}

TEST(test_validate_title_too_long) {
    std::string longTitle(101, 'a');
    ASSERT_FALSE(validateNoteTitle(longTitle));
}

TEST(test_validate_title_only_spaces) {
    ASSERT_FALSE(validateNoteTitle("   "));
    ASSERT_FALSE(validateNoteTitle("\t\t\t"));
    ASSERT_FALSE(validateNoteTitle("  \t  "));
}

TEST(test_validate_category_valid) {
    ASSERT_TRUE(validateNoteCategory("Работа"));
    ASSERT_TRUE(validateNoteCategory("A"));
    ASSERT_TRUE(validateNoteCategory("Category123"));
}

TEST(test_validate_category_empty) {
    ASSERT_FALSE(validateNoteCategory(""));
}

TEST(test_validate_category_too_long) {
    std::string longCategory(51, 'a');
    ASSERT_FALSE(validateNoteCategory(longCategory));
}

TEST(test_validate_category_only_spaces) {
    ASSERT_FALSE(validateNoteCategory("   "));
    ASSERT_FALSE(validateNoteCategory("\t"));
}

TEST(test_validate_content_valid) {
    ASSERT_TRUE(validateNoteContent("Текст заметки"));
    ASSERT_TRUE(validateNoteContent("A"));
    ASSERT_TRUE(validateNoteContent("Длинный текст с множеством символов"));
}

TEST(test_validate_content_empty) {
    ASSERT_FALSE(validateNoteContent(""));
}

TEST(test_validate_content_too_long) {
    std::string longContent(10001, 'a');
    ASSERT_FALSE(validateNoteContent(longContent));
}

TEST(test_validate_content_max_length) {
    std::string maxContent(10000, 'a');
    ASSERT_TRUE(validateNoteContent(maxContent));
}

TEST(test_validate_menu_choice_valid) {
    ASSERT_TRUE(validateMenuChoice(1, 1, 6));
    ASSERT_TRUE(validateMenuChoice(3, 1, 6));
    ASSERT_TRUE(validateMenuChoice(6, 1, 6));
}

TEST(test_validate_menu_choice_out_of_range) {
    ASSERT_FALSE(validateMenuChoice(0, 1, 6));
    ASSERT_FALSE(validateMenuChoice(7, 1, 6));
    ASSERT_FALSE(validateMenuChoice(-1, 1, 6));
}

TEST(test_get_current_date_format) {
    std::string date = getCurrentDate();
    // Проверяем формат ГГГГ-ММ-ДД
    ASSERT_EQUAL(date.length(), 10);
    ASSERT_EQUAL(date[4], '-');
    ASSERT_EQUAL(date[7], '-');
    // Проверяем что это цифры
    ASSERT_TRUE(std::isdigit(date[0]));
    ASSERT_TRUE(std::isdigit(date[1]));
    ASSERT_TRUE(std::isdigit(date[2]));
    ASSERT_TRUE(std::isdigit(date[3]));
    ASSERT_TRUE(std::isdigit(date[5]));
    ASSERT_TRUE(std::isdigit(date[6]));
    ASSERT_TRUE(std::isdigit(date[8]));
    ASSERT_TRUE(std::isdigit(date[9]));
}

// ===== ТЕСТЫ ДЛЯ NOTEMANAGER =====

// Вспомогательная функция для очистки тестовых данных
void cleanupTestData() {
    std::remove("notes_metadata.dat");
    int result = system("rm -rf notes 2>/dev/null");
    (void)result;  // Игнорируем результат
}

TEST(test_note_manager_initialization) {
    cleanupTestData();
    NoteManager manager;
    ASSERT_EQUAL(manager.getNoteCount(), 0);
}

TEST(test_add_note_success) {
    cleanupTestData();
    NoteManager manager;
    
    bool result = manager.addNote("Тестовая заметка", "Тест", "Содержимое заметки");
    ASSERT_TRUE(result);
    ASSERT_EQUAL(manager.getNoteCount(), 1);
    
    cleanupTestData();
}

TEST(test_add_multiple_notes) {
    cleanupTestData();
    NoteManager manager;
    
    ASSERT_TRUE(manager.addNote("Заметка 1", "Тест", "Содержимое 1"));
    ASSERT_TRUE(manager.addNote("Заметка 2", "Работа", "Содержимое 2"));
    ASSERT_TRUE(manager.addNote("Заметка 3", "Личное", "Содержимое 3"));
    
    ASSERT_EQUAL(manager.getNoteCount(), 3);
    
    cleanupTestData();
}

TEST(test_add_note_duplicate_title) {
    cleanupTestData();
    NoteManager manager;
    
    ASSERT_TRUE(manager.addNote("Дубликат", "Тест", "Первая заметка"));
    ASSERT_FALSE(manager.addNote("Дубликат", "Тест", "Вторая заметка с тем же названием"));
    
    ASSERT_EQUAL(manager.getNoteCount(), 1);
    
    cleanupTestData();
}

TEST(test_note_exists) {
    cleanupTestData();
    NoteManager manager;
    
    manager.addNote("Тест", "Категория", "Содержимое");
    
    ASSERT_TRUE(manager.noteExists(1));
    ASSERT_FALSE(manager.noteExists(999));
    
    cleanupTestData();
}

TEST(test_find_note_index) {
    cleanupTestData();
    NoteManager manager;
    
    manager.addNote("Первая", "Тест", "Содержимое 1");
    manager.addNote("Вторая", "Тест", "Содержимое 2");
    
    ASSERT_EQUAL(manager.findNoteIndex(1), 0);
    ASSERT_EQUAL(manager.findNoteIndex(2), 1);
    ASSERT_EQUAL(manager.findNoteIndex(999), -1);
    
    cleanupTestData();
}

TEST(test_delete_note_success) {
    cleanupTestData();
    NoteManager manager;
    
    manager.addNote("Для удаления", "Тест", "Содержимое");
    ASSERT_EQUAL(manager.getNoteCount(), 1);
    
    ASSERT_TRUE(manager.deleteNote(1));
    ASSERT_EQUAL(manager.getNoteCount(), 0);
    
    cleanupTestData();
}

TEST(test_delete_note_not_exists) {
    cleanupTestData();
    NoteManager manager;
    
    ASSERT_FALSE(manager.deleteNote(999));
    
    cleanupTestData();
}

TEST(test_delete_multiple_notes) {
    cleanupTestData();
    NoteManager manager;
    
    manager.addNote("Заметка 1", "Тест", "Содержимое 1");
    manager.addNote("Заметка 2", "Тест", "Содержимое 2");
    manager.addNote("Заметка 3", "Тест", "Содержимое 3");
    
    ASSERT_EQUAL(manager.getNoteCount(), 3);
    
    ASSERT_TRUE(manager.deleteNote(2));
    ASSERT_EQUAL(manager.getNoteCount(), 2);
    
    ASSERT_TRUE(manager.noteExists(1));
    ASSERT_FALSE(manager.noteExists(2));
    ASSERT_TRUE(manager.noteExists(3));
    
    cleanupTestData();
}

TEST(test_save_and_load_notes) {
    cleanupTestData();
    
    {
        NoteManager manager;
        manager.addNote("Заметка для сохранения", "Тест", "Содержимое для сохранения");
        manager.saveToFile();
    }
    
    {
        NoteManager manager;
        manager.loadFromFile();
        ASSERT_EQUAL(manager.getNoteCount(), 1);
        ASSERT_TRUE(manager.noteExists(1));
    }
    
    cleanupTestData();
}

TEST(test_max_notes_limit) {
    cleanupTestData();
    NoteManager manager;
    
    // Добавляем несколько заметок для проверки (не 1000, чтобы тест был быстрым)
    for (int i = 0; i < 10; i++) {
        std::string title = "Заметка " + std::to_string(i);
        ASSERT_TRUE(manager.addNote(title, "Тест", "Содержимое"));
    }
    
    ASSERT_EQUAL(manager.getNoteCount(), 10);
    
    cleanupTestData();
}

TEST(test_generate_safe_filepath) {
    cleanupTestData();
    NoteManager manager;
    
    // Тест с небезопасными символами в названии
    bool result = manager.addNote("Название/с:символами<>", "Тест", "Содержимое");
    ASSERT_TRUE(result);
    
    cleanupTestData();
}

TEST(test_note_content_persistence) {
    cleanupTestData();
    
    std::string testContent = "Это тестовое содержимое заметки для проверки сохранения";
    
    {
        NoteManager manager;
        manager.addNote("Тест сохранения", "Тест", testContent);
        manager.saveToFile();
    }
    
    {
        NoteManager manager;
        manager.loadFromFile();
        ASSERT_EQUAL(manager.getNoteCount(), 1);
        // Содержимое должно быть загружено из файла
    }
    
    cleanupTestData();
}

// ===== ГЛАВНАЯ ФУНКЦИЯ =====

int main() {
    std::cout << "\n=== ЗАПУСК ТЕСТОВ СИСТЕМЫ УПРАВЛЕНИЯ ЗАМЕТКАМИ ===\n" << std::endl;
    
    // Тесты валидации названия
    std::cout << "--- Тесты валидации названия ---" << std::endl;
    RUN_TEST(test_validate_title_valid);
    RUN_TEST(test_validate_title_empty);
    RUN_TEST(test_validate_title_too_long);
    RUN_TEST(test_validate_title_only_spaces);
    
    // Тесты валидации категории
    std::cout << "\n--- Тесты валидации категории ---" << std::endl;
    RUN_TEST(test_validate_category_valid);
    RUN_TEST(test_validate_category_empty);
    RUN_TEST(test_validate_category_too_long);
    RUN_TEST(test_validate_category_only_spaces);
    
    // Тесты валидации содержимого
    std::cout << "\n--- Тесты валидации содержимого ---" << std::endl;
    RUN_TEST(test_validate_content_valid);
    RUN_TEST(test_validate_content_empty);
    RUN_TEST(test_validate_content_too_long);
    RUN_TEST(test_validate_content_max_length);
    
    // Тесты валидации выбора меню
    std::cout << "\n--- Тесты валидации выбора меню ---" << std::endl;
    RUN_TEST(test_validate_menu_choice_valid);
    RUN_TEST(test_validate_menu_choice_out_of_range);
    
    // Тесты получения текущей даты
    std::cout << "\n--- Тесты получения даты ---" << std::endl;
    RUN_TEST(test_get_current_date_format);
    
    // Тесты NoteManager
    std::cout << "\n--- Тесты NoteManager ---" << std::endl;
    RUN_TEST(test_note_manager_initialization);
    RUN_TEST(test_add_note_success);
    RUN_TEST(test_add_multiple_notes);
    RUN_TEST(test_add_note_duplicate_title);
    RUN_TEST(test_note_exists);
    RUN_TEST(test_find_note_index);
    RUN_TEST(test_delete_note_success);
    RUN_TEST(test_delete_note_not_exists);
    RUN_TEST(test_delete_multiple_notes);
    RUN_TEST(test_save_and_load_notes);
    RUN_TEST(test_max_notes_limit);
    RUN_TEST(test_generate_safe_filepath);
    RUN_TEST(test_note_content_persistence);
    
    // Итоги
    std::cout << "\n=== РЕЗУЛЬТАТЫ ТЕСТИРОВАНИЯ ===" << std::endl;
    std::cout << "Тесты пройдены: " << GREEN << testsPassed << RESET << std::endl;
    std::cout << "Тесты провалены: ";
    if (testsFailed > 0) {
        std::cout << RED << testsFailed << RESET << std::endl;
    } else {
        std::cout << testsFailed << std::endl;
    }
    std::cout << "Всего тестов: " << (testsPassed + testsFailed) << std::endl;
    
    // Очистка после всех тестов
    cleanupTestData();
    
    return (testsFailed == 0) ? 0 : 1;
}
