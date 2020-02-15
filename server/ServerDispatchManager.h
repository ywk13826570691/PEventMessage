#ifndef SERVER_DISPATCH_MANAGER_H
#define SERVER_DISPATCH_MANAGER_H

#include <string>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <queue>
#include <list>
#include "ProtocolId.h"
#include "Config.h"
#include "EventId.h"

using namespace std;

//we using udp

#define CONFIG_DISPATCH_MANAGER_DEBUG 1


typedef struct Subscriber
{
    struct sockaddr sub_addr;
    string sub_name;
    queue<char> message_buf;
    int message_len;
}Subscriber;

typedef struct EventSubscibe
{
    list<Subscriber *> sub_list;
}EventSubscibe;

#define RET_ERROR(ret) (-(ret))
class ServerDispatchManager
{
public:
    ServerDispatchManager();
    ServerDispatchManager(int port, string ip);
    ~ServerDispatchManager();

    typedef enum SERVER_RETURN
    {
        RET_OK,
        RET_INVALID,
        RET_EXIST,
    }SERVER_RETURN;
public:
    int exec();
private:
    int server_init();
    int handle_message();

    int handle_client_message();
    int handle_protocol(Subscriber *sub, int protocol_id, char *data, int data_len);
    int handle_protocol_subscribe(Subscriber *sub, char *data, int data_len);
    int handle_protocol_unsubscribe(Subscriber *sub, char *data, int data_len);
    int handle_protocol_publish_event(Subscriber *sub, char *data, int data_len);

    int client_exist(struct sockaddr *cli_addr);
    Subscriber *get_client(struct sockaddr *cli_addr);
    int register_client(string name, sockaddr *cli_addr);
    int unregister_client(struct sockaddr *cli_addr);
private:

    EventSubscibe* m_event_hash[EVENT_ID_MAX];
    list<Subscriber *> m_users;
    int m_udp_fd;
    string m_ip;
    int m_port;
};

#endif // SERVER_H
