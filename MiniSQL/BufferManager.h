#ifndef _BUFFERMANAGER_H_
#define _BUFFERMANAGER_H_

#include <string>
#include <cstdio>
#include <exception>

using std::string;
using std::exception;

//文件操作异常
class FileOperateException :public exception
{
private:
	string errorLog;
public:
	FileOperateException(string description) :errorLog(description) {}
	FileOperateException(string description,string filename) :errorLog(description + filename) {}
	virtual const char* what()const { return errorLog.c_str(); }
};


class BufferManager
{
};

#endif