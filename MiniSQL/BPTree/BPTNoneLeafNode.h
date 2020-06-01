#pragma once
#include "BPTNode.h"

class BPTNoneLeafNode : public BPTNode {
public:
	BPTNoneLeafNode(bool(*comp)(const string& a, const string& b), int m, BPTree* tree);
	~BPTNoneLeafNode();

	vector<BPTNode*>* GetChildren();
	int GetChildrenCount();
	void RemoveChild(BPTNode* child);
	void AddKey(string key, int address);
	void AddKey(string key);
	void DeleteKey(string key);
	int FindKey(string key);
	void ClearKey(string key);
	void Debug();
	BPTLeafNode* FindLeaf(string key);
	BPTLeafNode* FindFirstLeaf();

private:
	vector<BPTNode*> children;
	BPTNoneLeafNode* leftNode;
	BPTNoneLeafNode* rightNode;

	void Split();
	void ReDistribute();
};

