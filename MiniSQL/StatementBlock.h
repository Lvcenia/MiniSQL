


#ifndef _STATEMENTBLOCK_H_
#define _STATEMENTBLOCK_H_

//#include "Definition.h"
#include <memory>
#include <string>
#include <list>
#include <vector>
#include "Generals.h"



/*����sql����Ӧ����
*
*���ã�����interpreter�����ı������﷨������sql��䴦���Ľ����
*      ���Ž��м�飬������ݿ����Ƿ��Ѵ��ڡ��Ƿ񲻴���...������
*      û������͵���api�еĸ��ֽӿ�ִ��sql���
*      ����������ظ�interpreter
*
*/


class Interpreter;

// an interface 
// ����sql����Ӧ��Ļ���
class StatementBlock{
public:
	virtual QueryResult execute() = 0;
	virtual void print() = 0;
	virtual void check() = 0;
	virtual ~StatementBlock() {};
};

// �½���check����Ƿ��Ѵ��ڱ�execute����api���½�����
class CreateTableBlock :public StatementBlock {
public:
	CreateTableBlock(Table table) :table(table) {};
	CreateTableBlock(Table table, std::string primaryKeyName) 
		:table(table),primaryKeyName(primaryKeyName) {};
	virtual void check();
	virtual QueryResult execute();
	virtual void print();

	virtual ~CreateTableBlock() {};
private:
	Table table;
	std::string primaryKeyName;
};

// �½�������check����Ƿ��Ѵ������������ڱ��������ԣ�execute����api���½���������
class CreateIndexBlock :public StatementBlock {
public:
	CreateIndexBlock(std::string indexName,std::string tableName, std::string attributeName) 
		:indexName(indexName),tableName(tableName),attributeName(attributeName) {};
	virtual QueryResult execute();
	virtual void check();
	virtual void print();
	virtual ~CreateIndexBlock() {};
private:
	std::string indexName;
	std::string tableName;
	std::string attributeName;
};

//	�����¼�� check����Ƿ���ڱ����������Ƿ��Ӧ�����������Ƿ��Ӧ��execute����api�в����¼����
class InsertTableBlock :public StatementBlock {
public:
	InsertTableBlock(std::string tableName, std::vector<std::string> values) 
	:tableName(tableName),values(values) {};
	virtual QueryResult execute();
	virtual void check();
	virtual void print();
	virtual ~InsertTableBlock() {};
private:
	std::string tableName;
	std::vector<std::string> values;
};

// ��ѯ��¼��check����Ƿ���ڱ����ԡ�����������������ֵ�Ƿ��Ӧ��execute����api�еĲ�ѯ����
class SelectBlock:public StatementBlock {
public:
	//SelectBlock(Interpreter* const ip):star(false),doNothingFlag(false),ip(ip){}
	SelectBlock(Interpreter* const ip):star(false),doNothingFlag(false){}
	void setStar(bool star) { this->star = star; }
	void setTableName(std::string& tableName) { this->tableName = tableName; }
	void setAttributes(std::list<std::string>& attributes) { this-> attributes = attributes; }
	void setExpressions(std::list<Expression>& exps) { this->exps = exps; }
	virtual QueryResult execute();
	virtual void check();
	virtual void print();
	~SelectBlock(){}
private:
	bool star;
	bool doNothingFlag;
	std::list<std::string> attributes;
	std::string tableName;
	std::list<Expression> exps;
	//Interpreter* const ip;
};

// �˳����ݿ⣺ ֱ���׳��˳��쳣�����˳�
class QuitBlock :public StatementBlock {
public:
	QuitBlock(){}
	virtual QueryResult execute();
	virtual void check();
	virtual void print();

	virtual ~QuitBlock() {};
private:
};

// ɾ����check����Ƿ���ڱ�execute����api�е�ɾ������
class DropTableBlock :public StatementBlock {
public:
	DropTableBlock(std::string s) :tableName(s) {}
	virtual QueryResult execute();
	virtual void check();
	virtual void print();
	virtual ~DropTableBlock(){}
private:
	std::string tableName;
};

// ɾ��������check����Ƿ���������� execute����api�е�ɾ����������
class DropIndexBlock :public StatementBlock {
public:
	DropIndexBlock(std::string s) :indexName(s) {}
	virtual QueryResult execute();
	virtual void check();
	virtual void print();

	virtual ~DropIndexBlock() {}
private:
	std::string indexName;
};

// ɾ����¼��check����Ƿ���ڱ������е����Լ������Ƿ���ֵ��Ӧ��execute����api�е�ɾ����¼����
class DeleteBlock : public StatementBlock{
public:
	DeleteBlock(std::string tableName) :tableName(tableName), flag(false), doNothingFlag(false) {}
	DeleteBlock(std::string tableName, std::list<Expression> exps)
	 :tableName(tableName),exps(exps),flag(true),doNothingFlag(false){}
	 
	virtual QueryResult execute();
	virtual void check();
	virtual void print();
	virtual ~DeleteBlock(){}
private:
	std::string tableName;
	std::list<Expression> exps;
	bool flag;
	bool doNothingFlag;
};

// ִ��һ��sql�ļ���executeֱ�Ӵ��ļ�����sql��䣬��������interpreter�����ļ��е�����sql�����������꣬�ټ�顢ִ�У��������ֻص����������sql����Ӧ����
class execBlock : public StatementBlock {
public:
	execBlock(std::string s,Interpreter* ip) :fileName(s),ip(ip){};
	virtual QueryResult execute();;
	virtual void print() {};
	virtual void check() {};
	virtual ~execBlock() {};
private:
	std::string fileName;
	Interpreter* const ip; // ���ڷ�����interpreter
};


// ��������Ƿ�ƥ��
class CheckType {
public:
	CheckType(){}
	CheckType(const Table& table) :table(table) {};
	bool isType(const std::string& s, Type type);
	bool isString(const std::string& s);
	bool isFloat(const std::string& s);
	bool isInt(const std::string& s);
	bool isAttribute(const std::string& s);
	Type isWhatType(const std::string& s);
private:
	Table table;
};

// ���ڱȽϵ� ģ�庯��
template<typename T>
auto compareFunc(OPERATOR oper)->bool(*)(T, T) {
	switch (oper) {
	case GREATER:
		return [](T t1, T t2)->bool {return t1 > t2; };
	case LESS:
		return [](T t1, T t2)->bool {return t1 < t2; };
	case EQUAL:
		return [](T t1, T t2)->bool {return t1 == t2; };
	case NOTEQUAL:
		return [](T t1, T t2)->bool {return t1 != t2; };
	case GREATER_AND_EQUAL:
		return [](T t1, T t2)->bool {return t1 >= t2; };
	case LESS_AND_EQUAL:
		return [](T t1, T t2)->bool {return t1 <= t2; };
	}
	return nullptr;
}

// ������ʽ
bool compareExp(const std::string& left, const std::string& right, Type type, OPERATOR op);

#endif