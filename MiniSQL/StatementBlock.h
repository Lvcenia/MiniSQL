


#ifndef _STATEMENTBLOCK_H_
#define _STATEMENTBLOCK_H_

//#include "Definition.h"
#include <memory>
#include <string>
#include <list>
#include <vector>
#include "Generals.h"



/*各种sql语句对应的类
*
*作用：接收interpreter传来的被初步语法分析的sql语句处理后的结果，
*      接着进行检查，检查数据库中是否已存在、是否不存在...等问题
*      没有问题就调用api中的各种接口执行sql语句
*      并将结果返回给interpreter
*
*/


class Interpreter;

// an interface 
// 所有sql语句对应类的基类
class StatementBlock{
public:
	virtual QueryResult execute() = 0;
	virtual void print() = 0;
	virtual void check() = 0;
	virtual ~StatementBlock() {};
};

// 新建表：check检查是否已存在表，execute调用api中新建表函数
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

// 新建索引：check检查是否已存在索引、存在表、存在属性，execute调用api中新建索引函数
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

//	插入记录： check检查是否存在表、属性数量是否对应、属性类型是否对应，execute调用api中插入记录函数
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

// 查询记录：check检查是否存在表、属性、条件中属性类型与值是否对应，execute调用api中的查询函数
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

// 退出数据库： 直接抛出退出异常进行退出
class QuitBlock :public StatementBlock {
public:
	QuitBlock(){}
	virtual QueryResult execute();
	virtual void check();
	virtual void print();

	virtual ~QuitBlock() {};
private:
};

// 删除表：check检查是否存在表，execute调用api中的删除表函数
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

// 删除索引：check检查是否存在索引， execute调用api中的删除索引函数
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

// 删除记录：check检查是否存在表、条件中的属性及类型是否与值对应，execute调用api中的删除记录函数
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

// 执行一个sql文件：execute直接打卡文件读入sql语句，并反调用interpreter，把文件中的所有sql语句分析处理完，再检查、执行，这样，又回到了上面各种sql语句对应的类
class execBlock : public StatementBlock {
public:
	execBlock(std::string s,Interpreter* ip) :fileName(s),ip(ip){};
	virtual QueryResult execute();;
	virtual void print() {};
	virtual void check() {};
	virtual ~execBlock() {};
private:
	std::string fileName;
	Interpreter* const ip; // 用于反调用interpreter
};


// 检查类型是否匹配
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

// 用于比较的 模板函数
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

// 处理表达式
bool compareExp(const std::string& left, const std::string& right, Type type, OPERATOR op);

#endif