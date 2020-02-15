#ifndef PROTOCOLID_H
#define PROTOCOLID_H

#pragma pack(1)

typedef enum PROTOCOL_ID
{
    PROTOCOL_ID_CLIENT_ONLINE,
    PROTOCOL_ID_CLIENT_OFFLINE,
    PROTOCOL_ID_SUBSCRIBE,
    PROTOCOL_ID_UNSUBSCRIBE,
    PROTOCOL_ID_PUBLISH_EVENT,
}PROTOCOL_ID;

typedef struct FrameBegin
{

}FrameBegin;

typedef struct ProtocolHead
{
    int pkg_len;
    int protocol_id;
}ProtocolHead;

typedef struct ProtocolClientOnline
{
    char cl_name[50];
}ProtocolClientOnline;

typedef struct ProtocolClientOffline
{
    char cl_name[50];
}ProtocolClientOffline;

typedef struct ProtocolSubscribe
{
    int event_id;
}ProtocolSubscribe;

typedef struct ProtocolUnsubscribe
{
    int event_id;
}ProtocolUnsubscribe;


typedef struct ProtocolPublishEvent
{
    int event_data_len;
    int event_id;
}ProtocolPublishEvent;

#pragma pack()

#endif // PROTOCOLID_H

