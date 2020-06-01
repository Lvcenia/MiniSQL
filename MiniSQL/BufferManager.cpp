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
	//���������е�����ȫ��д�����
	for (int i = 0; i < BLOCKNUM; i++)
	{
		if (blocks[i].isDirtyBit()) {
			writeABlock(i);
		}
	}
	//�رյ�ǰ�ļ�
	if (cur_file != NULL) {
		fclose(cur_file);
	}
	//�����ڴ�
	delete[] blocks;
}

//����һ����̬BufferManager�����ָ��
BufferManager* BufferManager::getBufferManager()
{
	static BufferManager bm;
	return &bm;
}

/*
	����:����һ����Ϊ<filename>.data���ļ�
	ע:����ʧ�ܷ���false,�ɹ�����true
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
	����:ɾ��<filename>.data�ļ��ڴ��̺ͻ������е���������
	ע:�ú�����ɾ��ʧ��ʱ����FileOperateException�쳣
*/
void BufferManager::delFile(const string&filename)
{
	string fn = filename + ".data";
	//Ҫɾ�����ļ����ڴ�,���ȹرո��ļ�
	if (cur_filename == fn) {
		fclose(cur_file);
		cur_filename = "";
		cur_file = NULL;
	}
	//ɾ�������������ڸ��ļ�������
	for (int i = 0; i < BLOCKNUM; i++) {
		if (blocks[i].getFileName() == fn) {
			blocks[i].clear();
#ifdef _BufferManager_DEBUG_
			std::cout << "clear block:" << i << std::endl;
#endif
		}
	}
	//ɾ�������ϵĸ��ļ�
	if ((remove(fn.c_str())) == -1)
		throw FileOperateException("Fail to remove ",fn);
}

/*
	����:���±�Ϊindex�����ݿ�д�����
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
	����:��filename�ļ��ж�ȡtag�����ݿ�
	����ֵ:ȡ�������ڻ������еĿ���
*/
int BufferManager::fetchABlock(const string&filename, const ADDRESS& tag)
{
	//���Ҫ��ȡ���ļ������Ѵ򿪵��ļ�, ��ر�֮ǰ���ļ�, ��filename�ļ���Ϊ��ǰ�ļ�
	if (cur_filename != filename || cur_file == NULL)
	{
		if (cur_file != NULL) fclose(cur_file);
		if ((cur_file = fopen(filename.c_str(), "rb+")) == NULL) {
			throw FileOperateException("Fail to open ", filename);
		}
		cur_filename = filename;
	}

	//�����ݴ��ļ���ȥ������
	BYTE buffer[BLOCKSIZE];
	fseek(cur_file, BLOCKSIZE*tag, SEEK_SET);
	fread(buffer, BLOCKSIZE, 1, cur_file);

	//������д�뻺����, indexΪд�����ݿ�ı��
	int index = substitute(filename, tag, buffer);
	return index;
}

/*
	����:�ҳ����ʵ����ݿ�, ��filename,tag,buffer����Ϣ��������ݿ�
	����ֵ:��������ݿ�ı��
	ע:���������ݿⶼpinned��ʱ��,���׳�BlockAllPinnedException�쳣
*/
int BufferManager::substitute(const string& filename, const ADDRESS& tag, BYTE* buffer)
{
	int iterator = subQueque.getHead();
	while (iterator != -1 && blocks[iterator].isPinnnedBit())
	{
		//�����������ݿ�ŵ�����β��
		subQueque.moveToTail(iterator);
		iterator = subQueque.getHead();
	}

	//���е����ݿ鶼pinnned��
	if (iterator == -1) {
		throw BlockAllPinnedException();
	}

	//���Ҫʹ�õ����ݿ���������, ��ԭ��������д�����
	if (blocks[iterator].isDirtyBit()) writeABlock(iterator);

	//������ݿ�
	subQueque.moveToTail(iterator);
	blocks[iterator].setBlockData(buffer);
	blocks[iterator].setDirtyBit(false);
	blocks[iterator].setFileName(filename);
	blocks[iterator].setTag(tag);

	return iterator;
}

/*
	����:����ڻ��������Ƿ��ж�Ӧfilename,ƫ����Ϊtag�����ݿ�
	����ֵ: �����ڶ�Ӧ��, �򷵻����ݿ�ı��,�������򷵻�-1
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
	����:��ѯname�ļ���ƫ�����ڻ������еĵ�ַ
	����ֵ:��������ַ
*/
BYTE* BufferManager::fetchARecord(const string&name, const ADDRESS& address)
{
	int blockIndex;
	int tag = address / BLOCKSIZE;
	string fn = name + ".data";
	if ((blockIndex = hit(fn, tag)) == -1) {
		//�������в�����Ҫ��ļ�¼,��Ӵ����м���
		blockIndex = fetchABlock(fn, tag);
	}
	else {
		//����Ծ�����ݿ�ŵ�β��
		subQueque.moveToTail(blockIndex);
	}

	//��¼��ַ�����ݿ��ַ�����ϵ�ƫ����
	int offset = address - tag * BLOCKSIZE;
	return blocks[blockIndex].getBlockData() + offset;
}

/*
	����:��record��¼д���ļ���Ӧλ�õĻ�������
*/
void BufferManager::writeARecord(BYTE* record, int len, const string& name, const ADDRESS& address)
{
	int blockIndex;
	int tag = address / BLOCKSIZE;
	string fn = name + ".data";
	if ((blockIndex = hit(fn, tag)) == -1) {
		//�������в�����Ҫд�ļ�¼,��Ӵ����м���
		blockIndex = fetchABlock(fn, tag);
	}
	else {
		//����Ծ�����ݿ�ŵ�β��
		subQueque.moveToTail(blockIndex);
	}

	//��¼��ַ�����ݿ��ַ�����ϵ�ƫ����
	int offset = address - tag * BLOCKSIZE;
	memcpy(blocks[blockIndex].getBlockData() + offset, record, len);

	blocks[blockIndex].setDirtyBit(true);
}

//��ָ�����ݿ���Ϊpinned
void BufferManager::setBlockPinned(int index)
{
	blocks[index].setPinnedBit(true);
}

//��ָ�����ݿ���Ϊnot pinned
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
	char buffer[] = "����һ����������";
	try {
		bm.writeARecord((BYTE *)buffer, strlen(buffer), "test", 20);
		bm.setBlockPinned(0);
		bm.writeARecord((BYTE *)"�⻹��һ����������", strlen("�⻹��һ����������"), "test", 100);
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
