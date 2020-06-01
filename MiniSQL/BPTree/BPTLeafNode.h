#pragma once
#include "BPTNode.h"

class BPTLeafNode : public BPTNode {
public:
	BPTLeafNode(bool(*comp)(const string& a, const string& b), int m, BPTree* tree);
	~BPTLeafNode();

	void AddKey(string key, int address);
	void DeleteKey(string key);
	int FindKey(string key);
	void Debug();
	void RemoveKey(string key);
	BPTLeafNode* FindLeaf(string key);
	BPTLeafNode* FindFirstLeaf();
	int GetAddress(string key);
	BPTLeafNode* GetRightNode();
	BPTLeafNode* GetLeftNode();

private:
	map<string, int> addressDictionary;
	BPTLeafNode* leftNode;
	BPTLeafNode* rightNode;

	void Split();
	void ReDistribute();
};

