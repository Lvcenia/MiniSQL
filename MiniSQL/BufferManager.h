#ifndef _BUFFERMANAGER_H_
#define _BUFFERMANAGER_H_

#include <string>
#include <cstdio>
#include <exception>
#include "ArrayList.h"

typedef unsigned char BYTE;
typedef int ADDRESS;
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

//数据块全部占用异常
class BlockAllPinnedException : public std::exception
{
private:
	std::string errLog;
public:
	BlockAllPinnedException() { errLog = "All block has been pinned!"; }
	virtual const char* what()const { return errLog.c_str(); }
};

//数据块类
class Block
{
private:
	BYTE data[BLOCKSIZE];	//缓冲区数据
	bool isDirty;			//记录缓冲区中数据是否和磁盘一样, 不一样则为true,否则为false
	bool isPinned;			//锁定标志
	ADDRESS tag;			//数据块对应文件中数据位置
	string filename;		//数据块所属文件名
public:
	Block():isDirty(false),isPinned(false) {}
	~Block() {}
	void setBlockData(BYTE* data) { memcpy(this->data, data, BLOCKSIZE); }
	BYTE* getBlockData() { return data; }
	void setDirtyBit(bool isDirty) { this->isDirty = isDirty; }
	bool isDirtyBit() { return isDirty; }
	void setPinnedBit(bool pinnnedBit) { this->isPinned = pinnnedBit; }
	bool isPinnnedBit() { return isPinned; }
	void setFileName(const std::string& fileName) { this->filename = fileName; }
	std::string getFileName() { return filename; }
	void setTag(ADDRESS tag) { this->tag = tag; }
	ADDRESS getTag() { return tag; }
	void clear() { isDirty = false; filename = ""; }
};

class BufferManager
{
private:
	FILE * cur_file;	//当前打开的文件指针
	string cur_filename;	//当前打开的文件名
	Block *blocks;			//数据块
	ArrayList subQueque;	//数据块顺序队列
	void writeABlock(const int& index);
	int fetchABlock(const std::string& filename, const ADDRESS& address);
	int substitute(const std::string& filename, const ADDRESS& tag, BYTE* buffer);
	int hit(const std::string& filename, const ADDRESS& tag);
public:
	BufferManager();
	~BufferManager();
	bool newFile(const std::string& filename);
	void delFile(const std::string& filename);
	BYTE* fetchARecord(const std::string& name, const ADDRESS& address);
	void writeARecord(BYTE* record, int len, const std::string& name, const ADDRESS& address);
	void setBlockPinned(int index);
	void setBlockNotPinned(int index);
	static BufferManager* getBufferManager();
};

#endif