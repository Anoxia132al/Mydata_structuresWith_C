#include <iostream>
#include <vector>

//  Boyer-Moore majority vote algorithm
void majorityElement (std::vector<int>& num)
{
    if (num.empty())
    {
        std::cout << "no input" << std::endl;
        return;
    }

    int candidate = 0, count = 0;
    for (int x : num)
    {
        if (count == 0) candidate = x;
        if (x == candidate) count++;
        else count--;   //  一定有过半数时，count 不会递减至 0，
                        //  因此不会再次执行 candidate = x，输出过半数
    }
    if (count == 0)
    {
        std::cout << "half appearance but failed" << std::endl;
        return;
    } 
    else
    {
        int cnt = 0;
        for (int val : num)
        {
            if (val == candidate)   cnt++;
        }
        if (cnt > static_cast<int>(num.size()) / 2)
            std::cout << candidate << std::endl;
        else 
        {
            std::cout << "failed" << std::endl;
            return;
        }
    }
}

int main ()
{
    std::vector<int> num;
    int x;
    while (std::cin >> x)   num.push_back(x);
    majorityElement (num);
    return 0;
}