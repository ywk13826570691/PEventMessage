#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "Common.h"
#include "ProtocolId.h"
#include "server/ServerDispatchManager.h"
#include "client/ClientObserver.h"
#include "event/EventTest.h"

using namespace std;

#if 0
pthread_t server_pt;
pthread_t cl1_pt;
pthread_t cl2_pt;


void *server_thread(void *arg)
{
    ServerDispatchManager sr;
    while(1)
    {
        sr.exec();
    }
    return NULL;
}


void *cl1_thread(void *arg)
{
    ClientObserver cl("cli1");
    cl.subscribe_event(0,1,EVENT_ID_NULL);
    while(1)
    {
        cl.exec();
    }
    return NULL;
}

void *cl2_thread(void *arg)
{
    ClientObserver cl("cli2");
    cl.subscribe_event(0,1,EVENT_ID_NULL);
    cl.unsubscribe_event(0,EVENT_ID_NULL);
    EventTest et("Hello", "vkang");
    sleep(1);
    cl.publish_event(&et);

    while(1)
    {
        cl.exec();
    }
    return NULL;
}
#endif
int main()
{
#if 0
    pthread_create(&server_pt, NULL, server_thread, NULL);
    pthread_detach(server_pt);
    sleep(1);
    pthread_create(&cl1_pt, NULL, cl1_thread, NULL);
    pthread_create(&cl2_pt, NULL, cl2_thread, NULL);

    pthread_detach(cl1_pt);
    pthread_detach(cl2_pt);
    cout << "Hello World!" <<endl;
#endif
    ServerDispatchManager server;
    while(1)
    {
        server.exec();
    }
    return 0;
}

