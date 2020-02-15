#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include "ServerDispatchManager.h"

ServerDispatchManager::ServerDispatchManager()
{
    m_ip = CONFIG_SERVER_IP;
    m_port = CONFIG_SERVER_PORT;
    server_init();

    for(int i = 0; i < EVENT_ID_MAX; i++)
    {
        m_event_hash[i] = new EventSubscibe;
        m_event_hash[i]->sub_list.clear();
    }
    m_users.clear();
}

ServerDispatchManager::ServerDispatchManager(int port, string ip)
{
    m_ip = ip;
    m_port = port;
    server_init();

    for(int i = 0; i < EVENT_ID_MAX; i++)
    {
        m_event_hash[i] = new EventSubscibe;
        m_event_hash[i]->sub_list.clear();
    }

    m_users.clear();
}

ServerDispatchManager::~ServerDispatchManager()
{
    for(int i = 0; i < EVENT_ID_MAX; i++)
    {
        delete m_event_hash[i];
    }
}

int ServerDispatchManager::server_init()
{
    int ret = 0;
    struct sockaddr_in server_addr;
    m_udp_fd = socket(AF_INET, SOCK_DGRAM, 0); //AF_INET:IPV4;SOCK_DGRAM:UDP
    if(m_udp_fd < 0)
    {
        printf("udp server create error:%d\n", m_udp_fd);
        return m_udp_fd;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, m_ip.c_str(), &server_addr.sin_addr.s_addr);
    server_addr.sin_port = htons(m_port);  //端口号，需要网络序转换
    ret = bind(m_udp_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(ret < 0)
    {
        printf("udp server bind error:%d\n", ret);
        return ret;
    }
    return ret;
}

int ServerDispatchManager::client_exist(struct sockaddr *cli_addr)
{
    int cl_exist = 0;
    list<Subscriber *>::iterator it = m_users.begin();
    struct sockaddr_in *cl_addr = (struct sockaddr_in*)cli_addr;
    for(; it != m_users.end(); it++)
    {
        struct sockaddr_in *user_addr = (struct sockaddr_in *)&((*it)->sub_addr);
        if(user_addr->sin_addr.s_addr == cl_addr->sin_addr.s_addr && user_addr->sin_port == cl_addr->sin_port)
        {
            cl_exist = 1;
            break;
        }
    }

    if(cl_exist == 1)
    {
        return RET_OK;
    }

    return RET_ERROR(RET_EXIST);
}

Subscriber *ServerDispatchManager::get_client(sockaddr *cli_addr)
{
    list<Subscriber *>::iterator it = m_users.begin();
    struct sockaddr_in *cl_addr = (struct sockaddr_in*)cli_addr;
    for(; it != m_users.end(); it++)
    {
        struct sockaddr_in *user_addr = (struct sockaddr_in *)&((*it)->sub_addr);
        if(user_addr->sin_addr.s_addr == cl_addr->sin_addr.s_addr && user_addr->sin_port == cl_addr->sin_port)
        {
            return *it;
        }
    }

    return NULL;
}

int ServerDispatchManager::register_client(string name, struct sockaddr *cli_addr)
{
    int cl_exist = client_exist(cli_addr);

    struct sockaddr_in *cl_addr = (struct sockaddr_in*)cli_addr;

    if(cl_exist < 0)
    {
        Subscriber *sub = new Subscriber;
        sub->sub_name = name;
        sub->sub_addr = *cli_addr;
        m_users.push_back(sub);
#if CONFIG_DISPATCH_MANAGER_DEBUG
        printf("Register client:%s %d %d\n", name.c_str(), cl_addr->sin_addr.s_addr, cl_addr->sin_port);
#endif
        return RET_OK;
    }

    return RET_ERROR(RET_EXIST);
}

int ServerDispatchManager::unregister_client(struct sockaddr *cli_addr)
{
    int cl_exist = 0;
    list<Subscriber *>::iterator it = m_users.begin();
    struct sockaddr_in *cl_addr = (struct sockaddr_in*)cli_addr;
    for(; it != m_users.end(); it++)
    {
        struct sockaddr_in *user_addr = (struct sockaddr_in *)&((*it)->sub_addr);
        if(user_addr->sin_addr.s_addr == cl_addr->sin_addr.s_addr)
        {
            m_users.erase(it);
#if CONFIG_DISPATCH_MANAGER_DEBUG
        printf("Unregister client:%s %d\n", (*it)->sub_name.c_str(), cl_addr->sin_addr.s_addr);
#endif
            delete *it;
            cl_exist = 1;
            break;
        }
    }

    if(cl_exist == 1)
    {
        return RET_OK;
    }

    return RET_ERROR(RET_EXIST);
}

int ServerDispatchManager::handle_message()
{
    char buf[CONFIG_TRANSFER_PACKAGE_SIZE] = { 0 };  //接收缓冲区，1024字节
    socklen_t len;
    int count;
    struct sockaddr_in client_addr;  //clent_addr用于记录发送方的地址信息

    memset(buf, 0, CONFIG_TRANSFER_PACKAGE_SIZE);
    len = sizeof(client_addr);
    count = recvfrom(m_udp_fd, buf, CONFIG_TRANSFER_PACKAGE_SIZE, 0, (struct sockaddr*)&client_addr, &len);
    if(count == -1)
    {
        printf("client udp recieve data fail!\n");
        return RET_ERROR(RET_INVALID);
    }

    struct sockaddr* cl_addr = (struct sockaddr*)&client_addr;

    ProtocolHead *head = (ProtocolHead *)buf;
    if(head->protocol_id == PROTOCOL_ID_CLIENT_ONLINE)
    {
        ProtocolClientOnline *online_pr = (ProtocolClientOnline *)(buf+sizeof(ProtocolHead));
        register_client(online_pr->cl_name, cl_addr);
    }
    else if(head->protocol_id == PROTOCOL_ID_CLIENT_OFFLINE)
    {
        unregister_client(cl_addr);
    }
    else // push the data to user buff
    {
        Subscriber *sub = get_client(cl_addr);
        if(sub)
        {
            handle_protocol(sub, head->protocol_id, buf+sizeof(ProtocolHead), count - sizeof(ProtocolHead));
#if 0
            for(int i = 0; i < count; i++)
            {
                sub->message_buf.push(buf[i]); // if there is an 10M data ???
                sub->message_len = sub->message_buf.size();
            }
#endif

        }
        else
        {
#if CONFIG_DISPATCH_MANAGER_DEBUG
            printf("An unregister client\n");
#endif
        }
    }

    return 0;
}

int ServerDispatchManager::handle_protocol_subscribe(Subscriber *sub, char *data, int data_len)
{
    if(data_len < sizeof(ProtocolSubscribe))
    {
#if CONFIG_DISPATCH_MANAGER_DEBUG
        printf("handle Subscribe protocol error:%d\n", data_len);
#endif
        return RET_ERROR(RET_INVALID);
    }

    ProtocolSubscribe * pro_sub = (ProtocolSubscribe *)data;

#if CONFIG_DISPATCH_MANAGER_DEBUG
    printf("Subscribe event:%d\n", pro_sub->event_id);
#endif
    if(pro_sub->event_id >= EVENT_ID_MAX)
    {
#if CONFIG_DISPATCH_MANAGER_DEBUG
        printf("Subscribe invalid event:%d\n", pro_sub->event_id);
#endif
        return RET_ERROR(RET_INVALID);
    }
    else
    {
        EventSubscibe *event_node = m_event_hash[pro_sub->event_id];
        event_node->sub_list.push_back(sub);
    }

    return RET_OK;
}

int ServerDispatchManager::handle_protocol_unsubscribe(Subscriber *sub, char *data, int data_len)
{
    if(data_len < sizeof(ProtocolSubscribe))
    {
#if CONFIG_DISPATCH_MANAGER_DEBUG
        printf("handle Subscribe protocol error:%d\n", data_len);
#endif
        return RET_ERROR(RET_INVALID);
    }

    ProtocolSubscribe * pro_sub = (ProtocolSubscribe *)data;

#if CONFIG_DISPATCH_MANAGER_DEBUG
    printf("Unsubscribe event:%d\n", pro_sub->event_id);
#endif
    if(pro_sub->event_id >= EVENT_ID_MAX)
    {
#if CONFIG_DISPATCH_MANAGER_DEBUG
        printf("Subscribe invalid event:%d\n", pro_sub->event_id);
#endif
        return RET_ERROR(RET_INVALID);
    }
    else
    {
        EventSubscibe *event_node = m_event_hash[pro_sub->event_id];

        list<Subscriber *>::iterator it = event_node->sub_list.begin();
        for(;it != event_node->sub_list.end();)
        {
            if(*it == sub)
            {
                it = event_node->sub_list.erase(it);
            }
            else
            {
                it++;
            }
        }
    }

    return RET_OK;
}

int ServerDispatchManager::handle_protocol_publish_event(Subscriber *sub, char *data, int data_len)
{
    if(data_len < sizeof(ProtocolPublishEvent))
    {
#if CONFIG_DISPATCH_MANAGER_DEBUG
        printf("handle publish event protocol error:%d\n", data_len);
#endif
        return RET_ERROR(RET_INVALID);
    }
    ProtocolPublishEvent *pub_e = (ProtocolPublishEvent *)data;
    int event_id = pub_e->event_id;

    if(event_id >= EVENT_ID_MAX)
    {
        return RET_ERROR(RET_INVALID);
    }

    EventSubscibe *event_node = m_event_hash[pub_e->event_id];
    list<Subscriber *>::iterator it = event_node->sub_list.begin();
     for(;it != event_node->sub_list.end();it++)
     {
         Subscriber *sub = *it;
         socklen_t len = (socklen_t)sizeof(struct sockaddr_in);
         sendto(m_udp_fd, (void*)data, data_len, 0, &sub->sub_addr, len);
     }

     return RET_OK;
}

int ServerDispatchManager::handle_protocol(Subscriber *sub, int protocol_id, char *data, int data_len)
{

#if CONFIG_DISPATCH_MANAGER_DEBUG
    printf("handle protocol:%d\n", protocol_id);
#endif
    switch (protocol_id) {
    case PROTOCOL_ID_SUBSCRIBE:
        handle_protocol_subscribe(sub, data, data_len);
        break;
    case PROTOCOL_ID_UNSUBSCRIBE:
        handle_protocol_unsubscribe(sub, data, data_len);
        break;
    case PROTOCOL_ID_PUBLISH_EVENT:
        handle_protocol_publish_event(sub, data, data_len);
        break;
    default:
        break;
    }
    return RET_OK;
}

/*
unusing function ......
*/
int ServerDispatchManager::handle_client_message()
{
    list<Subscriber *>::iterator it = m_users.begin();
    for(; it != m_users.end(); it++)
    {
        Subscriber *sub = *it;
        while(1)
        {
            if(sub->message_len)
            {
                unsigned int data_len = sub->message_buf.front();
                sub->message_buf.pop();
                unsigned int proto_id = sub->message_buf.front();
                sub->message_buf.pop();
                if(sub->message_buf.size() >= data_len)
                {
                    char *message = (char*)malloc(data_len);
                    for(unsigned int i = 0; i< data_len; i++)
                    {
                        message[i] = sub->message_buf.front();
                        sub->message_buf.pop();
                        sub->message_len = sub->message_buf.size();
                    }
                    handle_protocol(sub, proto_id, message, data_len);
                    free(message);
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    return RET_OK;
}

int ServerDispatchManager::exec()
{
    handle_message();
}

