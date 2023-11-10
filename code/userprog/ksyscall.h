/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include "synchconsole.h"
#include "syscall.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXLENGTH 1024

char *User2System(int virtAddr, int limit) 
{
    int i; 
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1];

    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit + 1);

    for (i = 0; i < limit; i++) {
        kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

int System2User(int virtAddr, char *buffer, int length) 
{
    if (length <= 0)
        return length;
    int i = 0;
    int oneChar = 0;
    do 
    {
        oneChar = (int)buffer[i];
        kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < length && oneChar != 0);

    return i;
}

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

void SysReadString(int length) 
{
    char *buffer = new char[length + 1];
    if (buffer == NULL) 
    {
        return;
    }

    int i = 0;
    char tmp;

    while (i < length) 
    {
        tmp = kernel->synchConsoleIn->GetChar();
        if (tmp == '\0' || tmp == '\n') 
        {
            buffer[i] = '\0';
            break;
        }
        buffer[i++] = tmp;
    }

    // Overflow control
    if (tmp != '\0' && tmp != '\n') 
    {
        while (true) 
        {
            tmp = kernel->synchConsoleIn->GetChar();
            if (tmp == '\0' || tmp == '\n')
                break;
        }
    }

    int virtAddr = kernel->machine->ReadRegister(4);
    System2User(virtAddr, buffer, length);
    delete[] buffer;
}

void SysPrintString() 
{
    int i = 0;
    int virtAddr = kernel->machine->ReadRegister(4);

    //Read string input from User
    char *buffer = User2System(virtAddr, MAXLENGTH);

    //Write to console word by word until the end of string
    while (buffer[i] != '\0') 
    {
        kernel->synchConsoleOut->PutChar(buffer[i++]);
    }
    delete[] buffer;
}

void SysPrintNum() 
{
    bool isNegative = false;
    int number = kernel->machine->ReadRegister(4);
    int i = 0;
    char *buffer = new char[12 + 1];

    if (number == -2147483648 - 1) 
    {
        strcpy(buffer, "-2147483648");
        buffer[strlen(buffer)] = '\0';
        i = 0;
        while (buffer[i] != '\0') 
        {
            kernel->synchConsoleOut->PutChar(buffer[i++]);
        }
    }
    else 
    {
        if (number != 0) 
        {
            if (number < 0) 
            {
                isNegative = true;
                number = -number;
            }
            int num = 0;
            while (number != 0) 
            {
                num = number % 10;
                buffer[i] = num + '0';
                number = number / 10;
                i++;
            }
            if (isNegative) 
            {
                buffer[i] = '-';
                i++;
            }
        }
        else 
        {
            buffer[i] = '0';
            i++;
        }
        buffer[i] = '\0';
        while (i >= 0) {
            kernel->synchConsoleOut->PutChar(buffer[i--]);
        }
    }
    delete[] buffer;
}

int SysSocketTCP()
{
    return kernel->skTable->SocketTCP();
}

int SysConnect(int socketID, char* ip,int port)
{
    if(socketID <0 || socketID >=MAX_SOCKETS || kernel->skTable->getInUse(socketID) == 0)
    {
        return -1;
    }
    struct sockaddr_in serverAddr;
    memset(&serverAddr,0,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    serverAddr.sin_port = htons(port);

    int result = connect(socketID, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    return result;
}

int SysSend(int socketID, char* buffer, int len)
{
    int result = -1;

    int n = send(socketID, buffer, len +1, 0);
    if (n > 0) result = n;  

    return result; 
}

int SysRecieve(int socketID, char* buffer, int len, int virtAddr)
{
    int result = -1;
    int n = read(socketID, buffer, len);
    if (n >= 0) 
    {   
        result = n;
    }    
    //cerr << "Buffer read from socket: "<< buffer <<"\n";
    System2User(virtAddr,buffer,len);
    return result;
}

void SysCloseSock()
{
    int socketID = kernel->machine->ReadRegister(4);

    if(socketID < 0)
    {
        kernel->machine->WriteRegister(2,-1);
        return;
    }
    
    int res = kernel->skTable->Close(socketID);
    if(res == 0)
    {
        kernel->machine->WriteRegister(2,-1);
        return;
    }
    kernel->machine->WriteRegister(2,1);
    return;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
