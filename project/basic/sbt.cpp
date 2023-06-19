#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <vector>

int lastCell = 99;  // Последняя ячейка памяти
int currentCommandCell = 0;  // Текущая ячейка памяти для команды
int variablesNum = 0;  // Количество переменных
int position = 0;     // Позиция при чтении выражений

// Класс для представления переменных
class Variable {
 private:
  std::string title;  // Название переменной
  int address;   // Адрес переменной
 public:
  Variable() {}
  Variable(std::string tit, int addr) {
    title = tit;
    address = addr;
  }
  std::string getTit() { return title; }
  int getAddr() { return address; }
};

// Структура для представления команды ассемблера
struct AsmbCommand {
  AsmbCommand(int cell, char *command, int operand) {
    this->cell = cell;
    this->command = command;
    this->operand = operand;
  }
  int cell;  // Адрес ячейки памяти
  char *command;
  int operand;
};

std::vector<Variable> variables;       // Вектор переменных
std::vector<AsmbCommand> commands;  // Вектор команд ассемблера
std::vector<int> queueGoTo;        // Очередь для команды GOTO
std::map<int, int> startCommands;  // Словарь для хранения адресов начала команд
std::stack<int> Operands;         // Стек операндов
std::stack<char> Functions;       // Стек операций

// Функция для проверки наличия переменной
int presenceVarCheck(std::string str);

// Функции для обработки различных команд
int Input(char *operand);
int Print(char *operand);
int GoTo(char *operand);
int End(char *operand);
int Let(char *str_operands);
int If(char *str_operands);

// Вспомогательные функции для работы с выражениями
void popFunction();
bool canPop(char op1);
int getPriority(char op);
std::string getToken(std::string s);
std::string readInteger(std::string s);
void readWhiteSpace(std::string s);

int translate_bsc(const std::string &to_translate,
                  const std::string &save_file) {
  std::ifstream input(to_translate);
  if (!input) {
    std::cerr << "Ошибка чтения файла " << to_translate << std::endl;
    return -1;
  }

  std::ofstream output(save_file);
  if (!output) {
    std::cerr << "Ошибка записи файла " << save_file << std::endl;
    return -1;
  }

  int previous_addr = -1;
  bool checkMainEnd = false;

  while (input.good()) {
    char line[200];
    input.getline(line, 200);

    char *str_line = strtok(line, "\n");
    char *str_address = strtok(str_line, " ");
    int address = atoi(str_address);
    if (strlen(str_address) > 2 || address < 0 || address > 99)
      return -1;
    if (address < previous_addr)
      return -1;
    previous_addr = address;

    char *str_command = strtok(NULL, " ");
    if (str_command == nullptr)
      continue;
    if (!strcmp(str_command, "REM")) {
      continue;
    } else if (!strcmp(str_command, "INPUT")) {
      char *str_operand = strtok(NULL, " \n");
      startCommands[address] = currentCommandCell;
      if (Input(str_operand))
        return -1;
    } else if (!strcmp(str_command, "PRINT")) {
      char *str_operand = strtok(NULL, " \n");
      startCommands[address] = currentCommandCell;
      if (Print(str_operand))
        return -1;
    } else if (!strcmp(str_command, "GOTO")) {
      char *str_operand = strtok(NULL, " ");
      startCommands[address] = currentCommandCell;
      if (GoTo(str_operand))
        return -1;
    } else if (!strcmp(str_command, "END")) {
      char *str_operand = strtok(NULL, "");
      startCommands[address] = currentCommandCell;
      if (End(str_operand))
        return -1;
      checkMainEnd = true;
      break;
    } else if (!strcmp(str_command, "LET")) {
      char *str_operand = strtok(NULL, "");
      startCommands[address] = currentCommandCell;
      if (Let(str_operand))
        return -1;
    } else if (!strcmp(str_command, "IF")) {
      char *str_operand = strtok(NULL, "");
      startCommands[address] = currentCommandCell;
      if (If(str_operand))
        return -1;
    }

    if (currentCommandCell > lastCell)
      return -1;
  }

  if (!checkMainEnd)
    return -1;
  input.close();
  for (auto &i : queueGoTo) {
    if (startCommands.find(commands[i].operand) == startCommands.end()) {
      return -1;
    } else {
      short addressOperand = startCommands.find(commands[i].operand)->second;
      commands[i].operand = addressOperand;
    }
  }

  int gg = 0;
  for (auto &i : commands) {
    if (gg == 0) {
      output << std::setw(2) << std::setfill('0') << i.cell << " " << i.command
             << " " << std::setw(2) << std::setfill('0') << i.operand;
      gg++;
    } else {
      output << "\n"
             << std::setw(2) << std::setfill('0') << i.cell << " " << i.command
             << " " << std::setw(2) << std::setfill('0') << i.operand;
    }
  }

  for (int i = variables.size() - 1; i >= 0; i--) {
    if (!((variables[i].getTit()[0] >= 'A') &&
          (variables[i].getTit()[0] <= 'Z'))) {
      output << "\n"
             << std::setw(2) << std::setfill('0') << variables[i].getAddr()
             << " = +" << std::setw(4) << std::setfill('0')
             << stoi(variables[i].getTit());
    }
  }
  output.close();

  return 0;
}

int Input(char *operand) {
  if (!((operand[0] >= 'A') && (operand[0] <= 'Z')))
    return -1;
  int addressOperand = presenceVarCheck(operand);  // Получаем адрес операнда
  commands.emplace_back(AsmbCommand(
      currentCommandCell++, (char *)"READ",
      addressOperand));  // Генерируем код для чтения значения переменной
  return 0;
}

int Print(char *operand) {
  if (!((operand[0] >= 'A') && (operand[0] <= 'Z')))
    return -1;
  int addressOperand = presenceVarCheck(operand);
  commands.emplace_back(
      AsmbCommand(currentCommandCell++, (char *)"WRITE", addressOperand));
  return 0;
}

int GoTo(char *operand) {
  int address = atoi(operand);
  if (strlen(operand) > 2 || address < 0 || address > 99)
    return -1;
  if (startCommands.find(address) ==
      startCommands
          .end()) {  // Если адрес не присутствует в словаре startCommands
    commands.emplace_back(
        AsmbCommand(currentCommandCell++, (char *)"JUMP",
                    address));  // Генерируем код для безусловного перехода на
                               // указанный адрес
    queueGoTo.emplace_back(commands.size() -
                           1);  // Добавляем адрес команды в очередь GoTo для
                               // последующей замены, если потребуется
  } else {
    int addressOperand = startCommands.find(address)
                             ->second;  // Получаем адрес команды,
                                       // соответствующей указанному адресу
    commands.emplace_back(
        AsmbCommand(currentCommandCell++, (char *)"JUMP", addressOperand));
  }
  return 0;
}

int End(char *operand) {
  if (operand != NULL)
    return -1;
  commands.emplace_back(AsmbCommand(currentCommandCell++, (char *)"HALT", 0));
  return 0;
}

int If(char *operand) {
  // Разбиваем операнд на отдельные части с помощью strtok
  char *value1 = strtok(operand, " "); // первое значение операнда
  char *condition = strtok(NULL, " "); // условие
  char *value2 = strtok(NULL, " "); // второе значение операнда
  char *command = strtok(NULL, " "); // команда после If (обычно GOTO)
  char *str_address = strtok(NULL, ""); // строковое значение адреса перехода

  // Проверяем, являются ли значения операнда и команда допустимыми
  int addressvalue1 = presenceVarCheck(value1);
  int addressvalue2 = presenceVarCheck(value2);
  int basic_address = atoi(str_address);  // адрес перехода
  int goto_address = 0;
  if (!(!((value1[0] >= 'A') && (value1[0] <= 'Z')) ||
        !((value2[0] >= 'A') && (value2[0] <= 'Z'))) &&
      !(!((value1[0] >= '0') && (value1[0] <= '9')) ||
        !((value2[0] >= '0') && (value2[0] <= '9'))) &&
      strcmp(command, "GOTO") != 0)
    return -1;
  // Если адрес перехода присутствует в словаре startCommands, берем значение из
  // словаря иначе, добавляем новый адрес в очередь
  if (startCommands.find(basic_address) == startCommands.end()) {
    goto_address = basic_address;
    queueGoTo.emplace_back(commands.size() + 2);
  } else {
    goto_address = startCommands.find(basic_address)->second;
  }
  // Генерируем код в зависимости от условия
  if (!strcmp(condition, "=")) {
    commands.emplace_back(
        AsmbCommand(currentCommandCell++, (char *)"LOAD", addressvalue1));
    commands.emplace_back(
        AsmbCommand(currentCommandCell++, (char *)"SUB", addressvalue2));
    commands.emplace_back(
        AsmbCommand(currentCommandCell++, (char *)"JZ",
                    goto_address));  // если результат равен нулю, переходим по
                                     // адресу goto_address
  } else {
    if (!strcmp(condition, "<")) {
      commands.emplace_back(
          AsmbCommand(currentCommandCell++, (char *)"LOAD", addressvalue1));
      commands.emplace_back(
          AsmbCommand(currentCommandCell++, (char *)"SUB", addressvalue2));
    } else {
      commands.emplace_back(
          AsmbCommand(currentCommandCell++, (char *)"LOAD", addressvalue2));
      commands.emplace_back(
          AsmbCommand(currentCommandCell++, (char *)"SUB", addressvalue1));
    }
    commands.emplace_back(
        AsmbCommand(currentCommandCell++, (char *)"JNEG",
                    goto_address));  // если результат отрицательный, переходим
                                     // по адресу goto_address
  }
  return 0;
}

int Let(char *operand) {
  position = 0;
  if (!((operand[0] >= 'A') && (operand[0] <= 'Z')) &&
      (operand[1] != '=' || operand[2] != '=')) {
    return -1;
  }
  char *var = strtok(operand, " ");
  char *value = strtok(NULL, " =");
  char *epresh = strtok(NULL, "");
  if (epresh ==
      NULL) {  // если после первого значения не идёт продолжение выражения
    int valueAddress = presenceVarCheck(value);
    commands.emplace_back(
        AsmbCommand(currentCommandCell++, (char *)"LOAD", valueAddress));
    int varAddress = presenceVarCheck(var);
    commands.emplace_back(
        AsmbCommand(currentCommandCell++, (char *)"STORE", varAddress));
  } else {  // если есть продолжение выражения
    std::string expr = epresh;
    expr = "(" + (std::string)value + " " + expr +
           ")";  // форматируем и добавляем к expr начало и конец выражения
    std::string token, prevToken;
    do {
      token = getToken(expr);
      if (prevToken == "(" && (token == "+" || token == "-")) {
        int address =
            presenceVarCheck((std::string) "0");  // получаем адрес переменной "0"
        Operands.push(address);  // кладем адрес в стек операндов
      }

      if (isdigit(token[0]) || ((token[0] >= 'A') && (token[0] <= 'Z'))) {
        int address = presenceVarCheck(token);
        Operands.push(address);
      } else {  // если первый символ токена - знак операции
        if (token == ")") {
          while (Functions.size() > 0 &&
                 Functions.top() !=
                     '(')  // пока стек функций не пуст и верхняя функция на
                           // стеке не является открывающей скобкой
            popFunction();  // выталкиваем функции из стека
          Functions.pop();  // выталкиваем открывающую скобку из стека
        } else {  // если токен - не закрывающая скобка
          while (canPop(token[0]))  // пока можно извлекать функции из стека
            popFunction();
          Functions.push(token[0]);  // помещаем знак операции в стек функций
        }
      }
      prevToken = token;
    } while (position != (int)expr.length()); // повторяем цикл, пока не
                                              // обработаем все токены в expr
    int varAddress = presenceVarCheck(var);  // получаем адрес переменной var
    commands.emplace_back(
        AsmbCommand(currentCommandCell++, (char *)"STORE", varAddress));
    Operands.pop();  // извлекаем адрес из стека операндов
  }
  return 0;
}

void popFunction() { // извлекает функцию из стека функций, извлекает два
                     // операнда из стека операндов, выполняет операцию и кладет
                     // результат в стек операндов
  int B = Operands.top();  // получаем верхний операнд из стека операндов и
                           // записываем его в переменную B, потом в A
  Operands.pop();
  int A = Operands.top();
  Operands.pop();
  char oper = Functions.top();
  Functions.pop();

  int address =
      lastCell - variablesNum;  // получаем адрес для записи результата операции
  variablesNum++;
  switch (oper) {
  case '+':
    commands.emplace_back(AsmbCommand(currentCommandCell++, (char *)"LOAD", A));
    commands.emplace_back(AsmbCommand(currentCommandCell++, (char *)"ADD", B));
    commands.emplace_back(
        AsmbCommand(currentCommandCell++, (char *)"STORE", address));
    break;
  case '-':
    commands.emplace_back(AsmbCommand(currentCommandCell++, (char *)"LOAD", A));
    commands.emplace_back(AsmbCommand(currentCommandCell++, (char *)"SUB", B));
    commands.emplace_back(
        AsmbCommand(currentCommandCell++, (char *)"STORE", address));
    break;
  case '*':
    commands.emplace_back(AsmbCommand(currentCommandCell++, (char *)"LOAD", A));
    commands.emplace_back(AsmbCommand(currentCommandCell++, (char *)"MUL", B));
    commands.emplace_back(
        AsmbCommand(currentCommandCell++, (char *)"STORE", address));
    break;
  case '/':
    commands.emplace_back(AsmbCommand(currentCommandCell++, (char *)"LOAD", A));
    commands.emplace_back(
        AsmbCommand(currentCommandCell++, (char *)"DIVIDE", B));
    commands.emplace_back(
        AsmbCommand(currentCommandCell++, (char *)"STORE", address));
    break;
  }
  Operands.push(
      address);  // кладем адрес для записи результата операции в стек операндов
}

bool canPop(char op1) {  // возможность извлечения функции из стека функций
  if (Functions.size() == 0)
    return false;
  int p1 = getPriority(op1);  // получаем приоритет текущей операции
  int p2 = getPriority(
      Functions.top());  // получаем приоритет верхней функции в стеке функций

  return p1 >= 0 && p2 >= 0 &&
         p1 >= p2;  // возвращаем результат проверки на возможность извлечения
                    // функции из стека функций
}

int getPriority(char op) {
  switch (op) {
  case '(':
    return -1;
  case '*':
  case '/':
    return 1;
  case '+':
  case '-':
    return 2;
  default:
    return -1;
  }
}

std::string getToken(std::string s) {  // получает очередной токен из выражения
  readWhiteSpace(s);  // пропускаем все пробелы
  std::string tok = "";  // инициализируем строковую переменную tok

  if (position == (int)s.length())
    return tok;
  if (isdigit(s[position]))
    return readInteger(s);  // прочитанное целое число
  else  // иначе возвращаем tok, содержащую текущий символ
    return tok += s[position++];
}

std::string readInteger(std::string s) {  // функция считывает все цифры из
                               // строки s и возвращает их в виде строки
  std::string res = "";  // строка, которая будет содержать результат
  while (position < (int)s.length() && isdigit(s[position]))
    res += s[position++];  // добавить символ к результату и перейти
                           // к следующему символу
  return res;  // возвращаем полученную строку
}

void readWhiteSpace(std::string s) {
  while (position < (int)s.length() && isspace(s[position]))
    position++;
}

int presenceVarCheck(
    std::string str) { // функция проверяет наличие переменной с именем str в
                       // списке переменных, и если ее нет, то добавляет в
                       // список новую переменную с именем str и адресом
                       // lastCell - variablesNum
  for (int i = 0; i < variablesNum; i++) {
    if (variables[i].getTit() == str) {
      return variables[i].getAddr();
    }
  }
  variables.emplace_back(Variable(
      str, lastCell - variablesNum));  // добавляем новую переменную в список
  variablesNum++;
  return variables[variablesNum - 1].getAddr();
}

int main(int argc, char *argv[]) {
  std::cout << std::endl;
  if (argc != 3) {
    std::cout << "Неверное количество аргументов!" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::string fileName = argv[1];
  size_t slashPos = fileName.find_last_of("/");
  if (slashPos != std::string::npos) {
    fileName = fileName.substr(slashPos + 1);
  }
  size_t dotPos = fileName.find_last_of(".");
  if (dotPos != std::string::npos) {
    fileName = fileName.substr(0, dotPos);
  }
  fileName += ".sa";
  std::string term_command = "mv temp_asmb.sa project/assembler/" + fileName;
  if (translate_bsc(argv[1], (char *)"temp_asmb.sa") == 0) {
    system(term_command.c_str());
    std::cout << "Трансляция в SimpleAssembler завершена!" << std::endl;
    std::cout
        << "Нажмите Enter для трансляции из SimpleAssembler в машинный код";
    std::cin.get();
    std::cout << std::endl;
    system("make sa");
  }
  return 0;
}
