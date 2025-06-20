#include <iostream>
#include <string>
#include <cmath>
#include <limits>
#include "transaction.hxx"

int main(int argc, char** argv) {
	std::cout << "Welcome to LedgeLite." << std::endl;
	Transaction transaction;
	std::string filePath;

	std::cout << "Enter file address of SQLite DB (e.g., testData/database.db): ";
	std::getline(std::cin, filePath);

	if (filePath.empty()) {
		filePath = "testData/database.db";  
	}

	transaction.connectDatabase(filePath);  

	while (true) {
		std::cout
			<< "Enter Choice\n"
			<< "1 - Show Previous Transactions\n"
			<< "2 - Add new entry\n"
			<< "3 - Remove an Entry\n"
			<< "4 - Edit an Entry\n"
			<< "5 - Exit\n"
			<< std::flush;
		char choice;
		std::cin >> choice;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		switch (choice) {
		case '1':
			transaction.showPrevious();
			break;
		case '2':
			transaction.addEntry();
			break;
		case '3':
			transaction.removeEntry();
			break;
		case '4':
			transaction.editEntry();
			break;
		case '5':
			std::cout << "Thanks for using LedgeLite." << std::endl;
			return 0;
		default:
			std::cout << "Invalid Choice!" << std::endl;
		}
	}

	return 0;
}






