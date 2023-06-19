#include "../commands.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

int stringToCommand(const std::string &command) {
    if (command == "READ") return READ;
    if (command == "WRITE") return WRITE;
    if (command == "LOAD") return LOAD;
    if (command == "STORE") return STORE;
    if (command == "ADD") return ADD;
    if (command == "SUB") return SUB;
    if (command == "DIVIDE") return DIVIDE;
    if (command == "MUL") return MUL;
    if (command == "JUMP") return JUMP;
    if (command == "JNEG") return JNEG;
    if (command == "JZ") return JZ;
    if (command == "HALT") return HALT;
    if (command == "ADDC") return ADDC;
    if (command == "=") return 0;
    return -1;
}

int readFile(const std::string &filename, int *memory) {
  std::ifstream input(filename); // Открываем файл
  if (!input.is_open()) {
    std::cerr << "Невозможно открыть файл " << filename << std::endl;
    return -1;
  }

  std::string line;
  int lineNumber = 1;

  try {
    while (std::getline(input, line)) { // Читаем файл построчно
      int position, command,
          operand; // Извлекаем номер позиции, команду и операнд
      char *piece = strtok(const_cast<char *>(line.c_str()), " ");
      // Проверка на правильность формата первого элемента строки (номер
      // позиции)
      if (strlen(piece) != 2) {
        throw std::invalid_argument("Неверный формат в строке: " +
                                    std::to_string(lineNumber));
      }
      if (piece[0] < '0' || piece[0] > '9' || piece[1] < '0' ||
          piece[1] > '9') {
        throw std::invalid_argument("Неверный формат в строке: " +
                                    std::to_string(lineNumber));
      }
      position = atoi(piece);
      piece = strtok(NULL, " ");
      command = stringToCommand(piece); // Получаем команду
      if (command == -1) {
        throw std::invalid_argument("Неверный формат в строке: " +
                                    std::to_string(lineNumber));
      }
      piece = strtok(NULL, " "); // Получаем операнд
      if (piece == NULL) {
        throw std::invalid_argument("Неверный формат в строке: " +
                                    std::to_string(lineNumber));
      }
      if (*(piece + strlen(piece) - 1) ==
          '\n') // удаляем символ переноса строки
        piece[strlen(piece) - 1] = '\0';
      else {
        char *temp = strtok(NULL, " ");
        if (temp != NULL &&
            *temp != ';') { // проверка на символ конца строки или комментарий
          throw std::invalid_argument("Неверный формат в строке: " +
                                      std::to_string(lineNumber));
        }
      }
      if (command == 0) { // Проверяем значение переменной command
                          // Если command равно 0, то это команда присваивания
                          // Проверяем правильность формата аргумента
        if (strlen(piece) != 5) {
          throw std::invalid_argument("Неверный формат в строке: " +
                                      std::to_string(lineNumber));
        }
        if (piece[0] != '+' && piece[0] != '-') {
          throw std::invalid_argument("Неверный формат в строке: " +
                                      std::to_string(lineNumber));
        }
        for (int i = 1; i < 5;
             ++i) { // Проверяем, что символы аргумента являются цифрами или
                    // буквами A-F или a-f
          if ((piece[i] < '0' || piece[i] > '9') &&
              (piece[i] < 'a' || piece[i] > 'f') &&
              (piece[i] < 'A' || piece[i] > 'F')) {
            throw std::invalid_argument("Неверный формат в строке: " +
                                        std::to_string(lineNumber));
          }
        }
        // Преобразуем строку с аргументом в числа
        char *pEnd;
        char com[3];
        char op[3];
        strncpy(com, piece + 1, 2);
        strncpy(op, piece + 3, 2);
        command = strtol(com, &pEnd, 16);
        operand = strtol(op, &pEnd, 16);
        if (command > 0x7f ||
            operand > 0x7f) { // Проверяем, что значение command
                              // и operand не превышает 0x7f
          throw std::invalid_argument(
              "Неверная команда или операнд в строке: " +
              std::to_string(lineNumber));
        }
        if (piece[0] == '-') // Если знак команды отрицательный, то
                             // устанавливаем старший бит command в 1
          command |= 0x80;
      } else { // Если command не равно 0, то это другая команда
        if (strlen(piece) != 2) { // Проверяем правильность формата аргумента
          throw std::invalid_argument("Неверный формат в строке: " +
                                      std::to_string(lineNumber));
        }
        if (piece[0] < '0' || piece[0] > '9' || piece[1] < '0' ||
            piece[1] > '9') {
          throw std::invalid_argument("Неверный формат в строке: " +
                                      std::to_string(lineNumber));
        }
        operand = atoi(piece); // Преобразуем строку с аргументом в числа
      }
      memory[position] =
          operand + (command << 7); // Записываем команду и операнд в память
      lineNumber++;
    }
  } catch (const std::exception &e) {
    std::cerr << "Ошибка чтения файла " << filename << ": " << e.what()
              << std::endl;
    return -1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  std::cout << std::endl;
  if (argc != 3) {
    std::cout << "Неверное количество аргументов!" << std::endl;
    return -1;
  }
  std::string format(argv[1]); // сохраняем первый аргумент командной строки в
                               // строковой переменной
  if (format.substr(format.size() - 3) !=
      ".sa") { // проверяем, что расширение первого аргумента .sa
    std::cout << "Неверный формат первого аргумента"
              << std::endl; // выводим сообщение об ошибке
    return -1; // завершаем программу с кодом ошибки -1
  }
  format = std::string(argv[2]); // сохраняем второй аргумент командной строки в
                                 // строковой переменной
  if (format.substr(format.size() - 2) !=
      ".o") { // проверяем, что расширение второго аргумента .o
    std::cout << "Неверный формат второго аргумента"
              << std::endl; // выводим сообщение об ошибке
    return -1; // завершаем программу с кодом ошибки -1
  }
  int memory[100] = {0};
  if (readFile(argv[1], memory))
    return -1;
  std::ofstream out(
      argv[2],
      std::ios::binary); // создаем объект ofstream для записи в бинарный файл
  if (!out.is_open())
    return -1;
  out.write(reinterpret_cast<char *>(memory),
            sizeof(memory)); // записываем массив в файл
  std::cout << "Трансляция в машинный код завершена!" << std::endl;
  out.close();
  std::cout << "Нажмите Enter для запуска интерфейса";
  std::cin.get();
  std::cout << std::endl;
  return 0;
}
