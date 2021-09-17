#include <iostream>
#include <unistd.h>
#include <sys/utsname.h>

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
    std::cout << "Domain name:" << domainName << std::endl;
    std::cout << "Host name:" << hostName << std::endl;
    std::cout << "OS: " << unameData.sysname << std::endl;
    std::cout << "nodename: " << unameData.nodename << std::endl;
    std::cout << "release: " << unameData.release << std::endl;
    std::cout << "version: " << unameData.version << std::endl;
    std::cout << "machine: " << unameData.machine << std::endl;
}

int main()
{
   printMachineInfo();
}