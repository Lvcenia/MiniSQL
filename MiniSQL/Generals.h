#pragma once
/*����һЩȫ�ַ�Χ�ڵ������࣬�����ѯ���*/
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
	//���������
	string content;
	//Ӱ�������
	int affectedRows;
	//query��ִ��ʱ��
	double execTime;

};

