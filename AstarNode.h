#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <cmath>
#include <set>
#include <ctime>
#include <queue>

using namespace std;

int g_k;			// 选择启发式函数
int g_n;			// n-Puzzle
int g_countTiles;	//瓷砖数，包括空格瓷砖
int g_states;		//用来记录状态数，不需要定义

enum Move { MOVE_UP = -1, MOVE_DOWN = 1, MOVE_LEFT = -2, MOVE_RIGHT = 2, NA = 0 };

class State
{
public:
	int* m_tiles;
	Move  m_parentAction;

private:
	/*
	获得空瓷砖0的位置
	*/
	int GetBlankTilePosition()
	{
		for (int i = 0; i < g_countTiles; i++)
			if (m_tiles[i] == 0)
				return i;
	}	

	/*
	根据move指令移动当前瓷砖
	*/
	bool MoveBlankTile(Move move)
	{
		int blankPos = GetBlankTilePosition();
		int newPos;

		// 检查指令是否有效，是否在边界上还继续移动
		if (move == MOVE_UP && blankPos / g_n == 0 ||
			move == MOVE_DOWN && blankPos / g_n == g_n - 1 ||
			move == MOVE_LEFT && blankPos % g_n == 0 ||
			move == MOVE_RIGHT && blankPos % g_n == g_n - 1)
			return false;

		// 移动后，获得新的空瓷砖的位置
		if (move == MOVE_UP || move == MOVE_DOWN)
			newPos = blankPos + move*g_n;
		else
			newPos = blankPos + move / 2;

		// 修改瓷砖数组中的值
		m_tiles[blankPos] = m_tiles[newPos];
		m_tiles[newPos] = 0;
		return true;
	}		

	/*
	为当前节点添加其可达到的节点状态
	*/
	void AddSuccessor(vector<State*>& successors, Move move)
	{
		State* newState = new State(m_tiles);
		if (newState->MoveBlankTile(move))
		{
			newState->m_parentAction = move;
			successors.push_back(newState);
		}
		else
			delete newState;
	}	

public:

	/*
	设置Tiles的值，为深拷贝操作
	*/
	void SetTiles(int* tiles)
	{
		m_tiles = new int[g_countTiles];
		for (int i = 0; i < g_countTiles; i++)
			m_tiles[i] = tiles[i];
	}

	State(int* tiles)
	{
		SetTiles(tiles);
	}

	State(const State& state)
	{
		SetTiles(state.m_tiles);
	}

	State() {}

	~State()
	{
		delete[] m_tiles;
	}


	/*
	判断是否是同一状态
	*/
	bool IsSameState(const State& state) const
	{
		for (int i = 0; i < g_countTiles; i++)
			if (m_tiles[i] != state.m_tiles[i])
				return false;

		return true;
	}  



	/*
	找出所有当前结点可到达的状态
	*/
	void GenerateSuccessors(vector<State*>& successors)
	{
		successors.clear();
		// 往四个方向搜索，并且避免往回走
		if (m_parentAction != MOVE_DOWN) AddSuccessor(successors, MOVE_UP);
		if (m_parentAction != MOVE_UP) AddSuccessor(successors, MOVE_DOWN);
		if (m_parentAction != MOVE_RIGHT) AddSuccessor(successors, MOVE_LEFT);
		if (m_parentAction != MOVE_LEFT) AddSuccessor(successors, MOVE_RIGHT);
	}

	/*
	启发式函数1：计算错位的瓷砖的数目（这个启发式函数是效果最差的）
	*/
	int HMisplacedTiles(const State& goalState)
	{
		int h = 0;
		for (int i = 0; i < g_countTiles; i++)
			if (m_tiles[i] != 0 && m_tiles[i] != goalState.m_tiles[i])
				h++;

		return h;
	}	/* -----  end of function HMisplacedTiles  ----- */

	/*
	启发式函数2：计算各个瓷砖上数目到目的状态对应数目的绝对差的总和
	*/
	int HManhattanDistance(const State& goalState)
	{
		int h = 0;
		for (int i = 0; i < g_countTiles; i++)
			if (m_tiles[i] != 0 && m_tiles[i] != goalState.m_tiles[i])
				for (int j = 0; j < g_countTiles; j++)
					if (m_tiles[i] == goalState.m_tiles[j])
					{
						h += abs(float(i%g_n - j%g_n)) + abs(float(i / g_n - j / g_n));
						break;
					}

		return h;
	}	

	/*
	启发式函数3：计算各个瓷砖离对应的正确位置的距离，这里直接利用两个坐标的距离来计算
	*/
	int HCustom(const State& goalState)
	{
		int h = 0;
		for (int i = 0; i < g_countTiles; i++)
			if (m_tiles[i] != 0 && m_tiles[i] != goalState.m_tiles[i])
				for (int j = 0; j < g_countTiles; j++)
					if (m_tiles[i] == goalState.m_tiles[j])
					{
						h += sqrt((i%g_n - j%g_n)*(i%g_n - j%g_n) + (i / g_n - j / g_n)*(i / g_n - j / g_n));
						break;
					}

		return h;
	}

	/*
	利用所选择的启发式函数来计算启发式代价
	*/
	int H(const State& goalState)
	{
		if (g_k == 1)
			return HMisplacedTiles(goalState);
		else if (g_k == 2)
			return HManhattanDistance(goalState);
		else if (g_k == 3)
			return HCustom(goalState);
	}
};

class Node
{
public:
	State m_state;
	int   m_pathCost;  //节点所花的路径代价
	int   m_f;

	Node(State state, int pathCost) : m_state(state), m_pathCost(pathCost) {}

	Node() : m_pathCost(0) {}

	void ComputeF(const State& goalState)
	{
		m_f = m_pathCost + m_state.H(goalState);
	}
};