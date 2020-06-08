#pragma once
#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include "Generals.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

class StatementBlock;


class Interpreter
{
public:
	Interpreter() {};
	~Interpreter() {};
	QueryResult executeSql(const std::string& sql); // 总执行
	void parse(const std::string& sql); // 总语法分析
	void check(); // 语句检查
	QueryResult execute(); // 执行
	//void print();
	//void readInput(const std::string& s);
	void executeFile(const std::string& fileName); // 执行文件
	std::shared_ptr<StatementBlock>& gettPtr() { return tptr; } 
	std::vector<std::shared_ptr<StatementBlock>>& getVsb() { return vStatementBlock; }
	//void setRecordBuffer(RECORDBUFFER& rb) { this->rb = &rb; }
	//RECORDBUFFER& getRecordBuffer() { return *rb; }
private:
	typedef std::string::iterator Iterator;
	//std::vector<std::string> split(std::string s, std::string::value_type c);

	// 各类语句分析
	void createTableParser(Iterator& begin, Iterator end);
	void createIndexParser(Iterator& begin, Iterator end);
	void dropTableParser(Iterator& begin, Iterator end);
	void dropIndexParser(Iterator& begin, Iterator end);
	void selectParser(Iterator& begin, Iterator end);
	void insertParser(Iterator& begin, Iterator end);
	void deleteParser(Iterator& begin, Iterator end);
	void quitParser(Iterator& begin, Iterator end);
	void execfileParser(Iterator& begin, Iterator end);
	// 各类语句信息类的指针集
	std::vector<std::shared_ptr<StatementBlock>> vStatementBlock;
	std::string tmpStoredSql;
	//RECORDBUFFER* rb;
	std::shared_ptr<StatementBlock> tptr;
};
#endif
