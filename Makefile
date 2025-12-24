# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Файлы проекта
TARGET = task_manager
SOURCES = main.cpp note.cpp validation.cpp ui.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = note.h validation.h ui.h

# Файлы тестов
TEST_TARGET = test_runner
TEST_SOURCES = test.cpp note.cpp validation.cpp
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)

# Основная цель
all: $(TARGET)

# Сборка исполняемого файла
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Компиляция объектных файлов
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Очистка
clean: 
	rm -f $(OBJECTS) $(TARGET)
	rm -rf notes
	rm -f notes_metadata.dat

# Очистка объектных файлов
clean-obj:
	rm -f $(OBJECTS)

# Запуск программы
run: $(TARGET)
	./$(TARGET)

# Перекомпиляция
rebuild: clean all

# Компиляция тестов
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Сборка исполняемого файла тестов
$(TEST_TARGET): test.o note.o validation.o
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) test.o note.o validation.o

# Очистка включая тесты
clean-all: clean
	rm -f $(TEST_TARGET) test.o

.PHONY: all clean clean-obj run rebuild test clean-all