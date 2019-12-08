#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <stack>

using namespace std;

vector<vector<int>> adjacency_list;
vector <int> walk_list;
int* weight_matrix;
int* weight_matrix_copy;
int* parent;
int from, to;
int vertex_count;

void print_weight_matrix();
void get_weight_matrix(char* file_path);
void weight_matrix_to_adjacency_list();
void print_adjacency_list();

void get_weight_matrix(char* file_path)
{
	vertex_count = 0;
	int i = 0, j;
	char buf[100];
	FILE* file;
	if ((file = fopen(file_path, "r")) == NULL)
	{
		printf("> ќшибка открыти€ файла\n");
		return;
	}
	fgets(buf, 100, file);
	while (buf[i] != '\n')
	{
		if (buf[i] != 32 && buf[i] != '-' && !(buf[i - 1] >= '0' && buf[i - 1] <= '9'))
			vertex_count++;
		i++;
	}
	weight_matrix = new int[(vertex_count + 1) * (vertex_count + 1)];
	memset(weight_matrix, 0, (vertex_count + 1) * (vertex_count + 1) * sizeof(int));
	for (i = 1; i < vertex_count + 1; i++)
	{
		weight_matrix[i] = i;
		weight_matrix[i * (vertex_count + 1)] = i;
	}
	fseek(file, 0, SEEK_SET);
	while (!feof(file))
		for (i = 1; i < vertex_count + 1; i++)
			for (j = 1; j < vertex_count + 1; j++)
				fscanf_s(file, "%d", &weight_matrix[i * (vertex_count + 1) + j]);
	fclose(file);
	print_weight_matrix();
	weight_matrix_to_adjacency_list();
	print_adjacency_list();
	return;
}

void print_weight_matrix()
{
	printf("> ћатрица весов:\n");
	for (int i = 0; i < vertex_count + 1; i++)
	{
		for (int j = 0; j < vertex_count + 1; j++)
			printf("%d ", weight_matrix[i * (vertex_count + 1) + j]);
		printf("\n");
	}
	return;
}

void weight_matrix_to_adjacency_list()
{
	for (int i = 1; i < vertex_count + 1; i++)
	{
		adjacency_list.push_back(vector<int>());
		adjacency_list[i - 1].push_back(weight_matrix[i]);
		for (int j = 1; j < vertex_count + 1; j++)
		{
			if (weight_matrix[i * (vertex_count + 1) + j] != 0)
				adjacency_list[i - 1].push_back(weight_matrix[j]);
		}
	}
	return;
}

void print_adjacency_list()
{
	printf("> —писок смежности:\n");
	for (int i = 0; i < adjacency_list.size(); i++)
	{
		printf("{%d", adjacency_list[i][0]);
		for (int j = 1; j < adjacency_list[i].size(); j++)
		{
			printf(" %d", adjacency_list[i][j]);
		}
		printf("}\n");
	}
	return;
}

int min(int _value_1, int _value_2)
{
	if (_value_1 < _value_2)
		return _value_1;
	else
		return _value_2;
}

int find_source(int* matrix)
{
	for (int i = 0; i < vertex_count; i++) {
		int flag = 0;
		for (int j = 0; j < vertex_count; j++) {
			if (matrix[j * vertex_count + i] != 0) {
				flag++;
				break;
			}
		}
		if (!flag)
			return i;
	}
	cout << "[find_source]: not found" << endl;
	return INT_MAX;
}

int find_stock(int* matrix)
{
	for (int i = 0; i < vertex_count; i++) {
		int flag = 0;
		for (int j = 0; j < vertex_count; j++) {
			if (matrix[i * vertex_count + j] != 0) {
				flag = 1;
				break;
			}
		}
		if (!flag)
			return i;
	}
	return INT_MAX;
}

int dfs(int from, int to)
{
	bool* visited = new bool[vertex_count];
	memset(visited, false, sizeof(bool) * vertex_count); //important
	
	stack<int> dfs_stack;
	dfs_stack.push(from);

	visited[from] = true;
	parent[from] = -1;
	
	//цикл поиска в глубину
	//просто кидаем все вершины, смежные с текущей в стек, после чего
	//из стека достаем одну и все повтор€етс€
	while (!dfs_stack.empty()) {
		int i = dfs_stack.top();
		dfs_stack.pop();
		for (int j = 0; j < vertex_count; j++) {
			if ((visited[j] == false) && (weight_matrix_copy[i * vertex_count + j] > 0)) {
				dfs_stack.push(j);
				parent[j] = i;
				visited[j] = true;
			}
		}
	}

	//пока не дошли до стока, будет выполн€тьс€ цикл
	//в ford_fulkerson()
	return (visited[to] == true);
}

void ford_fulkerson()
{
	//копирование матрицы весов (матрицы пропускных способностей)
	weight_matrix_copy = new int[vertex_count * vertex_count];
	for (int i = 0; i < vertex_count; i++) {
		for (int j = 0; j < vertex_count; j++) {
			weight_matrix_copy[i * vertex_count + j] = weight_matrix[(i + 1) * (vertex_count + 1) + j + 1];
		}
	}

	//напечатать исходную
	/*cout << "[ford]: start maxtrix:" << endl;
	for (int i = 0; i < vertex_count; i++) {
		for (int j = 0; j < vertex_count; j++) {
			cout << weight_matrix_copy[i * vertex_count + j] << " ";
		}
		cout << endl;
	}
	cout << endl;*/
	
	from = find_source(weight_matrix_copy), to = find_stock(weight_matrix_copy);
	cout << "[ford]: source = " << from + 1 << endl;
	cout << "[ford]: stock = " << to + 1 << endl;

	int max_flow = 0;
	parent = new int[vertex_count];
	while (dfs(from, to))
	{
		//надо посмотреть минимальную пропускную способность
		int path_flow = INT_MAX;
		for (int j = to; j != from; j = parent[j])
		{
			int i = parent[j];
			path_flow = min(path_flow, weight_matrix_copy[i * vertex_count + j]);
		}

		//обновл€ем пропускные способности ребер
		cout << "[ford]: ";
		for (int j = to; j != from; j = parent[j])
		{
			cout << j + 1 << " <- ";
			int i = parent[j];
			weight_matrix_copy[i * vertex_count + j] -= path_flow;
			weight_matrix_copy[j * vertex_count + i] += path_flow;
		}
		cout << from + 1 << endl;

		//напечатать промежуточную
		/*cout << "[ford]: maxtrix:" << endl;
		for (int i = 0; i < vertex_count; i++) {
			for (int j = 0; j < vertex_count; j++) {
				cout << weight_matrix_copy[i * vertex_count + j] << " ";
			}
			cout << endl;
		}
		cout << "[ford]: parent:" << endl;
		for (int i = 0; i < vertex_count; i++) {
			cout << parent[i] << " ";
		}
		cout << endl;*/

		//текущий поток
		cout << "[ford]: path flow = " << path_flow << endl;
		max_flow += path_flow;
	}
	//полный поток
	cout << "[ford]: max flow = " << max_flow << endl;
	return;
}

void main()
{
	setlocale(LC_ALL, ".1251");
	get_weight_matrix((char*)"ћатрица¬есов.txt");
	printf("> ¬ведите команду:\n> 1 - ‘орд-‘аркенсон\n> 2 - ¬ыход\n> ");
	int command;
	scanf("%d", &command);
	if (command == 1) {
		ford_fulkerson();
	}
	else {
		return;
	}
	system("pause");
	return;
}