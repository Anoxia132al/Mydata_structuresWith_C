#include <iostream>

using std::cout;
using std::endl;

constexpr int MAXSIZE = 100;
constexpr int MAX = 0x7fffffff;

struct Mat_Grph
{
    std::string vertex;
    int arc[MAXSIZE][MAXSIZE];
    int vertex_num, edge_num;
};

void creat_graph (Mat_Grph* G)
{
    G->vertex_num = 9;
    G->edge_num = 15;

    std::string str = "ABCDEFGHI";
    for (auto x : str)  G->vertex.push_back(x);

    for (int i = 0; i != G->vertex_num; ++i)
    {
        for (int j = 0; j != G->vertex_num; ++j)
        {
            if (i == j)  G->arc[i][j] = 0;
            else G->arc[i][j] = MAX;
        }
    }
    //  A-B A-F
    G->arc[0][1] = 10;
    G->arc[0][5] = 11;
    //  B-C B-G B-I
    G->arc[1][2] = 18;
    G->arc[1][6] = 16;
    G->arc[1][8] = 12;
    //  C-D C-I
    G->arc[2][3] = 22;
    G->arc[2][8] = 8;
    //  D-E D-G D-H D-I
    G->arc[3][4] = 20;
    G->arc[3][6] = 24;
    G->arc[3][7] = 16;
    G->arc[3][8] = 21;
    //  E-F E-H
    G->arc[4][5] = 26;
    G->arc[4][7] = 7;
    //  F-G
    G->arc[5][6] = 17;
    //  G-H
    G->arc[6][7] = 19;

    for (int i = 0; i != G->vertex_num; ++i)
        for (int j = 0; j != G->vertex_num; ++j)  
            G->arc[j][i] = G->arc[i][j];
}

//  Prim 算法
void prim (Mat_Grph* G)
{
    int weight[MAXSIZE];  //  侯选边，选择权值最小的
    int vex_index[MAXSIZE];  //  值表示出发点，下标表示终点，表示两点连线

    //  从 A 开始，依次选择权值最小的边，直到所有点都被访问
    for (int i = 0; i != G->vertex_num; ++i)
    {
        weight[i] = G->arc[0][i];
        vex_index[i] = 0;
    }
    for (int i = 1; i != G->vertex_num; ++i)
    {
        int j = 0, k = 0;
        int min = MAX;
        while (j < G->vertex_num)
        {
            if (weight[j] != 0 && weight[j] < min)
            {
                min = weight[j];
                k = j;  //  k 表示权值最小的边的终点
            }
            j++;
        }
        cout << "[" << G->vertex[vex_index[k]] << ", " << G->vertex[k] << "] = " << weight[k] << endl;
        weight[k] = 0;  //  将权值最小的边的终点标记为已访问

        for (j = 0; j != G->vertex_num; ++j)
        {
            if (weight[j] != 0 && G->arc[k][j] < weight[j])
            {
                weight[j] = G->arc[k][j];
                vex_index[j] = k;
            }
        }
    }
}

int main ()
{
    Mat_Grph G;
    creat_graph (&G);
    prim (&G);
    return 0;
}