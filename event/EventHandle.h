#ifndef EVENTHANDLE_H
#define EVENTHANDLE_H
#include "EventId.h"

class EventTest;

class EventHandle
{
public:
    EventHandle();
    virtual ~EventHandle();
    int event_handler(int event_id, char *event_data);
    virtual void handle_event(EventTest *e){ e = e;}
};

#endif // EVENTHANDLE_H
