# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Файлы проекта
TARGET = task_manager
SOURCES = main.cpp note.cpp validation.cpp ui.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = note.h validation.h ui.h

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

.PHONY: all clean clean-obj run rebuild
