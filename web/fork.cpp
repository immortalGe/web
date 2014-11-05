#include <unistd.h>
#include <iostream>
#include <sys/types.h>
using namespace std;

static int a = 10;

int main()
{
    pid_t pid = fork();
    if (pid == 0)
    {
        a = 20;
        cout << a;
    }
    else
    {
        sleep(2);
        cout << "parent" << a;
    }
}
