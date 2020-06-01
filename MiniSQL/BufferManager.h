#ifndef _BUFFERMANAGER_H_
#define _BUFFERMANAGER_H_

#include <string>
#include <cstdio>
#include <exception>

using std::string;
using std::exception;

#define BLOCKSIZE 4096
#define BLOCKNUM 10
typedef unsigned char BYTE;
typedef int ADDRESS;
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

//数据块全部被占用时异常
class BlockAllPinnedException :public exception
{
private:
	string errorLog;
public:
	BlockAllPinnedException() { errorLog = "All block has been pinned!"; }
	virtual const char* what()const { return errorLog.c_str(); }
};

//数据块类,用于管理一个磁盘分区
class Block
{
private:
	string filename;
	bool dirtyBitFlag;
	bool pinnedFlag;
	ADDRESS tag;
	BYTE data[BLOCKSIZE];
public:
	Block():dirtyBitFlag(false),pinnedFlag(false),tag(0){}
	void setData(BYTE* data) { memcpy(this->data, data, BLOCKSIZE); }
	BYTE* getData(){ return data; }
	void setDirtyBit(bool isDirty) { this->dirtyBitFlag = isDirty; }
	bool isDirtyBit() { return this->dirtyBitFlag; }
	void setPinnedBit(bool isPinned) { this->pinnedFlag = isPinned; }
	bool isPinnedBit() { return this->pinnedFlag; }
	void setTag(ADDRESS tag) { this->tag = tag; }
	ADDRESS getTag() { return tag; }
	void setFileName(const string& name) { filename = name; }
	string getFileName() { return filename; }
	void clear() { dirtyBitFlag = false; filename = ""; }
};
class BufferManager
{
private:
	FILE* file;
	string filename;
	Block* blocks;
public:
	BufferManager();
	~BufferManager();
	bool newFile(const string&filename);
	void delFile(const string&filename);
	void setBlockPinned(int index);
	void setBlockNotPinned(int index);
	void writeARecord(BYTE* record, int len, const string&name, const ADDRESS& address);
	BYTE* readARecord(const string&name, const ADDRESS& address);
	static BufferManager* getBufferManager();
};

#endif