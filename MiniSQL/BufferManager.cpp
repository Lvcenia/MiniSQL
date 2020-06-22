#include "BufferManager.h"

//#define _BufferManager_DEBUG_
#ifdef _BufferManager_DEBUG_
#include<iostream>
#endif

BufferManager::BufferManager() :
	cur_file(NULL), cur_filename("")
{
	blocks = new Block[BLOCKNUM];
}

BufferManager::~BufferManager()
{
	//将缓冲区中的数据全部写入磁盘
	for (int i = 0; i < BLOCKNUM; i++)
	{
		if (blocks[i].isDirtyBit()) {
			writeABlock(i);
		}
	}
	//关闭当前文件
	if (cur_file != NULL) {
		fclose(cur_file);
	}
	//回收内存
	delete[] blocks;
}

void BufferManager::saveDataForcedly()
{
	//将缓冲区中的数据全部写入磁盘
	for (int i = 0; i < BLOCKNUM; i++)
	{
		if (blocks[i].isDirtyBit()) {
			writeABlock(i);
		}
	}
	//关闭当前文件
	if (cur_file != NULL) {
		fclose(cur_file);
	}
}

//返回一个静态BufferManager对象的指针
BufferManager* BufferManager::getBufferManager()
{
	static BufferManager bm;
	return &bm;
}

/*
	作用:创建一个名为<filename>.data的文件
	注:创建失败返回false,成功返回true
*/
bool BufferManager::newFile(const string& filename)
{
	string fn = filename + ".data";
	FILE* fp = fopen(fn.c_str(), "wb+");
	if (fp == NULL) {
		return false;
	}
	fclose(fp);
	return true;
}

/*
	作用:删除<filename>.data文件在磁盘和缓冲区中的所有数据
	注:该函数在删除失败时会抛FileOperateException异常
*/
void BufferManager::delFile(const string&filename)
{
	string fn = filename + ".data";
	//要删除的文件正在打开,则先关闭该文件
	if (cur_filename == fn) {
		fclose(cur_file);
		cur_filename = "";
		cur_file = NULL;
	}
	//删除缓冲区中属于该文件的数据
	for (int i = 0; i < BLOCKNUM; i++) {
		if (blocks[i].getFileName() == fn) {
			blocks[i].clear();
#ifdef _BufferManager_DEBUG_
			std::cout << "clear block:" << i << std::endl;
#endif
		}
	}
	//删除磁盘上的该文件
	if ((remove(fn.c_str())) == -1)
		throw FileOperateException("Fail to remove ",fn);
}

/*
	作用:将下标为index的数据块写入磁盘
*/
void BufferManager::writeABlock(const int& index) 
{
	string fn = blocks[index].getFileName();
	FILE* fp = fopen(fn.c_str(), "rb+");
	if (fp == NULL) throw FileOperateException("Fail to open ", fn);
	fseek(fp, blocks[index].getTag()*BLOCKSIZE, SEEK_SET);
	fwrite(blocks[index].getBlockData(), BLOCKSIZE, 1, fp);
	fclose(fp);
}

/*
	作用:从filename文件中读取tag个数据块
	返回值:取出数据在缓冲区中的块编号
*/
int BufferManager::fetchABlock(const string&filename, const ADDRESS& tag)
{
	//如果要读取的文件不是已打开的文件, 则关闭之前的文件, 将filename文件设为当前文件
	if (cur_filename != filename || cur_file == NULL)
	{
		if (cur_file != NULL) fclose(cur_file);
		if ((cur_file = fopen(filename.c_str(), "rb+")) == NULL) {
			throw FileOperateException("Fail to open ", filename);
		}
		cur_filename = filename;
	}

	//将数据从文件中去读出来
	BYTE buffer[BLOCKSIZE];
	fseek(cur_file, BLOCKSIZE*tag, SEEK_SET);
	fread(buffer, BLOCKSIZE, 1, cur_file);

	//将数据写入缓冲区, index为写入数据块的编号
	int index = substitute(filename, tag, buffer);
	return index;
}

/*
	作用:找出合适的数据块, 将filename,tag,buffer等信息填入该数据块
	返回值:被填充数据块的编号
	注:在所有数据库都pinned的时候,会抛出BlockAllPinnedException异常
*/
int BufferManager::substitute(const string& filename, const ADDRESS& tag, BYTE* buffer)
{
	int iterator = subQueque.getHead();
	while (iterator != -1 && blocks[iterator].isPinnnedBit())
	{
		//将已满的数据块放到队列尾部
		subQueque.moveToTail(iterator);
		iterator = subQueque.getHead();
	}

	//所有的数据块都pinnned了
	if (iterator == -1) {
		throw BlockAllPinnedException();
	}

	//如果要使用的数据块已有数据, 将原来的数据写入磁盘
	if (blocks[iterator].isDirtyBit()) writeABlock(iterator);

	//填充数据块
	subQueque.moveToTail(iterator);
	blocks[iterator].setBlockData(buffer);
	blocks[iterator].setDirtyBit(false);
	blocks[iterator].setFileName(filename);
	blocks[iterator].setTag(tag);

	return iterator;
}

/*
	作用:检查在缓冲区中是否有对应filename,偏移量为tag的数据块
	返回值: 若存在对应的, 则返回数据块的编号,不存在则返回-1
*/
int BufferManager::hit(const string& fileName, const ADDRESS& tag)
{
	for (int i = 0; i < BLOCKNUM; i++)
	{
		if (blocks[i].getFileName() == fileName && blocks[i].getTag() == tag)
		{
			return i;
		}
	}
	return -1;
}

/*
	作用:查询name文件中偏移量在缓冲区中的地址
	返回值:缓冲区地址
*/
BYTE* BufferManager::fetchARecord(const string&name, const ADDRESS& address)
{
	int blockIndex;
	int tag = address / BLOCKSIZE;
	string fn = name + ".data";
	if ((blockIndex = hit(fn, tag)) == -1) {
		//缓冲区中不存在要查的记录,则从磁盘中加载
		blockIndex = fetchABlock(fn, tag);
	}
	else {
		//将活跃的数据块放到尾部
		subQueque.moveToTail(blockIndex);
	}

	//记录地址在数据块地址基础上的偏移量
	int offset = address - tag * BLOCKSIZE;
	return blocks[blockIndex].getBlockData() + offset;
}

/*
	作用:将record记录写在文件对应位置的缓冲区上
*/
void BufferManager::writeARecord(BYTE* record, int len, const string& name, const ADDRESS& address)
{
	int blockIndex;
	int tag = address / BLOCKSIZE;
	string fn = name + ".data";
	if ((blockIndex = hit(fn, tag)) == -1) {
		//缓冲区中不存在要写的记录,则从磁盘中加载
		blockIndex = fetchABlock(fn, tag);
	}
	else {
		//将活跃的数据块放到尾部
		subQueque.moveToTail(blockIndex);
	}

	//记录地址在数据块地址基础上的偏移量
	int offset = address - tag * BLOCKSIZE;
	memcpy(blocks[blockIndex].getBlockData() + offset, record, len);

	blocks[blockIndex].setDirtyBit(true);
}

//将指定数据块设为pinned
void BufferManager::setBlockPinned(int index)
{
	blocks[index].setPinnedBit(true);
}

//将指定数据块设为not pinned
void BufferManager::setBlockNotPinned(int index)
{
	blocks[index].setPinnedBit(false);
}


#ifdef _BufferManager_DEBUG_
#include<Windows.h>
int main()
{
	BufferManager bm;
	//bm.newFile("test");
	char buffer[] = "这是一条测试数据";
	try {
		bm.writeARecord((BYTE *)buffer, strlen(buffer), "test", 20);
		bm.setBlockPinned(0);
		bm.writeARecord((BYTE *)"这还是一条测试数据", strlen("这还是一条测试数据"), "test", 100);
		std::cout << bm.fetchARecord("test", 20) << std::endl;
		bm.delFile("test");
	}
	catch (FileOperateException &ex) {
		std::cout << ex.what() << std::endl;
	}
	system("pause");
	return 0;
}
#endif // _BufferManager_DEBUG_
