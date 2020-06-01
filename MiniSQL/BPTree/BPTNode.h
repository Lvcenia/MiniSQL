#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
using namespace std;

class BPTree;
class BPTLeafNode;

class BPTNode {
public:
	BPTNode(bool(*comp)(const string& a, const string& b), bool isLeaf, BPTree* tree, int m);
	virtual ~BPTNode();

	bool IsLeaf();
	void SetIsLeaf(bool val);
	BPTNode* GetParent();
	void SetParent(BPTNode* parent);
	vector<string>* GetKeys();
	string GetKey(int index);
	bool ContainsKey(string key);
	int GetKeyCount();
	virtual void RemoveKey(string key);//ºÚµ•“∆≥˝
	void InsertKey(string key);//ºÚµ•≤Â»Î+≈≈–Ú
	void InsertKey(string key, int index);
	bool IsSiblingWith(BPTNode* node);

	virtual void AddKey(string key, int address) = 0;
	virtual void DeleteKey(string key) = 0;
	virtual int FindKey(string key) = 0;
	virtual void Debug() = 0;
	virtual BPTLeafNode* FindLeaf(string key) = 0;
	virtual BPTLeafNode* FindFirstLeaf() = 0;

protected:
	BPTree* bPTree;
	bool isLeaf;
	BPTNode* parent;
	vector<string> keys;
	bool(*Comp)(const string& a, const string& b);
	int maxKeyCount;
	int minKeyCount;
};
