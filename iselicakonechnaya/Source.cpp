#include <iostream>
#include <Windows.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

//структура слова с подсказкой
struct word_hint {
	string word;	//слово
	string topic;	//тема
	string hint;	//подсказка
};

//структура игрока
struct player {
	string name;
	int score;
};

//функция сравнения двух игроков для сортировки таблицы рекордов
bool players_cmp(player p1, player p2) {
	return p1.score > p2.score;
}

//структура игры
struct game {
	//имя для таблицы рекордов
	player curPlayer;
	vector<player> players;
	string word;				//текущее слово, которое видит игрок игрока
	vector <word_hint> words;	//базовый массив слов с подсказками
	string mistakes;			//кол-во ошибок игрока

	//конструктор по умолчанию для игры
	game() {
		words = vector<word_hint>();
		word = "";
		mistakes = "";
		curPlayer.score = 0;
		cout << "Введите имя: "; cin >> curPlayer.name;
		loadFromFiles();
	}

	//функция считывает из файла все базовые слова и подсказки
	void loadFromFiles() {
		words.clear();						//очищаем базовый массив слов и подсказок
		ifstream fin("C:\\words_hints.txt");	//пытаемся открыть файл с указанным названием
		word_hint temp;						//буферная переменная для заполнения базового массива слов с подсказками
		if (fin.is_open()) {				//если удалось открыть файл
			while (!fin.eof()) {			//пока не конец файла
				fin >> temp.word;			//считываем само слово
				fin >> temp.topic;			//считываем тему слова
				getline(fin, temp.hint);	//далее считываем до конца строки позсказку
				words.push_back(temp);		//добавляем данные в базу
			}
		}
		fin.close();

		//аналогично заполняем таблицу рекордов
		players.clear();
		ifstream finr("records.in");
		player player_temp;
		if (finr.is_open()) {
			while (!finr.eof()) {
				finr >> player_temp.name;
				if (finr.eof()) break;
				finr >> player_temp.score;
				players.push_back(player_temp);	//добавляем данные в базу
			}
		}
		finr.close();
	}

	void saveRecords() {
		ofstream fout("records.in");

		for (player p : players) {
			fout << p.name << ' ' << p.score << '\n';
		}
	}

	//функция вывода виселицы в соответствтие с кол-во ошибок
	void printGallows(size_t m) {
		cout << "---------------\n";
		cout << "              |\n";
		if (m >= 1 && m <= 2)	cout << "              O\n"; else
			if (m == 3)				cout << "             \\O\n"; else
				if (m > 3)				cout << "             \\O/\n"; else cout << '\n';
		if (m >= 2)				cout << "              |\n"; else cout << '\n';
		if (m == 5)				cout << "             /\n"; else
			if (m == 6)				cout << "             / \\\n"; else cout << '\n';
		cout << '\n';
	}

	void printRecords() {
		cout << "\n\tТаблица рекордов\n";
		printf("+----------------+--------+\n");
		printf("|%16s|%8s|\n", "Имя", "Счёт");
		printf("+----------------+--------+\n");
		for (player p : players) {
			printf("|%16s|%8d|\n", p.name.c_str(), p.score);
			printf("+----------------+--------+\n");
		}
	}

	//функция вывода состояния игры для слова с индексом i из базы, с учетом уровня сложности
	void printState(int ind, int lvl) {
		cout << '\n';
		printGallows(mistakes.size());

		//выводим тему
		if (lvl < 3) {
			cout << "Тема раунда: " << words[ind].topic << '\n';
		}

		//выводим слово
		cout << "Слово: ";
		for (char c : word) { cout << c << " "; } //перебираем все символы и выводим
		cout << '\n'; //обозначаем конец строки

		//выводим ошибки
		cout << "Ошибки (" << mistakes.size() << "): ";
		for (size_t i = 0; i < mistakes.size(); ++i) {
			cout << mistakes[i] << ' ';
		}
		cout << '\n';

		//выводим напоминание о подсказке
		if (lvl < 2) {
			cout << "Чтобы использовать подсказку введите '?'\n";
		}
	}

	char toUpper(char c) {
		if (c <= -1 && c >= -32) c -= 32;
		return c;
	}

	//главная функция игрового процесса
	void play() {
		srand(time(NULL));											//устанавливаем случайное значение для генерации
		int ind = rand() % int(words.size());						//случайно выбираем слово из базы

		mistakes = "";												//очищаем все ошибки
		word = "";													//очищаем текущее слово пользователя
		for (size_t i = 0; i < words[ind].word.size(); ++i) {		//в цикле заполняем вместо всех букв знаки _
			word += '_';
		}


		int lvl;//уровень сложности
		cout << "добро пожаловать в игру Виселица! Ваша задача угадать слово, заданное программой.Для начала вам нужно выбрать уровень сложности. В 1 уровне вам будем задана тема раунда и возможность использовать одну подсказку. Во 2 уровне вам будет дана только тема раунда. В 3 уровне никаких подсказок нет. Если вы ошибетесь , то рисунок виселицы дополнится одним элементом. В результате 6 ошибок вы проиграете , а если допустите меньше ошибок и угадаете слово , то вы выиграете! Удачной игры! " << endl;
		cout << "(1:легко 2:средне 3:сложно)\n";
		cout << "Введите уровень сложности: ";
		cin >> lvl;													//вводится уровень
		bool check = true;
		bool hint = false;
		string input;												//строка, которую вводит пользователь

		while (true) {
			system("cls");
			printState(ind, lvl);									//выводим текущую информацию об игре
			if (!check) {
				cout << "Нужно ввести русскую букву, которой еще не было!\n";
			}
			if (hint) {
				cout << "Подсказка:" << words[ind].hint << '\n';	//выводим подсказку
			}
			cout << "Буква: "; cin >> input;						//пользователь вводит букву
			if (lvl == 1 && input[0] == '?') {						//если пользователь запрашивает подсказку
				hint = true;
				continue;
			}

			//проверка на корректный ввод
			check = true;
			if ((input[0] > -1 || input[0] < -64)) check = false;	//если ввели не букву, ввод не корректный
			for (size_t i = 0; i < word.size(); ++i) {
				if (toUpper(input[0]) == word[i]) check = false;	//если ввели уже открытую букву
			}
			for (size_t i = 0; i < mistakes.size(); ++i) {
				if (toUpper(input[0]) == mistakes[i]) check = false;	//если ввели букву, которая уже числится в ошибках
			}
			if (!check) {
				continue;
			}
			/////////////////////////////


			bool miss = true;
			for (size_t i = 0; i < words[ind].word.size(); ++i) {	//перебираем все буквы соответствующего слова из базы
				if (toUpper(input[0]) == words[ind].word[i]) {		//если нашли совпадение буквы пользователя с i'ой буквой слова
					word[i] = words[ind].word[i];					//открываем эти буквы в слове пользователя
					miss = false;									//если нашли хотя бы одно совпадение, значит ошибки не было
				}
			}

			if (miss) {												//если соответствующих букв в слове нет, добавляем к ошибкам
				mistakes += toUpper(input[0]);
			}

			//если игрок совершает 6 ошибок, то он проиграл
			if (mistakes.size() == 6) {
				cout << "Вы проиграли!\n";
				printGallows(6);
				cout << "Загаданное слово: " << words[ind].word;
				break;
			}

			//игрок победил, если в слове win не осталось пустых букв _
			if (word.find('_') == string::npos) {
				cout << "Вы выйграли!\n";
				cout << "Загаданное слово: " << words[ind].word << '\n';
				cout << "Ошибкок: " << mistakes.size() << '\n';
				curPlayer.score = (6 - mistakes.size()) * lvl;	//подсчёт очков
				players.push_back(curPlayer);
				sort(players.begin(), players.end(), players_cmp);
				break;
			}
		}

		printRecords();
		saveRecords();
	}
};

int main() {
	//устанавливаем кодировку для кириллицы
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	//создаем игру и запускаем ее
	game g;
	g.play();

	return 0;
}