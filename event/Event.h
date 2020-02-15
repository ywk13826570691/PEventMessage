#ifndef EVENT_H
#define EVENT_H
#include <string>
#include "EventHandle.h"
using namespace std;
#define EVENT_OBJECT(theClass) \
    int event_handler(EventHandle *e_handler) \
    {\
        e_handler->handle_event(this);\
        return 0; \
    }


class Event
{
public:
    Event(int id, string name);
    virtual ~Event();
    int event_id();

    virtual int event_handler(EventHandle *e_handler);
    virtual int serialize_to_buffer(char *buf);
    virtual int serialize_from_buffer(char *buf);
    virtual int serialize_buffer_len();
    virtual int event_clear();
private:
    int m_id;
    string m_name;
};

#endif // EVENT_H
