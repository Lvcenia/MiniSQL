#pragma once
#include <string>
#include <vector>
#include "BPTNode.h"
using namespace std;

class BPTree {
public:
	static bool CompInt(const string& a, const string& b);
	static bool CompChar(const string& a, const string& b);
	static bool CompFloat(const string& a, const string& b);

	BPTree(int m, Type type);
	BPTree(int m, Type type, int attributeLength, int offsetInRecord);
	virtual ~BPTree();
	void SetRoot(BPTNode* root);
	void Insert(string key, int address);
	void Delete(string key);
	int FindAddress(string key);
	int GetMaxKeyCount();
	int GetOffset();
	int GetLength();
	Type GetType();
	BPTLeafNode* GetLeafWithKey(string key);
	BPTLeafNode* GetFirstLeaf();

	void Debug();

private:
	BPTNode* root;
	int maxKeyCount;
	int offset;
	int length;
	Type type;
	bool(*Comp)(const string& a, const string& b);
};
