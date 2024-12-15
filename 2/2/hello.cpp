#include <iostream>
#include "CLI11.hpp"
using namespace std;

int main (int args, char* argv[]){
	CLI::App app{"Мое местоположение"};
	
	std::string userName = "странник";
	app.add_option("-n, --name", userName, "имя пользователя");

	std::string userYear = "2024";
	app.add_option("-y, --year", userYear ,"год рождения");

	CLI11_PARSE(app, args, argv);
	
	int year = stoi(userYear);
	auto t = time(nullptr);
	auto lt = localtime(&t);
	int currentYear = lt->tm_year + 1900;
	
	int age = currentYear - year;

	cout << "Привет, " << userName << endl << "ты родился в "<< year << endl << "тебе " << age << "лет" << endl;
	return 0;
}
