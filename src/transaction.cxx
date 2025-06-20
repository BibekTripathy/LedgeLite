#include <cctype>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <regex>
#include <sqlite3.h>
#include "transaction.hxx"

void Transaction::connectDatabase(const std::string& filePath) {  // 🔁 Changed from fetchData
	dbPath = filePath;
	int ReturnCode = sqlite3_open(dbPath.c_str(), &db);
	if (ReturnCode) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << "\n";
		exit(1);
	}

	std::string createTable = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            counterparty TEXT,
            amount TEXT,
            date TEXT,
            time TEXT,
            category TEXT,
            notes TEXT
        );
    )";
	executeSQL(createTable);
}

void Transaction::executeSQL(const std::string& query) {
	char* errMsg = nullptr;
	int ReturnCode = sqlite3_exec(db, query.c_str(), 0, 0, &errMsg);
	if (ReturnCode != SQLITE_OK) {
		std::cerr << "SQL error: " << errMsg << "\n";
		sqlite3_free(errMsg);
	}
}

int Transaction::printCallback(void* NotUsed, int argc, char** argv, char** azColName) {
	for (int i = 0; i < argc; i++) {
		std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << " | ";
	}
	std::cout << "\n";
	return 0;
}

void Transaction::addEntry() {
	std::string counterparty{ "" }, amount{ "" }, date{ "" }, time{ "" }, category{ "" }, notes{ "" };
	char choice{ '\0' };
	std::cout << "Payment or Reception? [S/R]: " << std::flush;
	std::cin >> choice;
	std::cin.ignore();
	std::cout << "\n";

	while (choice != 'S' && choice != 'R' && choice != 's' && choice != 'r') {
		std::cerr << "Invalid input, please enter either S or R: ";
		std::cin >> choice;
		if (std::cin.eof()) {
			std::cerr << "\nEnd of input detected. Exiting.\n";
			break;
		}
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "\n";
	}

	std::cout << "Please input the following fields..." << std::endl;
	std::cout << "Name of Counterparty: " << std::flush;
	std::getline(std::cin, counterparty);
	std::cout << "Transacted Amount: " << std::flush;
	std::getline(std::cin, amount);
	std::cout << "Date of Transaction: " << std::flush;
	std::getline(std::cin, date);
	while (!std::regex_match(date, std::regex(R"(^(0[1-9]|[12][0-9]|3[01])\.(0[1-9]|1[0-2])\.(1|2)\d{3}$)"))) {
		std::cout << "The entered date is invalid, please enter again: " << std::flush;
		std::getline(std::cin, date);
	}
	std::cout << "Time of Transaction: " << std::flush;
	std::getline(std::cin, time);
	std::cout << "Transaction Category: " << std::flush;
	std::getline(std::cin, category);
	std::cout << "Additional Notes: " << std::flush;
	std::getline(std::cin, notes);
	std::cout << "\n";

	std::cout
		<< "Entry has been added..."
		<< "\nSummary:"
		<< "\nName of Counterparty: " << counterparty
		<< "\nTransacted Amount: " << amount
		<< "\nDate of Transaction: " << date
		<< "\nTime of Transaction: " << time
		<< "\nTransaction Category: " << category
		<< "\nAdditional Notes: " << notes
		<< "\n" << std::endl;

	std::stringstream ss;
	ss << "INSERT INTO transactions (counterparty, amount, date, time, category, notes) VALUES ("
		<< "'" << counterparty << "', "
		<< "'" << amount << "', "
		<< "'" << date << "', "
		<< "'" << time << "', "
		<< "'" << category << "', "
		<< "'" << notes << "');";
	executeSQL(ss.str());
}

void Transaction::showPrevious() {
	std::cout << "Previous Transactions:\n";
	std::string query = "SELECT id, counterparty, amount, date, time, category, notes FROM transactions;";
	sqlite3_exec(db, query.c_str(), printCallback, 0, nullptr);
	std::cout << "\n";
}

void Transaction::removeEntry() {
	std::string serialStr{ "" };
	int serialNo{ 0 }, errorSwitch{ 0 };

	while (true) {
		if (!errorSwitch) {
			std::cout << "Enter the serial no. of the entry you would like to edit: " << std::flush;
		}
		else {
			std::cout << "Please enter a valid serial no.: " << std::flush;
		}
		std::getline(std::cin, serialStr);

		if (!std::all_of(serialStr.begin(), serialStr.end(), ::isdigit)) {
			std::cerr << "Please enter digits only.\n";
			errorSwitch = 1;
			continue;
		}

		serialNo = std::stoi(serialStr);

		if (serialNo < 1) {
			std::cerr << "Serial number out of range.\n";
			errorSwitch = 1;
			continue;
		}

		break;
	}
	std::stringstream ss;
	ss << "DELETE FROM transactions WHERE id = " << serialNo << ";";
	executeSQL(ss.str());
	// serialNo = std::stoi(serialStr);
	// database.erase(database.begin() + (serialNo - 1));
	std::cout << "Data at serial no. " << serialNo << " has been erased successfully.\n" << std::endl;
	Transaction::showPrevious();
}

void Transaction::editEntry() {
	std::string serialStr;
	int serialNo = 0, errorSwitch = 0;

	while (true) {
		std::cout << (errorSwitch ? "Please enter a valid serial no.: " : "Enter the serial no. of the entry you would like to edit: ");
		std::getline(std::cin, serialStr);

		if (!std::all_of(serialStr.begin(), serialStr.end(), ::isdigit)) {
			std::cerr << "Please enter digits only.\n";
			errorSwitch = 1;
			continue;
		}

		serialNo = std::stoi(serialStr);
		if (serialNo < 1) {
			std::cerr << "Serial number out of range.\n";
			errorSwitch = 1;
			continue;
		}

		break;
	}

	std::string field;
	std::cout << "Enter the name of field you want to edit (counterparty, amount, date, time, category, notes): ";
	std::getline(std::cin, field);
	std::transform(field.begin(), field.end(), field.begin(), ::tolower);

	std::string column;
	if (field == "counterparty" || field == "amount" || field == "date" ||
		field == "time" || field == "category" || field == "notes") {
		column = field;
	}
	else {
		std::cout << "Invalid field.\n";
		return;
	}

	std::cout << "Enter the new data: ";
	std::string newData;
	std::getline(std::cin, newData);

	std::stringstream ss;
	ss << "UPDATE transactions SET " << column << " = '" << newData << "' WHERE id = " << serialNo << ";";
	executeSQL(ss.str());
}

// void Transaction::editEntry() {
// 	std::string serialStr{ "" };
// 	int serialNo{ 0 }, errorSwitch{ 0 };

// 	while (true) {
// 		if (!errorSwitch) {
// 			std::cout << "Enter the serial no. of the entry you would like to edit: " << std::flush;
// 		}
// 		else {
// 			std::cout << "Please enter a valid serial no.: " << std::flush;
// 		}
// 		std::getline(std::cin, serialStr);

// 		if (!std::all_of(serialStr.begin(), serialStr.end(), ::isdigit)) {
// 			std::cerr << "Please enter digits only.\n";
// 			errorSwitch = 1;
// 			continue;
// 		}

// 		serialNo = std::stoi(serialStr);

// 		if (serialNo < 1 || serialNo > static_cast<int>(database.size())) {
// 			std::cerr << "Serial number out of range.\n";
// 			errorSwitch = 1;
// 			continue;
// 		}

// 		break;
// 	}

// 	std::string field;
// 	std::cout << "Enter the name of field you want to edit (counterparty, amount, date, time, category, notes): " << std::flush;
// 	std::getline(std::cin, field);

// 	std::transform(field.begin(), field.end(), field.begin(), ::tolower);
// 	// serialNo = std::stoi(serialStr);
// 	if (field == std::string("counterparty")) editField(0, serialNo);
// 	else if (field == std::string("amount")) editField(1, serialNo);
// 	else if (field == std::string("date")) editField(2, serialNo);
// 	else if (field == std::string("time")) editField(3, serialNo);
// 	else if (field == std::string("category")) editField(4, serialNo);
// 	else if (field == std::string("notes")) editField(5, serialNo);
// 	else std::cout << "Invalid choice\n";
// }

// void Transaction::editField(int field, int serialNo) {
// 	std::string newData{ "" };
// 	std::cout << "Enter the new data: " << std::flush;
// 	std::getline(std::cin, newData);

// 	switch (field) {
// 	case 0: {
// 		database[serialNo - 1].counterparty = newData;
// 		std::cout << "Data has been changed successfully\n" << std::endl;
// 		break;
// 	}
// 	case 1: {
// 		database[serialNo - 1].amount = newData;
// 		std::cout << "Data has been changed successfully\n" << std::endl;
// 		break;
// 	}
// 	case 2: {
// 		database[serialNo - 1].date = newData;
// 		std::cout << "Data has been changed successfully\n" << std::endl;
// 		break;
// 	}
// 	case 3: {
// 		database[serialNo - 1].time = newData;
// 		std::cout << "Data has been changed successfully\n" << std::endl;
// 		break;
// 	}
// 	case 4: {
// 		database[serialNo - 1].category = newData;
// 		std::cout << "Data has been changed successfully\n" << std::endl;
// 		break;
// 	}
// 	case 5: {
// 		database[serialNo - 1].notes = newData;
// 		std::cout << "Data has been changed successfully\n" << std::endl;
// 		break;
// 	}
// 	default: {
// 		std::cout << "Invalid choice\n";
// 	}
// 	}
// }

// void Transaction::fetchData(const std::string& filePath) {
// 	std::cout << "Database Address: " << filePath << std::endl;
// 	std::cout << "Processing data..." << std::endl;

// 	std::ifstream file(filePath);
// 	if (!file.is_open()) {
// 		std::cerr << "Error opening file: " << filePath << "\n";
// 		/* Extend this further to include further user operation in case of file error. */
// 	}

// 	std::string line{ "" };
// 	std::getline(file, line); /* Skip Header */
// 	while (std::getline(file, line)) {
// 		Transaction::dataRow row;
// 		std::stringstream ss(line);
// 		std::string amountStr{ "" };

// 		if (!std::getline(ss, row.counterparty, ',')) continue;
// 		if (!std::getline(ss, row.amount, ',')) continue;
// 		if (!std::getline(ss, row.date, ',')) continue;
// 		if (!std::getline(ss, row.time, ',')) continue;
// 		if (!std::getline(ss, row.category, ',')) continue;
// 		if (!std::getline(ss, row.notes, ',')) continue;
// 		/* Implement actionable measures for corrupt databases */

// 		database.push_back(row);
// 	}
// 	std::cout << "\n";

// 	Transaction::showPrevious();

// 	file.close();
// }
// void Transaction::writeData(const std::string& filePath) {
// 	std::ofstream file(filePath);
// 	if (!file.is_open()) {
// 		std::cerr << "Error opening file: " << filePath << "\n";
// 		/* Extend this further to include further user operation in case of file error. */
// 	}
// 
// 	file << "counterparty,amount,date,time,category,notes\n";
// 
// 	for (const auto& row : database) {
// 		file
// 			<< row.counterparty << ","
// 			<< row.amount << ","
// 			<< row.date << ","
// 			<< row.time << ","
// 			<< row.category << ","
// 			<< row.notes << "\n";
// 	}
// 
// 	file.close();
// }
