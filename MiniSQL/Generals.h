#pragma once
/*包含一些全局范围内的数据类，比如查询结果*/
#include <iostream>
#include <string>
#include<cstring>
#include<vector>
#include<cstdio>
#include <sstream>


using namespace std;

enum Type
{
	Int,
	Char,
	Float,
	UNDEFINEDTYPE
};

enum OP {
	Equal,
	NotEqual,
	GreaterThan,
	LessThan,
	GreaterEqual,
	LessEqual
};

//筛选条件请用Cnndition
//目前只考虑简单的 （属性 op 值），有时间再加
class Condition {
public:
	Condition(string attributeName, OP op, string value);
	virtual ~Condition();
	string getAttributeName();
	OP getOP();
	string getValue();
	bool FitCondition(string attributeVal, Type type);
	static bool GetMinimalConditoins(vector<Condition>& conditions, Type type);
	static bool FitAllConditions(const vector<Condition>& conditions, Type type, string value);
private:
	string attributeName;
	OP op;
	string value;
};


struct Expression;
struct Operand;

enum OPERATOR
{
	GREATER,
	LESS,
	EQUAL,
	NOTEQUAL,
	GREATER_AND_EQUAL,
	LESS_AND_EQUAL,
	UNDEFINED
};

OPERATOR reverseOperator(OPERATOR op);
OPERATOR stringToOperator(std::string s);

struct Operand
{
	std::string operandName;
	bool isAttribute;
};

struct Expression
{
	Operand leftOperand;
	Operand rightOperand;
	OPERATOR op;
	void swap();
};



enum QueryState
{
	Success,
	Fail
};

//记录一次用户操作是否成功的数据类，由API或interpreter生成并传递到最外层，在主循环中显示
//记录查询结果的缓存类，全局唯一（个人觉得不唯一，特殊情况：执行一个文件时，建议：每个结果queryresult存一个 recordbuffer，interpreter里存vector<queryresult>）
class RecordBuffer
{
public:
	RecordBuffer();
	RecordBuffer(const vector<string>& content);
	// 拷贝构造, 似乎是这样写拷贝构造的吧
	RecordBuffer(const RecordBuffer& that) { 
		for (auto iter : that.content)
		{
			content.push_back(iter);
		}
		
	}
	~RecordBuffer();
	void output();//简易输出
	void addContent(string info);//追加内容
	void clearContent();//清空
	const vector<string>& getContent();//获取内容
	int getLineCount();//获取行数

private:
	vector<string> content;
};

class QueryResult
{
public:
	QueryResult() {};
	//成功时用这个
	QueryResult(QueryState state, int affcRows, double exectime, RecordBuffer& rb):records(rb)
	{
		this->state = state;
		this->affectedRows = affcRows;
		this->execTime = exectime;
		stringstream fmt;
		switch (this->state)
		{
		case Success:fmt << "Query Successfully ended in "<< this->execTime <<  "s, " <<this->affectedRows <<" rows Affected";
			this->content = fmt.str();
		default:
			break;
		}
	}
	// 拷贝构造，方便interpreter 操作 （哈哈）add by dgz
	QueryResult(const QueryResult& that) : state(that.state),content(that.content),
	                        affectedRows(that.affectedRows),execTime(that.execTime),
		                    records(that.records),showRocords(that.showRocords) {}
	//出错时用这个
	QueryResult(QueryState state, std::exception e)
	{
		this->records = RecordBuffer();
		stringstream fmt;
		this->state = state;
		switch (this->state)
		{
		case Fail:fmt << "Query Failed " << (&e)->what();
			this->content = fmt.str();
		default:
			break;
		}
	}

	QueryResult(QueryState state, std::exception* e)
	{
		this->records = RecordBuffer();
		stringstream fmt;
		this->state = state;
		switch (this->state)
		{
		case Fail:fmt << "Query Failed " << e->what();
			this->content = fmt.str();
		default:
			break;
		}
	}


	~QueryResult() {};

	// 丁贵州添加：（个人想法）
	// 打印执行结果
	void PrintResult() {
		std::cout << content << endl;
	}
	// 返回执行状态
	bool GetQueryState() {
		return (state == Success) ? true : false;
	}
	// 打印查询的结果记录
	void PrintRecords() {
		if (showRocords)
		{
			if (this->records.getContent().size() <= 1)
			{
				cout << "No Records";
				return;
			}
				
			records.output();
		}

	}
	
public:
	QueryState state = Success;
	//结果的内容
	string content = "";
	//影响的行数
	int affectedRows = 0;
	//query的执行时间
	double execTime = 0;
	// 查询的记录
	RecordBuffer records;
	bool showRocords = false;
};



Type stringToType(std::string s);

string typeToString(Type t);


struct AttributeInfo {
	char name[32];
	Type type;
	int length;
	int offset;
	int isUnique;
	int isPrimaryKey;
};

class Attribute
{
private:
	string name;
	Type type;
	int length;
	int offset = 0;
	bool isUnique = false;
	bool isPrimaryKey = false;
public:
	Attribute();
	Attribute(const std::string& attributeName, Type type, int length, bool isUnique, bool isPrimary, int offset);
	Attribute(const AttributeInfo& info);
	virtual ~Attribute();
	string getAttributeName() const;
	void setAttributeName(string name);
	Type getType() const;
	void setType(Type type);
	int getLength() const;
	void setLength(int length);
	bool isUniqueKey() const;
	void setUnique(bool value);
	bool isPrimary() const;
	void setPrimary(bool value);
	void setOffset(int offset);
	int getOffset() const;
	AttributeInfo GetInfo();
};

#define MAXAttributeCount 32
struct TableHeader {
	char tableName[32];
	int attrCount;
	AttributeInfo attributes[MAXAttributeCount];
	int primaryKeyIndex;
	int recordLength;
	int recordCount;
};

class Table
{
public:
	Table();//Do not use
	Table(string tableName, const vector<Attribute>& attributes);//create a new table
	Table(const TableHeader& header);//create from file
	~Table();
	string getTableName() const;
	void setTableName(string tableName);
	const vector<Attribute>& getAttributes() const;
	void setAttributes(const vector<Attribute>& attributes);
	int getPrimaryKeyIndex() const;
	void setPrimaryKeyIndex(int primaryKeyIndex);
	int getAttributeCount() const;
	void addAttribute(Attribute& attribute);
	bool hasAttribute(string attributeName);
	const Attribute& getAttribute(string attributeName) const;//若未找到，则抛出异常
	TableHeader GetTableHeader();
	int getRecordLength() const;//获取记录长度
	int getRecordCount() const;//获取记录数
	
private:
	string name;
	vector<Attribute> attributes;
	int primaryKeyIndex;//-1 if no primary key
	int attrCount;
	int recordLength;
	int recordCount;
};








