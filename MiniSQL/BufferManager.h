#ifndef _BUFFERMANAGER_H_
#define _BUFFERMANAGER_H_

#include <string>
#include <cstdio>
#include <exception>

using std::string;
using std::exception;

//�ļ���ʱ�쳣
class FileOpenException :public exception
{
private:
	string errorLog;
public:
	FileOpenException() :errorLog("Fail to open file") {}
	FileOpenException(string filename) :errorLog("Fail to open " + filename) {}
	virtual const char* what()const { return errorLog.c_str(); }
};

//�ļ��Ƴ�ʱ�쳣
class FileRemoveException :public exception
{
private:
	string errorLog;
public:
	FileRemoveException() :errorLog("Fail to remove file") {}
	FileRemoveException(string filename) :errorLog("Fail to remove " + filename) {}
	virtual const char* what()const { return errorLog.c_str(); }
};

class BufferManager
{
};

#endif