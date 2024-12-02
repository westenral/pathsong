TARGET := target/pathsong

build: src/main.cpp
	gcc -Wall -Wpedantic -Werror -Iinclude -O2 -o $(TARGET) src/main.cpp

run: build
	./$(TARGET)

clean:
	rm $(TARGET)
