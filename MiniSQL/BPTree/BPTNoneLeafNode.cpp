#include "BPTNoneLeafNode.h"
#include "BPTree.h"

BPTNoneLeafNode::BPTNoneLeafNode(bool(*comp)(const string& a, const string& b), int m, BPTree* tree) : BPTNode(comp, false, tree, m) {
	leftNode = NULL;
	rightNode = NULL;
}

BPTNoneLeafNode::~BPTNoneLeafNode()
{
	for (auto node : children) {
		delete node;
	}
}

vector<BPTNode*>* BPTNoneLeafNode::GetChildren()
{
	return &children;
}

int BPTNoneLeafNode::GetChildrenCount()
{
	return children.size();
}

void BPTNoneLeafNode::RemoveChild(BPTNode * child)
{
	auto iter = find(children.begin(), children.end(), child);
	if (iter != children.end()) {
		children.erase(iter);
	}
}

void BPTNoneLeafNode::AddKey(string key, int address)
{
	if (ContainsKey(key)) {
		throw exception("Duplicate key!");
	}
	int i;
	for (i = 0; i < (int)keys.size(); i++) {
		if (Comp(key, keys[i])) {
			break;
		}
	}
	children[i]->AddKey(key, address);
}

void BPTNoneLeafNode::AddKey(string key)
{
	if (ContainsKey(key)) {
		throw exception("Duplicate key!");
	}
	keys.push_back(key);
	sort(keys.begin(), keys.end(), Comp);

	if ((int)keys.size() > maxKeyCount) {
		this->Split();
	}
}

void BPTNoneLeafNode::DeleteKey(string key)
{
	int i;
	for (i = 0; i < (int)keys.size(); i++) {
		if (Comp(key, keys[i])) {
			break;
		}
	}
	children[i]->DeleteKey(key);
}

int BPTNoneLeafNode::FindKey(string key)
{
	int i;
	for (i = 0; i < (int)keys.size(); i++) {
		if (Comp(key, keys[i])) {
			break;
		}
	}
	return children[i]->FindKey(key);
}

void BPTNoneLeafNode::ClearKey(string key)
{
	if (!ContainsKey(key)) {
		throw exception("Key not found!");
	}
	this->RemoveKey(key);
	if (parent != NULL && GetKeyCount() < minKeyCount) {
		this->ReDistribute();
	}
	else if (parent == NULL && GetKeyCount() == 0) {
		bPTree->SetRoot(children[0]);
		children[0]->SetParent(NULL);
		children.clear();
		delete this;
	}
}

void BPTNoneLeafNode::Debug()
{
	if (parent != NULL) {
		printf("[%s]", parent->GetKeys()->at(0).c_str());
	}
	printf("[%s", keys[0].c_str());
	for (int i = 1; i < (int)keys.size(); i++) {
		printf(", %s", keys[i].c_str());
	}
	printf("]");
}

BPTLeafNode * BPTNoneLeafNode::FindLeaf(string key)
{
	int i;
	for (i = 0; i < (int)keys.size(); i++) {
		if (Comp(key, keys[i])) {
			break;
		}
	}
	return children[i]->FindLeaf(key);
}

BPTLeafNode * BPTNoneLeafNode::FindFirstLeaf()
{
	return children[0]->FindFirstLeaf();
}

void BPTNoneLeafNode::Split()
{
	auto rNode = new BPTNoneLeafNode(Comp, maxKeyCount, bPTree);
	rNode->leftNode = this;
	rNode->rightNode = this->rightNode;
	this->rightNode = rNode;
	//keys
	auto key = *(keys.begin() + GetKeyCount() / 2);
	rNode->keys = vector<string>(keys.begin() + GetKeyCount() / 2 + 1, keys.end());
	this->keys = vector<string>(keys.begin(), keys.begin() + GetKeyCount() / 2);
	//children
	rNode->children = vector<BPTNode*>(children.begin() + GetKeyCount() + 1, children.end());
	this->children = vector<BPTNode*>(children.begin(), children.begin() + GetKeyCount() + 1);
	for (auto child : rNode->children) {
		child->SetParent(rNode);
	}

	//parent
	if (this->parent == NULL) {
		auto root = new BPTNoneLeafNode(Comp, maxKeyCount, bPTree);
		bPTree->SetRoot(root);
		root->keys.push_back(key);
		root->children.push_back(this);
		root->children.push_back(rNode);
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

void BPTNoneLeafNode::ReDistribute()
{
	if (leftNode != NULL && leftNode->IsSiblingWith(this) && leftNode->GetKeyCount() > minKeyCount) {
		//keys
		auto pchildren = ((BPTNoneLeafNode*)parent)->GetChildren();
		auto iter = find(pchildren->begin(), pchildren->end(), leftNode);
		int index = iter - pchildren->begin();
		auto pkey = parent->GetKey(index);
		auto lkey = leftNode->keys[leftNode->GetKeyCount() - 1];
		leftNode->RemoveKey(lkey);
		parent->RemoveKey(pkey);
		parent->InsertKey(lkey, index);
		this->InsertKey(pkey, 0);
		//children
		auto child = leftNode->children[leftNode->GetChildrenCount() - 1];
		leftNode->RemoveChild(child);
		this->children.insert(this->children.begin(), child);
		child->SetParent(this);
		return;
	}
	if (rightNode != NULL && rightNode->IsSiblingWith(this) && rightNode->GetKeyCount() > minKeyCount) {
		//keys
		auto pchildren = ((BPTNoneLeafNode*)parent)->GetChildren();
		auto iter = find(pchildren->begin(), pchildren->end(), this);
		int index = iter - pchildren->begin();
		auto pkey = parent->GetKey(index);
		auto rkey = rightNode->keys[0];
		rightNode->RemoveKey(rkey);
		parent->RemoveKey(pkey);
		parent->InsertKey(rkey, index);
		this->InsertKey(pkey, GetKeyCount());
		//children
		auto child = rightNode->children[0];
		rightNode->RemoveChild(child);
		this->children.push_back(child);
		child->SetParent(this);
		return;
	}
	if (leftNode != NULL && leftNode->IsSiblingWith(this)) {
		//keys
		auto pchildren = ((BPTNoneLeafNode*)parent)->GetChildren();
		auto iter = find(pchildren->begin(), pchildren->end(), leftNode);
		int index = iter - pchildren->begin();
		auto pkey = parent->GetKey(index);
		this->InsertKey(pkey, 0);
		for (int i = leftNode->GetKeyCount() - 1; i >= 0; i--) {
			this->InsertKey(leftNode->GetKey(i), 0);
		}
		//children
		for (int i = leftNode->GetChildrenCount() - 1; i >= 0; i--) {
			this->children.insert(children.begin(), leftNode->children[i]);
			leftNode->children[i]->SetParent(this);
		}
		leftNode->children.clear();
		//parent
		((BPTNoneLeafNode*)parent)->RemoveChild(leftNode);
		auto temp = leftNode;
		leftNode = temp->leftNode;
		if (leftNode != NULL) {
			leftNode->rightNode = this;
		}
		delete temp;
		((BPTNoneLeafNode*)parent)->ClearKey(pkey);
		return;
	}
	if (rightNode != NULL && rightNode->IsSiblingWith(this)) {
		//keys
		auto pchildren = ((BPTNoneLeafNode*)parent)->GetChildren();
		auto iter = find(pchildren->begin(), pchildren->end(), this);
		int index = iter - pchildren->begin();
		auto pkey = parent->GetKey(index);
		this->keys.push_back(pkey);
		for (auto key : rightNode->keys) {
			this->keys.push_back(key);
		}
		//children
		for (auto child : rightNode->children) {
			this->children.push_back(child);
			child->SetParent(this);
		}
		rightNode->children.clear();
		//parent
		((BPTNoneLeafNode*)parent)->RemoveChild(rightNode);
		auto temp = rightNode;
		rightNode = temp->rightNode;
		if (rightNode != NULL) {
			rightNode->leftNode = this;
		}
		delete temp;
		((BPTNoneLeafNode*)parent)->ClearKey(pkey);
		return;
	}
}
