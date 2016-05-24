#pragma once
#include "AstarNode.h"
class AStarSearch
{
private:

	/*
	用于比较两个节点g值，如果f相等则比较pathCost
	*/
	class Compare_f
	{
	public:
		bool operator() (const Node *x, const Node *y) const
		{
			if (x->m_f == y->m_f)
				return x->m_pathCost < y->m_pathCost;
			else
				return x->m_f > y->m_f;
		}
	};

	/*
	用于判断两个节点的状态是否一样
	*/
	class Compare_state
	{
	public:
		bool operator() (const Node* x, const Node* y) const
		{
			for (int i = 0; i < g_countTiles; i++)
				if (x->m_state.m_tiles[i] != y->m_state.m_tiles[i])
					return x->m_state.m_tiles[i] < y->m_state.m_tiles[i];

			return false;
		}
	};

public:
	State m_goalState;
	priority_queue<Node*, vector<Node*>, Compare_f> m_openNodes;		// open nodes
	set<Node*, Compare_state> m_allNodes;								// generated nodes

	AStarSearch(const State& goalState, Node& initNode) : m_goalState(goalState)
	{
		//初始节点没有父亲节点
		initNode.m_state.m_parentAction = NA;

		m_openNodes.push(&initNode);
		m_allNodes.insert(&initNode);
	}

	~AStarSearch()
	{
		//无需释放资源
	}

	/*
	解决N-puzzle问题，如果返回-1就是无法解决
	*/
	int Solve()
	{
		vector<State*> successors;		// 临时存储某节点作为父节点时，可达到的节点状态
		set<Node*>::iterator existed;	// 检查新节点是否已存在
		Node* newNode;
		int newPathCost;

		//如果开启列表不为空就一直检查
		while (!m_openNodes.empty())
		{
			//清空临时列表
			for (int i = 0; i < successors.size(); i++)
				delete successors[i];
			successors.clear();

			// 获得F()值最低的节点
			Node *bestNode = m_openNodes.top();
			m_openNodes.pop();

			// 如果当前结点已经是目标节点，返回当前结点的实际代价
			if (bestNode->m_state.IsSameState(m_goalState))
				return bestNode->m_pathCost;

			// 如果不是则进行下面的操作

			// 为当前的新节点添加其可到达的子节点
			newPathCost = bestNode->m_pathCost + 1;
			bestNode->m_state.GenerateSuccessors(successors);

			//检查各个子节点状态
			for (int i = 0; i < successors.size(); i++)
			{
				newNode = new Node(*successors[i], newPathCost);

				//重复状态检测，看是否已经出现过该节点
				existed = m_allNodes.find(newNode);
				if (existed != m_allNodes.end())
				{
					//如果已经出现过该节点，判断其节点原来的路径跟现在到达的哪个短
					if ((*existed)->m_pathCost <= newPathCost)
					{
						//如果已存在的节点比现在的节点更好，花费更少，就不做任何操作，判断下一个子节点
						continue;
					}
					else
					{
						m_allNodes.erase(existed);
					}
				}
				else g_states++;

				//节点通过测试
				newNode->ComputeF(m_goalState);

				m_openNodes.push(newNode);
				m_allNodes.insert(newNode);
			}

			//对迭代次数做出限制，如果超过了这个次数判断为失败
			if ((g_n == 3 && g_states >= 250000) ||
				(g_n == 4 && g_states >= 600000) ||
				(g_n >= 5 && g_states >= 1000000))
				return -1;
		}

		return -1;

	}
};