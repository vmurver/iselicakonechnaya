#include <iostream>
#include <Windows.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;
struct word_hint {
	string word;	
	string topic;	
	string hint;	
};
struct player {
	string name;
	int score;
};
bool players_cmp(player p1, player p2) {
	return p1.score > p2.score;
}
struct game {
	player curPlayer;
	vector<player> players;
	string word;				
	vector <word_hint> words;	
	string mistakes;			
	game() {
		words = vector<word_hint>();
		word = "";
		mistakes = "";
		curPlayer.score = 0;
		cout << "Введите имя: "; cin >> curPlayer.name;
		loadFromFiles();
	}
	void loadFromFiles() {
		words.clear();						
		ifstream fin("C:\\words_hints.txt");	
		word_hint temp;						
		if (fin.is_open()) {				
			while (!fin.eof()) {			
				fin >> temp.word;			
				fin >> temp.topic;			
				getline(fin, temp.hint);	
				words.push_back(temp);		
			}
		}
		fin.close();
		players.clear();
		ifstream finr("records.in");
		player player_temp;
		if (finr.is_open()) {
			while (!finr.eof()) {
				finr >> player_temp.name;
				if (finr.eof()) break;
				finr >> player_temp.score;
				players.push_back(player_temp);	
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
	void printState(int ind, int lvl) {
		cout << '\n';
		printGallows(mistakes.size());

		if (lvl < 3) {
			cout << "Тема раунда: " << words[ind].topic << '\n';
		}
		cout << "Слово: ";
		for (char c : word) { cout << c << " "; } 
		cout << '\n';
		cout << "Ошибки (" << mistakes.size() << "): ";
		for (size_t i = 0; i < mistakes.size(); ++i) {
			cout << mistakes[i] << ' ';
		}
		cout << '\n';
		if (lvl < 2) {
			cout << "Чтобы использовать подсказку введите '?'\n";
		}
	}

	char toUpper(char c) {
		if (c <= -1 && c >= -32) c -= 32;
		return c;
	}
	void play() {
		srand(time(NULL));											
		int ind = rand() % int(words.size());						
		mistakes = "";												
		word = "";													
		for (size_t i = 0; i < words[ind].word.size(); ++i) {		
			word += '_';
		}


		int lvl;//уровень сложности
		cout << "добро пожаловать в игру Виселица! Ваша задача угадать слово, заданное программой.Для начала вам нужно выбрать уровень сложности. В 1 уровне вам будем задана тема раунда и возможность использовать одну подсказку. Во 2 уровне вам будет дана только тема раунда. В 3 уровне никаких подсказок нет. Если вы ошибетесь , то рисунок виселицы дополнится одним элементом. В результате 6 ошибок вы проиграете , а если допустите меньше ошибок и угадаете слово , то вы выиграете! Удачной игры! " << endl;
		cout << "(1:легко 2:средне 3:сложно)\n";
		cout << "Введите уровень сложности: ";
		cin >> lvl;													
		bool check = true;
		bool hint = false;
		string input;												

		while (true) {
			system("cls");
			printState(ind, lvl);									
			if (!check) {
				cout << "Нужно ввести русскую букву, которой еще не было!\n";
			}
			if (hint) {
				cout << "Подсказка:" << words[ind].hint << '\n';	
			}
			cout << "Буква: "; cin >> input;						
			if (lvl == 1 && input[0] == '?') {						
				hint = true;
				continue;
			}

			//проверка на корректный ввод
			check = true;
			if ((input[0] > -1 || input[0] < -64)) check = false;	
			for (size_t i = 0; i < word.size(); ++i) {
				if (toUpper(input[0]) == word[i]) check = false;	
			}
			for (size_t i = 0; i < mistakes.size(); ++i) {
				if (toUpper(input[0]) == mistakes[i]) check = false;	
			}
			if (!check) {
				continue;
			}
			/////////////////////////////


			bool miss = true;
			for (size_t i = 0; i < words[ind].word.size(); ++i) {	
				if (toUpper(input[0]) == words[ind].word[i]) {		
					word[i] = words[ind].word[i];					
					miss = false;									
				}
			}

			if (miss) {												
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
