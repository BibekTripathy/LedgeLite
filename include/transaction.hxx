#ifndef TRANSACTION_HXX
#define TRANSACTION_HXX

#include <string>
//#include <vector>
#include <sqlite3.h>
class Transaction {
private:
	sqlite3* db;
	std::string dbPath;
public:
	void addEntry();
	void removeEntry();
	void editEntry();
	void showPrevious();
	void connectDatabase(const std::string& filePath);
private:
	 void executeSQL(const std::string& query);
    static int printCallback(void* NotUsed, int argc, char** argv, char** azColName);
};

#endif // !TRANSACTION_HXX
