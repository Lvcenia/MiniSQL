#pragma once
/*包含一些全局范围内的数据类，比如查询结果*/
#include <string>
#include<cstring>
#include<vector>
#include<cstdio>

using namespace std;

enum QueryState
{
	Success,
	Fail
};

class QueryResult
{
public:
	QueryResult(const string& cont);
	~QueryResult();
	QueryState state;
	//结果的内容
	string content;
	//影响的行数
	int affectedRows;
	//query的执行时间
	double execTime;

};

enum Type
{
	INT,
	CHAR,
	FLOAT,
	UNDEFINEDTYPE
};

class Attribute
{
private:
	string name;
	Type type;
	int length;
	int offset;
	bool isUnique;
	bool isPrimaryKey;
public:
	Attribute();
	Attribute(const std::string& attributeName, Type type, int length, bool isUnique, bool isPrimary);
	virtual ~Attribute();
	string getAttributeName();
	void setAttributeName(string name);
	Type getType();
	void setType(Type type);
	int getLength();
	void setLength(int length);
	bool isUnique();
	void setUnique(bool value);
	bool isPrimary();
	void setPrimary(bool value);
	void setOffset(int offset);
	int getOffset();
};

class Table
{
public:
	Table();
	Table(string tableName, const vector<Attribute>& attributes);
	~Table();
	string getTableName();
	void setTableName(string tableName);
	const vector<Attribute>& getAttributes();
	void setAttributes(const vector<Attribute>& attributes);
	int getPrimaryKeyIndex();
	void setPrimaryKeyIndex(int primaryKeyIndex);
	int getRowLength();
	void addAttribute(Attribute& attribute);
	bool hasAttribute(string attributeName);
	const Attribute& getAttribute(string attributeName);//若未找到，则抛出异常
	
private:
	string name;
	vector<Attribute> attributes;
	int primaryKeyIndex;//-1 if no primary key
	int rowLength;
};
