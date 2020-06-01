#include "BPTNode.h"

BPTNode::BPTNode(bool(*comp)(const string& a, const string& b), bool isLeaf, BPTree* tree, int m)
{
	this->isLeaf = isLeaf;
	this->parent = NULL;
	this->Comp = comp;
	this->bPTree = tree;
	this->maxKeyCount = m;
	this->minKeyCount = (int)((m + 1) / 2.0 + 0.5) - 1;
}

BPTNode::~BPTNode()
{
}

bool BPTNode::IsLeaf()
{
	return isLeaf;
}

void BPTNode::SetIsLeaf(bool val)
{
	isLeaf = val;
}

BPTNode* BPTNode::GetParent()
{
	return parent;
}

void BPTNode::SetParent(BPTNode* parent)
{
	this->parent = parent;
}

vector<string>* BPTNode::GetKeys()
{
	return &keys;
}

string BPTNode::GetKey(int index)
{
	return keys[index];
}

int BPTNode::GetKeyCount()
{
	return keys.size();
}

void BPTNode::RemoveKey(string key)
{
	auto iter = find(keys.begin(), keys.end(), key);
	if (iter != keys.end()) {
		keys.erase(iter);
	}
}

void BPTNode::InsertKey(string key)
{
	keys.push_back(key);
	sort(keys.begin(), keys.end(), Comp);
}

void BPTNode::InsertKey(string key, int index)
{
	keys.insert(keys.begin() + index, key);
}

bool BPTNode::IsSiblingWith(BPTNode * node)
{
	return this->parent == node->GetParent();
}

bool BPTNode::ContainsKey(string key)
{
	auto res = find(keys.begin(), keys.end(), key);
	return res != keys.end();
}