#pragma once
#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include "Generals.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

class StatementBlock; // 各种sql语句类的基类，对各种sql语句实现自我检查和执行

// 交互类，被主函数调用，分析执行sql或sql文件，打印结果
class Interpreter
{
public:
	Interpreter() {};
	~Interpreter() {};
	void executeSql(const std::string& sql); // 总执行,结果保存在vector<QueryResult>里,因为执行一个文件里的多条语句就有多条结果，所以用vector
	void parse(const std::string& sql); // 总语法分析
	void check(); // 语句检查
	void execute(); // 执行
	void print(); // 依次输出执行结果
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
	void check_execute();

private:
	std::vector<std::shared_ptr<StatementBlock>> vStatementBlock;// 各类语句信息类的指针集
	std::string tmpStoredSql; // 暂存的一条sql语句
	//RECORDBUFFER* rb;
	std::shared_ptr<StatementBlock> tptr; // 暂存的一个sql语句对应的类

	vector<QueryResult> rets;  // 查询的结果, 一个文件中多条sql语句会有多个结果
};
#endif
