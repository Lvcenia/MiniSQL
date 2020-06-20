#include "RecordManager.h"
#include <ctime>

RecordManager::RecordManager()
{
	bufferManager = BufferManager::getBufferManager();
}

RecordManager::~RecordManager()
{
}

const QueryResult& RecordManager::createTable(Table& tableInfo)
{
	auto start = clock();

	auto header = tableInfo.GetTableHeader();
	bufferManager->newFile(tableInfo.getTableName());
	int tail = 4096;
	bufferManager->writeARecord((unsigned char*)(&tail), sizeof(int), tableInfo.getTableName(), 0);
	bufferManager->writeARecord((unsigned char*)&header, sizeof(TableHeader), tableInfo.getTableName(), sizeof(int));

	auto end = clock();
	auto time = (double)(end - start) / CLOCKS_PER_SEC;
	RecordBuffer rb;
	return QueryResult(Success, 0, time, rb);
}

const QueryResult& RecordManager::dropTable(const Table& tableInfo)
{
	auto start = clock();
	int count = tableInfo.getRecordCount();
	bufferManager->delFile(tableInfo.getTableName());
	auto end = clock();
	auto time = (double)(end - start) / CLOCKS_PER_SEC;
	RecordBuffer rb;
	return QueryResult(Success, count, time, rb);
}

ADDRESS RecordManager::insertValues(const Table& tableInfo, const vector<string>& values)
{
	auto start = clock();
	ADDRESS tail = *((int*)(bufferManager->fetchARecord(tableInfo.getTableName(), 0)));
	int recordLength = tableInfo.getRecordLength();
	int blockIndex = tail / BLOCKSIZE;
	if ((blockIndex + 1) * BLOCKSIZE < tail + recordLength)
		tail = (blockIndex + 1) * BLOCKSIZE;
	BYTE* buffer = new BYTE[recordLength];	//a buffer to store the record
	ADDRESS offset = 0;
	int index = 0;
	for (auto attribute : tableInfo.getAttributes()) {
		memcpy(buffer + offset, values[index++].c_str(), attribute.getLength());
		offset += attribute.getLength();
	}
	bufferManager->writeARecord(buffer, recordLength, tableInfo.getTableName(), tail);
	int returnvalue = tail;
	tail += recordLength;
	bufferManager->writeARecord((BYTE*)(&tail), 4, tableInfo.getTableName(), 0);	//update the tail of the record
	delete []buffer;
	auto end = clock();
	auto time = (double)(end - start) / CLOCKS_PER_SEC;
	RecordBuffer rb;
	return tail;
}

const QueryResult& RecordManager::deleteValues(const Table& tableInfo, const vector<Condition>& conditions)
{
	auto start = clock();
	int count = 0;
	int recordLength = tableInfo.getRecordLength();
	ADDRESS tail = *((int*)(bufferManager->fetchARecord(tableInfo.getTableName(), 0)));
	RecordIterator it(recordLength, tail);
	while (it.hasNext()) {
		auto buffer = bufferManager->fetchARecord(tableInfo.getTableName(), it.value());
		char check[8];
		memcpy(check, buffer, 8);
		if (string(check) != "Deleted") {
			map<string, string> values;
			map<string, Type> types;
			int offset = 0;
			for (auto attribute : tableInfo.getAttributes()) {
				char* buff = new char[attribute.getLength() + 1];
				memcpy(buff, buffer + offset, attribute.getLength());
				buff[attribute.getLength()] = '\0';
				values.insert(pair<string, string>(attribute.getAttributeName(), string(buff)));
				delete []buff;
				types.insert(pair<string, Type>(attribute.getAttributeName(), attribute.getType()));
				offset += attribute.getLength();
			}
			bool flag = true;
			for (auto condition : conditions) {
				if (!condition.FitCondition(values[condition.getAttributeName()], types[condition.getAttributeName()])) {
					flag = false;
				}
			}
			if (flag) {
				//标记位已删除
				bufferManager->writeARecord((BYTE*)DeleteInfo, 8, tableInfo.getTableName(), it.value());
				count++;
			}
		}
		it = it.next();
	}
	auto end = clock();
	auto time = (double)(end - start) / CLOCKS_PER_SEC;
	RecordBuffer rb;
	return QueryResult(Success, count, time, rb);
}

const QueryResult& RecordManager::selectValues(const Table& tableInfo, const vector<Condition>& conditions)
{
	auto start = clock();
	RecordBuffer rb;
	int recordLength = tableInfo.getRecordLength();
	ADDRESS tail = *((int*)(bufferManager->fetchARecord(tableInfo.getTableName(), 0)));
	RecordIterator it(recordLength, tail);
	while (it.hasNext()) {
		auto buffer = bufferManager->fetchARecord(tableInfo.getTableName(), it.value());
		char check[8];
		memcpy(check, buffer, 8);
		if (string(check) != "Deleted") {
			map<string, string> values;
			map<string, Type> types;
			int offset = 0;
			for (auto attribute : tableInfo.getAttributes()) {
				char* buff = new char[attribute.getLength() + 1];
				memcpy(buff, buffer + offset, attribute.getLength());
				buff[attribute.getLength()] = '\0';
				values.insert(pair<string, string>(attribute.getAttributeName(), string(buff)));
				delete []buff;
				types.insert(pair<string, Type>(attribute.getAttributeName(), attribute.getType()));
				offset += attribute.getLength();
			}
			bool flag = true;
			for (auto condition : conditions) {
				if (!condition.FitCondition(values[condition.getAttributeName()], types[condition.getAttributeName()])) {
					flag = false;
				}
			}
			if (flag) {
				string res = "";
				for (auto pair : values) {
					res.append(pair.second);
					res.append(" ");
				}
				rb.addContent(res);
			}
		}
		it = it.next();
	}
	auto end = clock();
	auto time = (double)(end - start) / CLOCKS_PER_SEC;
	return QueryResult(Success, 0, time, rb);
}

const QueryResult& RecordManager::selectValues(const vector<string>& attributes, const Table& tableInfo, const vector<Condition>& conditions)
{
	auto start = clock();
	RecordBuffer rb;
	int recordLength = tableInfo.getRecordLength();
	ADDRESS tail = *((int*)(bufferManager->fetchARecord(tableInfo.getTableName(), 0)));
	RecordIterator it(recordLength, tail);
	while (it.hasNext()) {
		auto buffer = bufferManager->fetchARecord(tableInfo.getTableName(), it.value());
		char check[8];
		memcpy(check, buffer, 8);
		if (string(check) != "Deleted") {
			map<string, string> values;
			map<string, Type> types;
			int offset = 0;
			for (auto attribute : tableInfo.getAttributes()) {
				char* buff = new char[attribute.getLength() + 1];
				memcpy(buff, buffer + offset, attribute.getLength());
				buff[attribute.getLength()] = '\0';
				values.insert(pair<string, string>(attribute.getAttributeName(), string(buff)));
				delete []buff;
				types.insert(pair<string, Type>(attribute.getAttributeName(), attribute.getType()));
				offset += attribute.getLength();
			}
			bool flag = true;
			for (auto condition : conditions) {
				if (!condition.FitCondition(values[condition.getAttributeName()], types[condition.getAttributeName()])) {
					flag = false;
				}
			}
			if (flag) {
				string res = "";
				for (auto attributeName : attributes) {
					res.append(values[attributeName]);
					res.append(" ");
				}
				rb.addContent(res);
			}
		}
		it = it.next();
	}
	auto end = clock();
	auto time = (double)(end - start) / CLOCKS_PER_SEC;
	return QueryResult(Success, 0, time, rb);
}
