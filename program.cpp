/***************************************************************************
* File: program.cpp
* Author: Rigre R. Garciandia
* Procedures:
* main - Uses a fork and a pipe to make a client and a server process. The client sends in 
*    data for a square, and the server checks whether it's a square and returns the data
* printMachineInfo - gets and prints system information of the machine running the program
* printSquare - takes in a 3x3 2d array reprenting a matrix of numbers, and prints it out in square format
* isMagicSquare - returns true if the square represented by the string is a magic square, false otherwise
* clientMain - main method for the client process. It makes the call to the server with a 
*   static magic square string. It then waits for the result and prints it out.
* serverMain - main method for the server process. It waits for a string to be put through the
*   pipe, then processes it to check whether it's a magic square and retuns the result through
*   the pipe
***************************************************************************/

#include <iostream>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <string.h>

/***************************************************************************
* void printMachineInfo()
* Author: Rigre R. Garciandia
* Date: 17 September 2021
* Description: gets and prints system information of the machine running the program
**************************************************************************/
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

/***************************************************************************
* void printSquare(int nums[][3])
* Author: Rigre R. Garciandia
* Date: 17 September 2021
* Description: takes in a 3x3 2d array reprenting a matrix of numbers, and prints it out in square format
*
* Parameters:
* nums I/P int[][3] The 2d matrix to be printed out
**************************************************************************/
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

/***************************************************************************
* bool isMagicSquare(char *str)
* Author: Rigre R. Garciandia
* Date: 17 September 2021
* Description: returns true if the square represented by the string is a magic square, false otherwise
*
* Parameters:
* str I/P char* The string to be analyzed
**************************************************************************/
bool isMagicSquare(char *str)
{
    int nums[3][3];
    for (int i = 0; i < 9; i++)
    {
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

/***************************************************************************
* void clientMain(int readingPipe, int writingPipe)
* Author: Rigre R. Garciandia
* Date: 17 September 2021
* Description: main method for the client process. It makes the call to the server with a 
*   static magic square string. It then waits for the result and prints it out.
*
* Parameters:
* readingPipe I/P int File descriptor of reading pipe for the client
* writingPipe I/P int File descriptor of writing pipe for the client
**************************************************************************/
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

/***************************************************************************
* void serverMain(int readingPipe, int writingPipe)
* Author: Rigre R. Garciandia
* Date: 17 September 2021
* Description: main method for the server process. It waits for a string to be put through the
*   pipe, then processes it to check whether it's a magic square and retuns the result through
*   the pipe
*
* Parameters:
* readingPipe I/P int File descriptor of reading pipe for the server
* writingPipe I/P int File descriptor of writing pipe for the server
**************************************************************************/
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

/***************************************************************************
* int main ()
* Author: Rigre R. Garciandia
* Date: 17 September 2021
* Description: Uses a fork and a pipe to make a client and a server process. The client sends in 
*    data for a square, and the server checks whether it's a square and returns the data
**************************************************************************/
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