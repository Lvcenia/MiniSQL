#include "IndexManager.h"
#include "BPTree/BPTLeafNode.h"
#include <ctime>

IndexManager::IndexManager()
{
	this->bufferManager = BufferManager::getBufferManager();
}

IndexManager::IndexManager(list<string> indexNames) : IndexManager()
{
	for (auto indexName : indexNames) {
		this->CreateFromFile(indexName);
	}
}

IndexManager::~IndexManager()
{
	for (auto pair : indexDictionary) {
		SaveToFile(pair.first);
		delete pair.second;
	}
}

const QueryResult & IndexManager::createIndex(const string & indexName, Attribute & attribute, const int & recordLength, const string & fileName)
{
	auto start = clock();
	auto index = new BPTree(indexName, attribute, fileName);
	indexDictionary.insert(pair<string, BPTree*>(indexName, index));
	auto endOffset = this->getEndOffset(fileName);
	for (ADDRESS recordOffset = HEADER_BLOCK_OFFSET; recordOffset < endOffset; recordOffset += recordLength)
	{
		if ((recordOffset / 4096 + 1) * 4096 - recordOffset < recordLength&&recordOffset % 4096 != 0)
			recordOffset = (recordOffset / 4096 + 1) * 4096;/*eliminate the tail of a block*/
		auto recordData = bufferManager->fetchARecord(fileName, recordOffset + attribute.getOffset());/*point to the start of the attribute*/
		char* key = new char[sizeof(recordData)];
		memcpy(key, recordData, sizeof(recordData));
		string keyVal(key);
		delete key;
		index->Insert(key, recordOffset);
	}
	SaveToFile(indexName);
	auto end = clock();
	auto time = (double)(end - start) / CLOCKS_PER_SEC;
	RecordBuffer rb;
	return QueryResult(Success, 0, time, rb);
}

const QueryResult & IndexManager::dropIndex(const string & indexName)
{
	auto start = clock();
	delete indexDictionary[indexName];
	auto iter = indexDictionary.find(indexName);
	indexDictionary.erase(iter);

	bufferManager->delFile(indexName);
	auto end = clock();
	auto time = (double)(end - start) / CLOCKS_PER_SEC;
	RecordBuffer rb;
	return QueryResult(Success, 0, time, rb);
}

const QueryResult & IndexManager::deleteValues(const string & indexName, const list<string>& indexList, vector<Condition> conditions, const string & fileName, const int & recordLength)
{
	auto start = clock();
	auto currentIndex = indexDictionary[indexName];
	if (conditions.size() != 0) {
		auto res = Condition::GetMinimalConditoins(conditions, currentIndex->GetType());
		if (res) {

		}
		else {
			auto end = clock();
			auto time = (double)(end - start) / CLOCKS_PER_SEC;
			RecordBuffer rb;
			return QueryResult(Success, 0, time, rb);
		}
	}
	else {

	}
	
}

void IndexManager::CreateFromFile(string name)
{
	try
	{
		IndexHeader header = *(IndexHeader*)(bufferManager->fetchARecord(name, 0));
		auto index = new BPTree(header);
		indexDictionary.insert(pair<string, BPTree*>(name, index));
		ADDRESS curser = HEADER_BLOCK_OFFSET;
		for (int i = 0; i < header.elementCount; i++)
		{
			if ((curser / 4096 + 1) * 4096 - curser < header.attributeLength + 1 + sizeof(ADDRESS) && curser % 4096 != 0)
				curser = (curser / 4096 + 1) * 4096;/*eliminate the tail of a block*/
			string key = (char *)bufferManager->fetchARecord(name, curser + sizeof(ADDRESS));
			indexDictionary[name]->Insert(key, *(ADDRESS *)bufferManager->fetchARecord(name, curser));
			curser += header.attributeLength + 1 + sizeof(ADDRESS);
		}
	}
	catch (const std::exception& e)
	{
		throw e;
	}
}

void IndexManager::SaveToFile(string name)
{
	auto header = indexDictionary[name]->GetHeader();
	bufferManager->writeARecord((BYTE*)&header, sizeof(IndexHeader), name, 0);
}

ADDRESS IndexManager::getEndOffset(const string & fileName)
{
	return *(ADDRESS *)bufferManager->fetchARecord(fileName, 0);
}

ADDRESS IndexManager::getNextToEndOffset(const string & fileName, const int & recordLength)
{
	ADDRESS endOffset = *(ADDRESS *)bufferManager->fetchARecord(fileName, 0);
	if (endOffset % 4096 == 0)/*If the first record is reached*/
		return endOffset - (4096 - 4096 / recordLength * recordLength) - recordLength;/*eliminate the tail and minus the record length*/
	else
		return endOffset - recordLength;
}

void IndexManager::deleteRecordFromFile(const string & keyIndexName, const list<string>& indexList, const string & fileName, const ADDRESS & recordOffset, const int & recordLength)
{
	if (recordOffset == getNextToEndOffset(fileName, recordLength))/*renew End offset and delete key in every index that is not key index*/
	{
		renewEndOffset(fileName, recordLength);
		for (auto indexName : indexList)
		{
			if (indexName == keyIndexName)
				continue;
			auto currentIndex = indexDictionary[indexName];/*the current index*/
			BYTE* deleteRecordKey = bufferManager->fetchARecord(fileName, recordOffset + indexDictionary[indexName]->GetOffset());/*the key of record to be deleted*/

			char* tmpRecordString = new char[currentIndex->GetLength() + 1];
			memcpy(tmpRecordString, deleteRecordKey, currentIndex->GetLength());
			string recordString(tmpRecordString);
			delete tmpRecordString;
			currentIndex->Delete(recordString);
		}
		return;
	}
	/*Insert the keyvalue of end record to every Index and for index that is not key index delete it*/
	ADDRESS renewedEndOffset = getNextToEndOffset(fileName, recordLength); /*renewedOffset*/
	for (auto indexName : indexList)/*For every index that exists in the table*/
	{
		auto currentIndex = indexDictionary[indexName];/*the current index*/
		BYTE* deleteRecordKey = bufferManager->fetchARecord(fileName, recordOffset + indexDictionary[indexName]->GetOffset());/*the key of record to be deleted*/
		BYTE* endRecordKey = bufferManager->fetchARecord(fileName, renewedEndOffset + indexDictionary[indexName]->GetOffset());/*the key of end record*/
		char* tmpRecordString = new char[currentIndex->GetLength() + 1];
		memcpy(tmpRecordString, deleteRecordKey, currentIndex->GetLength());
		string recordString(tmpRecordString);
		delete tmpRecordString;
		currentIndex->Delete(recordString);
	}
	bufferManager->writeARecord(bufferManager->fetchARecord(fileName, renewedEndOffset), recordLength, fileName, recordOffset);/*Write the last record of data on the space of the deleted record*/
	renewEndOffset(fileName, recordLength);/*renew the end offset in header*/
}

void IndexManager::renewEndOffset(const string & fileName, const int & recordLength)
{
	auto renewedOffset = getNextToEndOffset(fileName, recordLength);
	bufferManager->writeARecord((BYTE *)&renewedOffset, recordLength, fileName, 0);
}