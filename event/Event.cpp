#include "Event.h"
#include <string.h>
#include <stdlib.h>
#include "Common.h"

typedef struct EventBaseBuffer
{
    int id;
    char name[125];
}__ALIGN__ EventBaseBuffer;

Event::Event(int id, string name)
{
    m_id = id;
    m_name = name;
}

Event::~Event()
{

}

int Event::event_id()
{
    return m_id;
}

int Event::serialize_to_buffer(char *buf)
{
    int len = 0;
    EventBaseBuffer base_buf;
    base_buf.id = m_id;
    strcpy(base_buf.name, m_name.c_str());
    memcpy(buf, &base_buf, sizeof(EventBaseBuffer));
    len+=sizeof(EventBaseBuffer);
    return len;
}

int Event::serialize_from_buffer(char *buf)
{
    int len = 0;
    EventBaseBuffer *base_buf = (EventBaseBuffer *)buf;
    memcpy(&m_id, buf, sizeof(m_id));
    m_id = base_buf->id;
    m_name = string(base_buf->name);
    len+=sizeof(EventBaseBuffer);
    return len;
}

int Event::serialize_buffer_len()
{
    int len = 0;
    len = sizeof(EventBaseBuffer);
    return len;
}

int Event::event_clear()
{
    return 0;
}

int Event::event_handler(EventHandle *e_handler)
{
    e_handler = e_handler;
    return 0;
}

