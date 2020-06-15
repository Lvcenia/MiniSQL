#pragma once
#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include "Generals.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

class StatementBlock; // ����sql�����Ļ��࣬�Ը���sql���ʵ�����Ҽ���ִ��

// �����࣬�����������ã�����ִ��sql��sql�ļ�����ӡ���
class Interpreter
{
public:
	Interpreter() {};
	~Interpreter() {};
	void executeSql(const std::string& sql); // ��ִ��,���������vector<QueryResult>��,��Ϊִ��һ���ļ���Ķ��������ж��������������vector
	void parse(const std::string& sql); // ���﷨����
	void check(); // �����
	void execute(); // ִ��
	void print(); // �������ִ�н��
	//void readInput(const std::string& s);
	void executeFile(const std::string& fileName); // ִ���ļ�
	std::shared_ptr<StatementBlock>& gettPtr() { return tptr; } 
	std::vector<std::shared_ptr<StatementBlock>>& getVsb() { return vStatementBlock; }
	//void setRecordBuffer(RECORDBUFFER& rb) { this->rb = &rb; }
	//RECORDBUFFER& getRecordBuffer() { return *rb; }
private:
	typedef std::string::iterator Iterator;
	//std::vector<std::string> split(std::string s, std::string::value_type c);

	// ����������
	void createTableParser(Iterator& begin, Iterator end);
	void createIndexParser(Iterator& begin, Iterator end);
	void dropTableParser(Iterator& begin, Iterator end);
	void dropIndexParser(Iterator& begin, Iterator end);
	void selectParser(Iterator& begin, Iterator end);
	void insertParser(Iterator& begin, Iterator end);
	void deleteParser(Iterator& begin, Iterator end);
	void quitParser(Iterator& begin, Iterator end);
	void execfileParser(Iterator& begin, Iterator end);

private:
	std::vector<std::shared_ptr<StatementBlock>> vStatementBlock;// ���������Ϣ���ָ�뼯
	std::string tmpStoredSql; // �ݴ��һ��sql���
	//RECORDBUFFER* rb;
	std::shared_ptr<StatementBlock> tptr; // �ݴ��һ��sql����Ӧ����

	vector<QueryResult> rets;  // ��ѯ�Ľ��, һ���ļ��ж���sql�����ж�����
};
#endif
