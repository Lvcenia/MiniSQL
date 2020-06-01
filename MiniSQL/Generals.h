#pragma once
/*����һЩȫ�ַ�Χ�ڵ������࣬�����ѯ���*/
#include <string>
#include<cstring>
#include<vector>
#include<cstdio>
#include <sstream>


using namespace std;

enum QueryState
{
	Success,
	Fail
};

//��¼һ���û������Ƿ�ɹ��������࣬��API��interpreter���ɲ����ݵ�����㣬����ѭ������ʾ
//��������ɹ�����������ĸ�����
class RecordBuffer;
class QueryResult
{
public:
	QueryResult() {};
	//�ɹ�ʱ�����
	QueryResult(QueryState state, int affcRows, double exectime,RecordBuffer& rb)
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
	//����ʱ�����
	QueryResult(QueryState state, exception e = exception(""))
	{
		stringstream fmt;
		switch (this->state)
		{
		case Fail:fmt << "Query Failed " << e.what();
			this->content = fmt.str();
		default:
			break;
		}
	}


	~QueryResult() {};
	QueryState state;
	//���������
	string content;
	//Ӱ�������
	int affectedRows;
	//query��ִ��ʱ��
	double execTime;
	
};


class Table
{
public:
	Table();
	~Table();
	string tableName;


private:

};





//��¼��ѯ����Ļ����࣬ȫ��Ψһ
class RecordBuffer
{
public:
	RecordBuffer();
	~RecordBuffer();

private:

};



