#include "BPTree.h"
#include "BPTLeafNode.h"
#include "BPTNoneLeafNode.h"

BPTLeafNode::BPTLeafNode(bool(*comp)(const string& a, const string& b), int m, BPTree* tree) : BPTNode(comp, true, tree, m) {
	leftNode = NULL;
	rightNode = NULL;
}

BPTLeafNode::~BPTLeafNode()
{
}

void BPTLeafNode::AddKey(string key, int address)
{
	if (ContainsKey(key)) {
		throw exception("Duplicate key!");
	}

	addressDictionary.insert(pair<string, int>(key, address));
	this->keys.push_back(key);
	sort(keys.begin(), keys.end(), Comp);

	if (GetKeyCount() > maxKeyCount) {
		this->Split();
	}
}

void BPTLeafNode::DeleteKey(string key)
{
	if (!ContainsKey(key)) {
		throw exception("Key not found!");
	}

	this->RemoveKey(key);
	if (parent != NULL) {
		if (parent->ContainsKey(key)) {
			parent->RemoveKey(key);
			parent->InsertKey(keys[0]);
		}
		if ((int)keys.size() < minKeyCount) {
			this->ReDistribute();
		}
	}
}

int BPTLeafNode::FindKey(string key)
{
	if (!ContainsKey(key)) {
		throw exception("Key not found!");
	}
	return addressDictionary[key];
}

void BPTLeafNode::Debug()
{
	if (parent != NULL) {
		printf("[%s]", parent->GetKeys()->at(0).c_str());
	}
	if (keys.size()  != 0) {
		printf("[%s(%d)", keys[0].c_str(), addressDictionary[keys[0]]);
		for (int i = 1; i < (int)keys.size(); i++) {
			printf(", %s(%d)", keys[i].c_str(), addressDictionary[keys[i]]);
		}
		printf("]");
	}
}

void BPTLeafNode::RemoveKey(string key)
{
	auto iter = find(keys.begin(), keys.end(), key);
	keys.erase(iter);
	auto iter0 = addressDictionary.find(key);
	addressDictionary.erase(iter0);
}

BPTLeafNode * BPTLeafNode::FindLeaf(string key)
{
	if (ContainsKey(key)) {
		return this;
	}
	throw exception("Key not found!");
}

BPTLeafNode * BPTLeafNode::FindFirstLeaf()
{
	return this;
}

int BPTLeafNode::GetAddress(string key)
{
	if (!ContainsKey(key)) {
		throw exception("Key not found!");
	}
	return addressDictionary[key];
}

BPTLeafNode * BPTLeafNode::GetRightNode()
{
	return rightNode;
}

BPTLeafNode * BPTLeafNode::GetLeftNode()
{
	return leftNode;
}

void BPTLeafNode::Split()
{
	auto rNode = new BPTLeafNode(Comp, maxKeyCount, bPTree);
	rNode->rightNode = this->rightNode;
	rNode->leftNode = this;
	this->rightNode = rNode;

	//keys & map
	rNode->keys = vector<string>(keys.begin() + GetKeyCount() / 2, keys.end());
	this->keys = vector<string>(keys.begin(), keys.begin() + GetKeyCount() / 2);
	for (auto key : rNode->keys) {
		auto iter = addressDictionary.find(key);
		rNode->addressDictionary.insert(pair<string, int>(key, iter->second));
		addressDictionary.erase(iter);
	}
	//parent
	if (this->parent == NULL) {
		auto root = new BPTNoneLeafNode(Comp, maxKeyCount, bPTree);
		bPTree->SetRoot(root);

		root->AddKey(rNode->keys[0]);
		root->GetChildren()->push_back(this);
		root->GetChildren()->push_back(rNode);
		this->parent = root;
		rNode->parent = root;
	}
	else {
		rNode->parent = parent;
		auto children = ((BPTNoneLeafNode*)parent)->GetChildren();
		auto iter = find(children->begin(), children->end(), this);
		children->insert(iter + 1, rNode);
		((BPTNoneLeafNode*)parent)->AddKey(rNode->keys[0]);
	}
}

void BPTLeafNode::ReDistribute()
{
	if (leftNode != NULL && leftNode->IsSiblingWith(this) && leftNode->GetKeyCount() > minKeyCount) {
		auto key = leftNode->keys[leftNode->GetKeyCount() - 1];
		//parent
		parent->RemoveKey(keys[0]);
		parent->InsertKey(key);
		//keys
		keys.insert(keys.begin(), key);
		//addr
		addressDictionary.insert(pair<string, int>(key, leftNode->addressDictionary[key]));
		leftNode->RemoveKey(key);
		return;
	}
	if (rightNode != NULL && rightNode->IsSiblingWith(this) && rightNode->GetKeyCount() > minKeyCount) {
		auto key = rightNode->keys[0];
		//parent
		parent->RemoveKey(key);
		parent->InsertKey(rightNode->keys[1]);
		//keys
		keys.push_back(key);
		//addr
		addressDictionary.insert(pair<string, int>(key, rightNode->addressDictionary[key]));
		rightNode->RemoveKey(key);
		return;
	}
	if (leftNode != NULL && leftNode->IsSiblingWith(this)) {
		auto key = keys[0];
		//keys&addr
		for (int i = leftNode->GetKeyCount() - 1; i >= 0; i--) {
			this->InsertKey(leftNode->GetKey(i), 0);
		}
		for (auto pair : leftNode->addressDictionary) {
			this->addressDictionary.insert(pair);
		}
		//parent&leftNode
		((BPTNoneLeafNode*)parent)->RemoveChild(leftNode);
		auto temp = leftNode;
		leftNode = temp->leftNode;
		if (leftNode != NULL) {
			leftNode->rightNode = this;
		}
		delete temp;
		((BPTNoneLeafNode*)parent)->ClearKey(key);
		return;
	}
	if (rightNode != NULL && rightNode->IsSiblingWith(this)) {
		auto key = rightNode->keys[0];
		//keys&addr
		for (auto key : rightNode->keys) {
			this->keys.push_back(key);
		}
		for (auto pair : rightNode->addressDictionary) {
			this->addressDictionary.insert(pair);
		}
		//parent&rightNode
		((BPTNoneLeafNode*)parent)->RemoveChild(rightNode);
		auto temp = rightNode;
		rightNode = temp->rightNode;
		if (rightNode != NULL) {
			rightNode->leftNode = this;
		}
		delete temp;
		((BPTNoneLeafNode*)parent)->ClearKey(key);
		return;
	}
}
