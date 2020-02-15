#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <stdlib.h>

#include "ClientObserver.h"
#include "Config.h"
#include "ProtocolId.h"
#include "EventId.h"

ClientObserver::ClientObserver(string name)
{
    m_name = name;
    m_port = CONFIG_SERVER_PORT;
    m_ip = CONFIG_SERVER_IP;
    init_client();
}

ClientObserver::ClientObserver(string name, string ip, int port)
{
    m_name = name;
    m_port = port;
    m_ip = ip;
    init_client();
}

ClientObserver::~ClientObserver()
{

}

int ClientObserver::init_client()
{
    m_udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&m_server_addr, 0, sizeof(m_server_addr));
    m_server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, m_ip.c_str(), &m_server_addr.sin_addr.s_addr);
    m_server_addr.sin_port = htons(m_port);

    client_login();

    return m_udp_fd;
}

string ClientObserver::client_name()
{
    return m_name;
}

int ClientObserver::client_login()
{
    char buf[1024] = {0};
    int len = 0;
    ProtocolHead head;
    head.protocol_id = PROTOCOL_ID_CLIENT_ONLINE;
    head.pkg_len = sizeof(ProtocolClientOnline);

    ProtocolClientOnline cl_online;
    strcpy(cl_online.cl_name, m_name.c_str());
    memcpy(buf, &head, sizeof(head));
    memcpy(buf+sizeof(head), &cl_online, sizeof(ProtocolClientOnline));
    len = sizeof(ProtocolHead) + sizeof(ProtocolClientOnline);

    return send_buffer(buf, len);
}

int ClientObserver::handle_message()
{
    int cnt = 0;
    socklen_t len;
    struct sockaddr_in src;
    len = sizeof(struct sockaddr_in);
    char buf[CONFIG_TRANSFER_PACKAGE_SIZE];
    memset(buf, 0, CONFIG_TRANSFER_PACKAGE_SIZE);
    cnt = recvfrom(m_udp_fd, buf, CONFIG_TRANSFER_PACKAGE_SIZE, 0, (struct sockaddr*)&src, &len);
    if(cnt > 0)
    {
        handle_event_protocol(buf, cnt);
#if 0
        for(int i = 0; i < cnt; i++)
        {
            m_message_buf.push(buf[i]);
        }
#endif
    }
    return cnt;
}

int ClientObserver::handle_event_protocol(char *buf, int len)
{
    ProtocolPublishEvent *pub_e = (ProtocolPublishEvent *)buf;
    unsigned int event_id = pub_e->event_id;
    unsigned int event_data_len = pub_e->event_data_len;
    if(event_data_len != len-sizeof(ProtocolPublishEvent))
    {
        printf("Error Event Frame:%d\n", event_id);
    }
    printf("Rcv Event:%d %d\n", event_id, len);
    event_handler(event_id, buf+sizeof(ProtocolPublishEvent));
    return 0;
}

int ClientObserver::exec()
{
    return handle_message();
}

/*subscribe functions*/
int ClientObserver::ClientObserver::subscribe_event(int event_id , ...)
{
    va_list arg_ptr;
    int arg = event_id;
    va_start(arg_ptr,event_id);
    while(arg != EVENT_ID_NULL)
    {
        if(arg >= 0 && arg < EVENT_ID_MAX)
        {
            ProtocolHead head;
            head.protocol_id = PROTOCOL_ID_SUBSCRIBE;
            head.pkg_len = sizeof(ProtocolSubscribe);
            ProtocolSubscribe subpkg;
            subpkg.event_id = arg;
            char buf[500] = { 0 };
            memcpy(buf, &head, sizeof(head));
            memcpy(buf+sizeof(head), &subpkg, sizeof(subpkg));

            send_buffer(buf, sizeof(head) + sizeof(subpkg));
        }
        arg = va_arg(arg_ptr,int);

    }
    return 0;
}

int ClientObserver::unsubscribe_event(int event_id, ...)
{
    va_list arg_ptr;
    int arg = event_id;
    va_start(arg_ptr,event_id);
    while(arg != EVENT_ID_NULL)
    {
        if(arg < EVENT_ID_MAX && arg >= 0)
        {
            ProtocolHead head;
            head.protocol_id = PROTOCOL_ID_UNSUBSCRIBE;
            head.pkg_len = sizeof(ProtocolUnsubscribe);
            ProtocolUnsubscribe unsubpkg;
            unsubpkg.event_id = arg;
            char buf[500] = { 0 };
            memcpy(buf, &head, sizeof(head));
            memcpy(buf+sizeof(head), &unsubpkg, sizeof(unsubpkg));

            send_buffer(buf, sizeof(head) + sizeof(unsubpkg));
        }
        arg = va_arg(arg_ptr,int);
    }
    return 0;
}

int ClientObserver::publish_event(Event *e)
{
    int ret = 0;
    int event_data_len = e->serialize_buffer_len();
    int res_len = 0;
    ProtocolHead head;
    head.protocol_id = PROTOCOL_ID_PUBLISH_EVENT;
    head.pkg_len = event_data_len + sizeof(ProtocolPublishEvent);

    ProtocolPublishEvent pub_e;
    pub_e.event_id = e->event_id();
    pub_e.event_data_len = event_data_len;

    char *buf = (char*)malloc(event_data_len);
    res_len = e->serialize_to_buffer(buf);

    if(event_data_len != res_len)
    {
        printf("Why serialize_to_buffer len not same to serialize_buffer_len\n");
        return -1;
    }
    int send_len = sizeof(ProtocolHead) + sizeof(ProtocolPublishEvent) + event_data_len;
    char *buf_send = (char*)malloc(send_len);
    memcpy(buf_send, &head, sizeof(head));
    memcpy(buf_send + sizeof(head), &pub_e, sizeof(pub_e));
#if 0
    /*I don't know why this is invalid using memcpy*/
    memcpy(buf_send + sizeof(head) + sizeof(pub_e), (void *)buf, sizeof(event_data_len));
#else
    char *p_send = buf_send + sizeof(head) + sizeof(pub_e);
    for(int i = 0; i < event_data_len; i++)
    {
        p_send[i] = buf[i];
    }
#endif
    ret = send_buffer(buf_send, send_len);
    free(buf);
    free(buf_send);
    return ret;
}

int ClientObserver::send_buffer(char *buf, int len)
{
    return sendto(m_udp_fd, buf, len, 0, (struct sockaddr*)&m_server_addr, sizeof(m_server_addr));
}

