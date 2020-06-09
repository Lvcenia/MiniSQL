#pragma once
#include <string>
#include <vector>
#include "BPTNode.h"
#include "../Generals.h"
using namespace std;

struct IndexHeader {
	char indexName[32];
	char tableName[32];
	char attributeName[32];
	Type type;
	int attributeLength;
	int offsetInRecord;
	int fanOut;
	int elementCount;
};

class BPTree {
public:
	static bool CompInt(const string& a, const string& b);
	static bool CompChar(const string& a, const string& b);
	static bool CompFloat(const string& a, const string& b);

	BPTree(int m, Type type);
	BPTree(int m, Type type, int attributeLength, int offsetInRecord);
	BPTree(string name, Attribute& attributeInfo, string tableName);
	BPTree(const IndexHeader& header);
	virtual ~BPTree();
	void SetRoot(BPTNode* root);
	void Insert(string key, int address);
	void Delete(string key);
	int FindAddress(string key);
	int GetMaxKeyCount();
	int GetOffset();
	int GetLength();
	int GetElementCount();
	Type GetType();
	BPTLeafNode* GetLeafWithKey(string key);
	BPTLeafNode* GetFirstLeaf();
	const IndexHeader& GetHeader();
	void Clear();

	void Debug();

private:
	BPTNode* root;
	int maxKeyCount;
	int offset;
	int length;
	Type type;
	bool(*Comp)(const string& a, const string& b);
	string name;
	string tableName;
	string attributeName;
	int elementCount;
};

