#include "syscall.h"

int main()
{
    int socketID,i;
    PrintString("Create Socket:");
    for (i=0;i<10;i++)
    {
        socketID =  SocketTCP();
        if (socketID == -1)
        {
            PrintString("Cannot create socket...");
        }
        else
        {
            PrintString("Socket ID : ");
            PrintNum(socketID);
            PrintString("\n");
        }
    }
    PrintString("Closing Socket......");
    for (i=8;i<17;i++)
    {
        PrintString("Close Socket ID : ");
        PrintNum(i);
        PrintString("\n");
        if(Close(i) == -1)
        {
            PrintString("Cannot close socket...");
        }
        else
        {
            PrintString("Socket ID : ");
            PrintNum(i);
            PrintString(" closed successfully!");
            PrintString("\n");
        }
    }
    Halt();
    /* not reached */
}
