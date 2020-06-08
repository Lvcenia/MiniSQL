#include "Interpreter.h"
#include "StatementBlock.h"
#include "Functor.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "Exception.h"
//#include "CatalogManager.h"
//#include "API.h"

using namespace std;

typedef std::string::iterator Iterator;


 //sql 语句总执行入口
QueryResult Interpreter::executeSql(const string & sql) {
	parse(sql);
	check();
	return execute();
}
 //总语法分析
void Interpreter::parse(const string& sql) {

	string str = sql + ' ';
	Iterator begin = str.begin();
	Iterator end = str.end();
	string s = readWord(begin, end);
	if (s == "select")
		selectParser(begin, end);
	else if (s == "insert")
		insertParser(begin, end);
	else if (s == "delete")
		deleteParser(begin, end);
	else if (s == "quit")
		quitParser(begin, end);
	else if (s == "create") {
		s = readWord(begin, end);
		if (s == "table")
			createTableParser(begin, end);
		else if (s == "index")
			createIndexParser(begin, end);
		else throw GrammarError("Syntax error in create opertion");
	}
	else if (s == "drop") {
		s = readWord(begin, end);
		if (s == "table")
			dropTableParser(begin, end);
		else if (s == "index")
			dropIndexParser(begin, end);
		else throw GrammarError("Syntax error in drop operation");
	}
	else if (s == "execfile")
		execfileParser(begin, end);
	else throw GrammarError("Undefined operation: " + s);


}
// //检查语句
//void Interpreter::check()
//{
//	bool flag = true;
//	string s;
//	for (auto& vsb : vStatementBlock) {
//		try {
//			//vsb->check();
//		}
//		catch (exception& e) {
//			s += string(e.what()) + "\n";
//			flag = false;
//		}
//	}
//
//	if (!flag) {
//		vStatementBlock.clear();
//		throw CatalogError(s.substr(0, s.size() - 1));
//	}
//}
// //执行语句
//QueryResult Interpreter::execute()
//{
//	try {
//		for (auto& vsb : vStatementBlock) {
//			vsb->execute();
//			if (vStatementBlock.size() == 1)
//				break;
//		}
//	}
//	catch (exception& e) {
//		cout << e.what();
//		vStatementBlock.clear();
//		throw runtime_error(e.what());
//	}
//	vStatementBlock.clear();
//}



// //各种语法分析
//
//void Interpreter::createIndexParser(Iterator& begin, Iterator end) {
//	string indexName = readWord(begin, end, IsVariableName());	//read index
//	readWord(begin, end, IsString("on"));	//read on
//	string tableName = readWord(begin, end, IsVariableName());	//read tableName
//	readWord(begin, end, IsString("("));	//read left brace
//	string attributeName = readWord(begin, end, IsVariableName());	//read attribute
//	readWord(begin, end, IsString(")"));	//read right brace 
//
//	readToEnd(begin, end);
//	shared_ptr<StatementBlock> pSB(new CreateIndexBlock(indexName, tableName, attributeName));
//	vStatementBlock.push_back(pSB);
//}
//
//void Interpreter::dropTableParser(Iterator& begin, Iterator end) {
//	string s = readWord(begin, end, IsVariableName()); //read table
//	shared_ptr<StatementBlock> pSB(new DropTableBlock(s));
//
//	readToEnd(begin, end);
//	vStatementBlock.push_back(pSB);
//}
//
//void Interpreter::dropIndexParser(Iterator& begin, Iterator end) {
//	string s = readWord(begin, end, IsVariableName()); //read index 
//	shared_ptr<StatementBlock> pSB(new DropIndexBlock(s));
//	readToEnd(begin, end);
//	vStatementBlock.push_back(pSB);
//}
//
//void Interpreter::selectParser(Iterator& begin, Iterator end) {
//	list<string> attributeList;
//
//	SelectBlock sb(this);
//
//	string s = readWord(begin, end, IsChar('*')); //read *
//	if (s.length() != 0) {
//		sb.setStar(true);
//	}
//	else {
//		while (true) {
//			s = readWord(begin, end, IsVariableName());//read attribute
//			attributeList.push_back(s);
//			s = readWord(begin, end, IsChar(','));
//			if (s.length() == 0) {
//				sb.setAttributes(attributeList);
//				break;
//			}
//		}
//	}
//
//	readWord(begin, end, IsString("from")); // read from
//	s = readWord(begin, end, IsVariableName()); // read table
//	sb.setTableName(s);
//	try {
//		s = readWord(begin, end);
//	}
//	//catch (EndOfString) {
//	catch (exception) {
//		shared_ptr<StatementBlock> pSB(new SelectBlock(sb));
//		vStatementBlock.push_back(pSB);
//		return;
//	}
//	//if (s != "where")throw GrammarError("illegal where");//read where
//	//list<Expression> expList = readExp(begin, end);// read exps
//	//sb.setExpressions(expList);
//
//	readToEnd(begin, end);
//	shared_ptr<StatementBlock> pSB(new SelectBlock(sb));
//	vStatementBlock.push_back(pSB);
//}
//
//void Interpreter::insertParser(Iterator& begin, Iterator end) {
//	vector<string> values;
//	readWord(begin, end, IsString("into")); //read into
//	string tableName = readWord(begin, end, IsVariableName()); //read tableName
//	readWord(begin, end, IsString("values"));//read values
//	readWord(begin, end, IsString("(")); //read left brace
//	while (true) {
//		string s = readWord(begin, end, IsChar('\''));
//		if (s.length() != 0) {
//			s = "\'" + readWord(begin, end, IsCharArray());
//			//readWord(begin, end, IsString("\'"));
//		}
//		else {
//			s = readWord(begin, end);
//		}
//		values.push_back(s);
//		s = readWord(begin, end, IsChar(')'));
//		if (s.length() != 0)break;
//		readWord(begin, end, IsString(","));
//	}
//	readToEnd(begin, end);
//	shared_ptr<StatementBlock> pSB(new InsertTableBlock(tableName, values));
//	vStatementBlock.push_back(pSB);
//}
//
//void Interpreter::deleteParser(Iterator& begin, Iterator end) {
//	readWord(begin, end, IsString("from"));  //read from
//	string s = readWord(begin, end, IsVariableName());  //read table
//	string tableName = s;
//
//	try {
//		s = readWord(begin, end);
//	}
//	//catch (EndOfString e) {
//	catch (exception) {
//			shared_ptr<StatementBlock> pSB(new DeleteBlock(tableName));
//		vStatementBlock.push_back(pSB);
//		return;
//	}
//	if (s != "where")
//		;
//		//throw GrammarError("illegal delete operation");
//	auto exps = readExp(begin, end);
//
//	readToEnd(begin, end);
//	shared_ptr<StatementBlock> pSB(new DeleteBlock(tableName, exps));
//	vStatementBlock.push_back(pSB);
//
//}
//
//void Interpreter::quitParser(Iterator& begin, Iterator end) {
//	try {
//		readWord(begin, end);
//	}
//	catch (exception) {
//	//catch (EndOfString e) {
//		shared_ptr<StatementBlock> pSB(new QuitBlock);
//		vStatementBlock.push_back(pSB);
//		return;
//	}
//	//throw GrammarError("Words are forbidden to be followed by quit");
//}
//
//void Interpreter::execfileParser(Iterator & begin, Iterator end)
//{
//	readWord(begin, end, IsString("\'"));
//	string s = readWord(begin, end, IsCharArray());
//	readToEnd(begin, end);
//	shared_ptr<StatementBlock> pSB(new execBlock(string(s.begin(), s.end() - 1), this));
//	vStatementBlock.push_back(pSB);
//}
//
//
//vector<string> Interpreter::split(string s, string::value_type c) {
//	stringstream ss(s);
//	vector<string> ret;
//	string tmp;
//	while (getline(ss, tmp, c)) {
//		ret.push_back(tmp);
//	}
//	return move(ret);
//}
//
//void Interpreter::readInput(const string & s)
//{
//	static bool inQuota = false;
//	auto iter1 = s.begin();
//	auto iter2 = iter1;
//	for (; iter2 != s.end(); iter2++) {
//		if (*iter2 == '\'') {
//			inQuota = !inQuota;
//		}
//		if (!inQuota) {
//			if (*iter2 == ';') {
//				tmpStoredSql += string(iter1, iter2);
//				try {
//					parse(tmpStoredSql);
//				}
//				catch (exception& e) {
//					//printOut(e.what());
//				}
//				check(); execute();
//				iter1 = iter2 + 1;
//				tmpStoredSql.clear();
//			}
//		}
//	}
//	tmpStoredSql += string(iter1, iter2);
//}
//
//void Interpreter::executeFile(const string & fileName)
//{
//	ifstream is(fileName);
//	if (!is.is_open()) {
//		throw runtime_error("file(" + fileName + ") can not be open");
//	}
//	vStatementBlock.clear();
//	string s;
//	string errorInfo;
//	int lineCount = 0;
//	bool inQuota = false;
//	bool errorFlag = false;
//	while (getline(is, s)) {
//		lineCount++;
//		auto iter1 = s.begin();
//		auto iter2 = iter1;
//		for (; iter2 != s.end(); iter2++) {
//			if (*iter2 == '\'') {
//				inQuota = !inQuota;
//			}
//			if (!inQuota) {
//				if (*iter2 == ';') {
//					tmpStoredSql += string(iter1, iter2);
//					try {
//						parse(tmpStoredSql);
//					}
//					catch (exception& e) {
//						stringstream ss;
//						string tmp;
//						ss << lineCount;
//						ss >> tmp;
//
//						errorInfo += "Line " + tmp + ":" + e.what() + '\n';
//						errorFlag = true;
//					}
//					iter1 = iter2 + 1;
//					tmpStoredSql.clear();
//				}
//			}
//		}
//		tmpStoredSql += string(iter1, iter2);
//	}
//	if (errorFlag)
//		throw runtime_error(errorInfo.substr(0, errorInfo.size() - 1));
//	check();
//	execute();
//}
//
//
//
////
////
////Interpreter::Interpreter()
////{
////	API::getAPIPtr();
////}
////
////
////void Interpreter::print()
////{
////	for (auto& vsb : vStatementBlock) {
////		vsb->print();
////	}
////}
