#include"ProcessPool.hpp"
#include"Task.hpp"

int main()
{
    ProcessPool pp(5);
    pp.Start();
    int cnt=10;
    while (cnt--)
    {
        pp.Run();
    }
    pp.Stop();
    return 0;
}