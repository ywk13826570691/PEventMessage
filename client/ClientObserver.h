#ifndef CLIENTOBSERVER_H
#define CLIENTOBSERVER_H
#include <string>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <queue>
#include "event/Event.h"
#include "event/EventTest.h"
#include "event/EventHandle.h"

using namespace std;


class ClientObserver : public EventHandle
{
public:
    ClientObserver(string name);
    ClientObserver(string name, string ip, int port);
    ~ClientObserver();
public:
    string client_name();
    int exec();
    int subscribe_event(int event_id , ...); //EVENT_ID_NULL must last
    int unsubscribe_event(int event_id, ...);//EVENT_ID_NULL must last
    int publish_event(Event *e);

private:
    int init_client();
    int client_login();
    int send_buffer(char *buf, int len);
    int handle_message();
    int handle_event_protocol(char *buf, int len);

private:
    queue<char> m_message_buf;
    struct sockaddr_in m_server_addr;
    string m_name;
    string m_ip;
    int m_port;
    int m_udp_fd;
};

#endif // CLIENTOBSERVER_H
