#pragma once
#include<queue>
#include<vector>
#include"FileCompress.h"

template<class W>
//hafuman树节点结构
struct HTNode
{
	HTNode(const W& weight)
		:_pLeft(nullptr)
		, _pRight(nullptr)
		,_pParent(nullptr)
		, _weight(weight)
	{}

	HTNode<W>* _pLeft;
	HTNode<W>* _pRight;
	HTNode<W>* _pParent;
	W _weight;
};

template<class W>
//通过仿函数来实现比较优先级队列底层建堆的比较规则
//其实就是对()的重载，让类能够由函数调用的功能
struct Compare
{
	bool operator()(HTNode<W>* pLeft, HTNode<W>* pRight)
	{
		if (pLeft->_weight >= pRight->_weight)
			return true;
		return false;
	}
};

//huffman树结构
template<class W>
class HuffmanTree
{
	typedef HTNode<W> Node;
	typedef Node* PNode;
public:
	HuffmanTree()
		: _pRoot(nullptr)
	{}
	~HuffmanTree()
	{
		Destroy(_pRoot);
	}
	void CreatHuffmanTree(const  std::vector<W>&  v,const W& invalid)
	{
		if (v.empty())
			return;
		//用的是小堆创建的优先级队列
		//创建二叉树的森林
		std::priority_queue<PNode, std::vector<PNode>, Compare<W>> hp;
		for (size_t i = 0; i < v.size(); ++i)
		{
			if(v[i]!=invalid)
				hp.push(new Node(v[i]));
		}
//从堆中获取到权值最小的两棵二叉树
		while (hp.size() > 1)
		{
			PNode pLeft = hp.top();
			hp.pop();

			PNode pRight = hp.top();
			hp.pop();

			//以pLeft与pRight权值之和创建新节点
			PNode pParent = new Node(pLeft->_weight + pRight->_weight);
			pParent->_pLeft = pLeft;
			pLeft->_pParent = pParent;
			
			pParent->_pRight = pRight;
			pRight->_pParent = pParent;

			hp.push(pParent);
		}
		_pRoot = hp.top();
	}
	PNode GetRoot()
	{
		return _pRoot;
	}
private:
	//销毁这个二叉树
	void Destroy(PNode& pRoot)
	{
		if (pRoot)
		{
			Destroy(pRoot->_pLeft);
			Destroy(pRoot->_pRight);
			delete(pRoot);
			pRoot = nullptr;
		}
	}
private:
	PNode _pRoot;
};