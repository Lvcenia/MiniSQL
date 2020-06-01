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

class Attribute
{
private:
	string name;
	TYPE type;
	int length;
	int offset;
	bool isUnique;
	bool isPrimaryKey;
public:
	Attribute() :isUnique(false), isPrimaryKey(false) {}
	Attribute(const std::string& attribute, TYPE type, int length, bool isUnique, bool isPrimary)
		:name(attribute), type(type), length(length), isUnique(isUnique), isPrimaryKey(isPrimary) {};
	const std::string& getAttribute()const { return name; }
	void setAttribute(const std::string& attr) { name = attr; }
	TYPE getType()const { return type; }
	void setType(TYPE type);
	int getLength()const { return length; }
	void setLength(int len) { length = len; }
	bool isUnique()const { return isUnique; }
	void setUnique(bool flag) { isUnique = flag; }
	bool isPrimary()const { return isPrimaryKey; }
	void setPrimary(bool flag) { isPrimaryKey = flag; }
	void setOffset(const int off) { offset = off; }
	int getOffset() { return offset; }
	~Attribute() {}
};

