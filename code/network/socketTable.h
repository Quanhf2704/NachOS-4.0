#include "sysdep.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define MAX_SOCKETS 20

class SocketTable
{
    private:
        bool* _inUse;
        int*  _socketID;
    public:
        SocketTable()
        {
            _inUse = new bool[MAX_SOCKETS];
            _socketID = new int[MAX_SOCKETS];
        }
        ~SocketTable()
        {
            for(int i=0;i<MAX_SOCKETS;i++)
            {
                if(_inUse[i] != NULL)
                {
                    _inUse[i] = false;
                    _socketID[i] = NULL;
                }
            }
            delete[] _inUse;
            delete[] _socketID;
        }
        int SocketTCP()
        {
            int freeIndex = -1;
            
            for(int i=2;i<MAX_SOCKETS;i++)
            {
                if(_inUse[i] == 0)
                {
                    freeIndex = i;
                    break;
                }
            }
            if(freeIndex == -1)
            {
                return -1;
            }
            else
            {
                _inUse[freeIndex] = 1;
                _socketID[freeIndex] = socket(AF_INET, SOCK_STREAM, 0);
            }
            return _socketID[freeIndex];

        }
        int Close(int socketID)
        {
            close(socketID);
            for(int i=2;i<MAX_SOCKETS;i++)
            {
                if(_inUse[i]==1 && _socketID[i]==socketID)
                {
                    _inUse[i] = 0;
                    _socketID[i] = NULL;
                    return 1;
                }
            }

            return 0;
        }
        bool getInUse(int socketID)
        {
            for(int i=2;i<MAX_SOCKETS;i++)
            {
                if(_socketID[i]==socketID)
                {
                    return 1;
                }
            }
            return 0;
        }
};