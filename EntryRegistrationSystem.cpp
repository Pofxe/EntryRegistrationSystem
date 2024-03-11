#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include <ctype.h>
#include <chrono>

// Макрос для пути к файлу
#define FILE "D:\InfoUsers.txt"

using namespace std;

// Структура для хранения данных пользователя
struct Users
{
	Users() = default;

	Users(string email_, string login_, string password_, string registration_time_, string last_entry_time_) :
		email(email_), login(login_), password(password_), registration_time(registration_time_), last_entry_time(last_entry_time_){}

	string email = ""s;
	string login = ""s;
	string password = ""s;
	string registration_time = ""s;
	string last_entry_time = ""s;
};

//Функция проверки корректности почты
bool IsValidEmail(const string& email)
{
	regex emailRegex("([a-zA-Z0-9]+)(\\.[a-zA-Z0-9]+)*@([a-zA-Z0-9]+)(\\.[a-zA-Z0-9]+)*"s);
	return regex_match(email, emailRegex);
}

//Функция проверки соответствия пароля заданным требованиям
bool IsValidPassword(const string& password)
{
	bool has_special_char = false;
	bool has_upper_case = false;
	bool has_lower_case = false;

	if (password.length() < 10)
	{
		return false;
	}

	for (char c : password)
	{
		if (!isalnum(c))
		{
			has_special_char = true;
		}
		if (isupper(c))
		{
			has_upper_case = true;
		}
		if (islower(c))
		{
			has_lower_case = true;
		}
	}
	return has_special_char && has_upper_case && has_lower_case;
}

//Функция для получения текущей даты и времени
string GetCurrentTime()
{
	std::chrono::zoned_time now{ std::chrono::current_zone(), std::chrono::system_clock::now() };

	ostringstream oss;
	oss << format("{:%d.%m.%Y-%H:%M}", now.get_local_time());
	string nowString = oss.str();

	return nowString;
}

// Функция для добавления нового пользователя
Users AddCustomer()
{
	string email = ""s;
	do
	{
		cout << "Enter your email : "s;
		getline(cin, email);
		cin.ignore();
	} while (!IsValidEmail(email));

	string password = ""s;
	do
	{
		cout << "Enter your password : "s;
		getline(cin, password);
		cin.ignore();
	} while (!IsValidPassword(password));

	string registration_time = GetCurrentTime();

	string last_entry_time = "NULL"s;

	string login = ""s;
	cout << "Enter your login : "s;
	getline(cin, login);
	cin.ignore();
	
	//Если логин не задан пользователем, будем использовать символы идущие до @
	if (login == "" || login.empty())
	{
		login = email.substr(0, email.find('@'));
	}

	return { email, login, password, registration_time, last_entry_time };
}

// Функция для записи данных пользователя в файл
void WriteToFile(const vector<Users>& users)
{
	ofstream file(FILE);

	if (file.is_open())
	{
		for (const auto& user : users)
		{
			file << user.login << " " << user.email << " " << user.password << " " << user.registration_time << " " << user.last_entry_time << endl;
		}
	}
	else
	{
		throw "User data could not be recorded!"s;
	}
	file.close();
}

//Функция получения записей из файла и записи их в вектор
vector<Users> GetDataFromFile()
{
	vector<Users> users = {};

	ifstream file(FILE);

	if (file.is_open())
	{
		Users user;

		while (file >> user.login >> user.email >> user.password >> user.registration_time >> user.last_entry_time)
		{
			users.push_back(user);
		}
		file.close();
	}
	else
	{
		throw("Data could not be retrieved from the file!"s);
	}

	return users;
}

//проверки корректности данных для входа
bool FindUserByLogin(const string& target_email, const string& target_password, const vector<Users>& user)
{
	for (const auto& us : user)
	{
		if (us.email == target_email && us.password == target_password)
		{
			return true;
		}
	}

	return false;
}

//Функция получения логина пользователя
string GetLogin(const string& target_email, const vector<Users>& user)
{
	for (const auto& us : user)
	{
		if (us.email == target_email)
		{
			return us.login;
		}
	}
	return ""s;
}

//Функция обновления времени последнего входа
void UpdateLastEntryTime(const string& target_login)
{
	vector<Users> users = GetDataFromFile();

	for (auto& user : users)
	{
		if (user.login == target_login)
		{
			user.last_entry_time = GetCurrentTime();
		}
	}
	WriteToFile(users);
}

//Функция для основного меню
void menu()
{
	int user_choice = 0;
	bool yes_no = true;

	Users user;

	do
	{
		cout << "1.Registration"s << endl;
		cout << "2.Input"s << endl;
		cout << "3.Output"s << endl;

		cout << "Select an item : "s;
		cin >> user_choice;
		cout << endl;
		cin.ignore();

		if (user_choice == 1)
		{
			vector<Users> users_ = GetDataFromFile();
			user = AddCustomer();
			users_.push_back(user);
			WriteToFile(users_);
		}

		if (user_choice == 2)
		{
			vector<Users> users = GetDataFromFile();

			cout << "Enter your email : "s;
			string email = ""s;
			getline(cin, email);

			cout << "Enter your password : "s;
			string password = ""s;
			getline(cin, password);

			if (IsValidPassword(password) && IsValidEmail(email))
			{
				if (FindUserByLogin(email, password, users))
				{
					string login = GetLogin(email, users);

					system("cls");
					cout << endl << "You have successfully logged in as - "s << login << endl << endl;

					UpdateLastEntryTime(login);
				}
				else
				{
					cout << "The user was not found"s << endl;
				}
			}
			else
			{
				cout << "Invalid email or password"s << endl;
			}
		}

		if (user_choice == 3)
		{
			yes_no = false;
			break;
		}
	} while (yes_no);
}

int main()
{
	menu();
}