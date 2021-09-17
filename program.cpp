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

bool isMagicSquare(char *str){
    return true; //TODO
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