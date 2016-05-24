#include "AstarNode.h"
#include "AstarSearch.h"
#include <iomanip>

#define PUZZLENUMS 25
#define SETPS 100
bool haveSolve(int *Tiles, int len);
void init();
void Input(State& goalState, Node& initNode);
void RandomInput(State& goalState, Node& initNode);
void Output(int optimalCost, int generatedStates);

int main()
{
	//参数初始化
	init();
	State goalState;
	Node  initNode;
	int result;

	//Input(goalState, initNode);
	RandomInput(goalState, initNode);
	for (int i = 2;i <= 3;i++) {
		//用三种启发式函数分别解决这个数码问题
		g_k = i;
		AStarSearch as = AStarSearch(goalState, initNode);
		result = as.Solve();
		cout << "启发式函数" << i << ":" << endl;
		Output(result, g_states);
		g_states = 1;
	}

	system("pause");
	return 0;
}

bool haveSolve(int *Tiles, int len) {
	return true;
}

void init() {
	/**************************************************************************
	int g_k;			// 选择启发式函数
	int g_n;			// n-Puzzle
	int g_countTiles;	//瓷砖数，包括空格瓷砖
	int g_states;		//用来记录状态数，不需要定义
	***************************************************************************/
	//设置各项参数,
	g_countTiles = PUZZLENUMS;
	//选择启发式函数,默认为2
	g_k = 2;
	//设置n
	g_n = sqrt(g_countTiles);
}

void Input(State& goalState, Node& initNode)
{
	int tile;
	int* initTiles;
	int* goalTiles;
	//分配内存
	initTiles = new int[g_countTiles];
	goalTiles = new int[g_countTiles];

	//输入
	cout << "输入开始状态[0为空格所在位置]" << endl;
	for (int i = 0; i < g_countTiles; i++)
	{
		cin >> tile;
		initTiles[i] = tile;
	}

	if (!haveSolve(initTiles, g_countTiles)) {
		//如果无解的话重新输入
		cout << "输入的序列无解，请重新输入" << endl;
		for (int i = 0; i < g_countTiles; i++)
		{
			cin >> tile;
			initTiles[i] = tile;
		}
	}

	cout << "输入目标状态" << endl;
	for (int i = 0; i < g_countTiles; i++)
	{
		cin >> tile;
		goalTiles[i] = tile;
	}

	goalState.SetTiles(goalTiles);

	initNode.m_state.SetTiles(initTiles);
	initNode.ComputeF(goalState);

	g_states = 1;


	delete[] initTiles;
	delete[] goalTiles;
}

void RandomInput(State& goalState, Node& initNode)
{
	//随机生成一组有解的数码，结果状态固定为0到24
	int tile;
	int* initTiles;
	int* goalTiles;
	//分配内存
	initTiles = new int[g_countTiles];
	goalTiles = new int[g_countTiles];
	srand(time(0));
	//生成随机puzzle和固定序列的从0到24的目标puzzle
	for (int i = 0; i < g_countTiles; i++)
	{
		initTiles[i] = i;
		goalTiles[i] = i;
	}

	//随机移动initTiles，大概为移动100步左右
	int blankpos = 0;
	int steplen[4] = { -1,-5,1,5 };
	for (int i = 0;i <= SETPS ;i++) {
		int randDirect = rand() % 4;
		int goalpos = blankpos + steplen[randDirect];
		if (goalpos >= 0 && goalpos < g_countTiles) {
			if ((blankpos + 1) % g_n == 1 && randDirect == 0 ) {
				//在左边界时，不能往左移动,否则重新结算
				i--;
				continue;
			}
			if ((blankpos + 1) % g_n == 0 && randDirect == 2) {
				//在右边界时，不能往右移动,否则重新结算
				i--;
				continue;

			}
			if (blankpos + 1 <= g_n  && randDirect == 1) {
				//在上边界时，不能往上移动,否则重新结算
				i--;
				continue;
			}
			if (blankpos + 1 >= g_countTiles - g_n && randDirect == 3) {
				//在下边界时，不能往下移动,否则重新结算
				i--;
				continue;
			}
			//如果没有走出边界的话,则行走
			initTiles[blankpos] = initTiles[goalpos];
			initTiles[goalpos] = 0;
			blankpos = goalpos;
			/*for (int i = 0; i < g_countTiles; i++)
			{
				cout << setw(2) << initTiles[i] << " ";
				if ((i + 1) % g_n == 0) {
					cout << endl;
				}
			}*/
			continue;
		}
		i--;
	}

	cout << "生成的随机puzzle如下" << endl;
	for (int i = 0; i < g_countTiles; i++)
	{
		cout<<setw(2)<<initTiles[i]<<" ";
		if ((i + 1)% g_n == 0) {
			cout << endl;
		}
	}

	cout << "目标状态如下:" << endl;
	for (int i = 0; i < g_countTiles; i++)
	{
		cout << setw(2)<< goalTiles[i] << " ";
		if ((i + 1) % g_n == 0) {
			cout << endl;
		}
	}

	goalState.SetTiles(goalTiles);

	initNode.m_state.SetTiles(initTiles);
	initNode.ComputeF(goalState);

	g_states = 1;

	delete[] initTiles;
	delete[] goalTiles;
}



void Output(int optimalCost, int generatedStates)
{
	//输出解决所需的步数和状态数
	cout << "步数:" << optimalCost << " 状态数:" << generatedStates << endl;
}
