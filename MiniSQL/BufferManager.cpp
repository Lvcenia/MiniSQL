#include "BufferManager.h"

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
		cur_file = fopen(filename.c_str(), "rb+");
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

int BufferManager::hit(const string& fileName, const ADDRESS& tag)
{

}