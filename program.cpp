//TODO: add file header
//TODO: add function headers


#include <iostream>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <string.h>

void printMachineInfo()
{
    //gather data
    char *domainName = new char[256];
    getdomainname(domainName, sizeof(domainName));

    char *hostName = new char[256];
    getdomainname(hostName, sizeof(hostName));

    struct utsname unameData;
    uname(&unameData);

    //print data
    std::cout << "--------------------------------------------------------------------" << std::endl;
    std::cout << "Domain name:" << domainName << std::endl;
    std::cout << "Host name:" << hostName << std::endl;
    std::cout << "OS: " << unameData.sysname << std::endl;
    std::cout << "nodename: " << unameData.nodename << std::endl;
    std::cout << "release: " << unameData.release << std::endl;
    std::cout << "version: " << unameData.version << std::endl;
    std::cout << "machine: " << unameData.machine << std::endl;
    std::cout << "--------------------------------------------------------------------" << std::endl;
}

void printSquare(int nums[][3])
{
    for (int r = 0; r < 3; r++) 
    {
        for (int c = 0; c < 3; c++)
        {
            std::cout << nums[r][c] << " ";
        }
        std::cout << std::endl;
    }
}

bool isMagicSquare(char *str)
{
    int nums[3][3];
    for (int i = 0; i < 9; i++) {
        nums[i / 3][i % 3] = str[i] - '0'; //get the int value of an ascii character
    }
    printSquare(nums);

    /*
    To verify that it's a magic square, we will take the sum of the first row
    Then we will make sure the sum of every other row, column, and diagonal is equal to that
    Returning false if any of those checks fail
    */
    int firstRowSum = nums[0][0] + nums[0][1] + nums[0][2];

    //checking the remaining rows
    for (int r = 1; r < 3; r++) 
    {
        int rowSum = nums[r][0] + nums[r][1] + nums[r][2];
        if (rowSum != firstRowSum)
            return false;
    }
    //checking columns
    for (int c = 0; c < 3; c++)
    {
        int colSum = nums[0][c] + nums[1][c] + nums[2][c];
        if (colSum != firstRowSum)
            return false;
    }
    //checking first diagonal
    int firstDiagonalSum = nums[0][0] + nums[1][1] + nums[2][2];
    if (firstDiagonalSum != firstRowSum)
        return false;

    //checking second diagonal
    int secondDiagonalSum = nums[0][2] + nums[1][1] + nums[2][0];
    if (secondDiagonalSum != firstRowSum)
        return false;

    //at this point, we have checked every row, column, and diagonal, and can confirm that we have a magic square
    return true;
}

void clientMain(int readingPipe, int writingPipe)
{
    char str[10] = "276951438";
    std::cout << "Sent string: " << str << std::endl;
    write(writingPipe, str, 9 * sizeof(char)); 
    
    bool result;
    read(readingPipe, &result, sizeof(bool));
    std::cout << "Received result: " << result << std::endl;

    close(readingPipe);
    close(writingPipe);
}

void serverMain(int readingPipe, int writingPipe)
{
    char str[9];
    read(readingPipe, str, 9 * sizeof(char));
    std::cout << "Received string: " << str << std::endl;

    bool result = isMagicSquare(str);
    std::cout << "Sent result: " << result << std::endl;
    write(writingPipe, &result, sizeof(bool));

    close(readingPipe);
    close(writingPipe);
}

int main()
{
    printMachineInfo();

    //setup pipes
    int clientToServerPipe[2];
    int serverToClientPipe[2];

    if (pipe(clientToServerPipe) == -1)
    {
        return 1;
    }
    if (pipe(serverToClientPipe) == -1)
    {
        return 1;
    }

    //perform forking
    int pid = fork();
    if (pid == -1) //fork failed
    {
        return 2;
    }
    if (pid == 0) //we are in the child process
    {
        clientMain(serverToClientPipe[0], clientToServerPipe[1]);
    }
    else //we are in the parent process
    {
        serverMain(clientToServerPipe[0], serverToClientPipe[1]);
        wait(NULL);
    }
}