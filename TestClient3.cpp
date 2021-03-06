#include "client/ClientObserver.h"
#include "event/EventTest.h"
#include <string>
using namespace std;
class TestClient : public ClientObserver
{
public:
    TestClient(string name);
    ~TestClient();
    virtual void handle_event(EventTest *e);
};

TestClient::TestClient(string name):
    ClientObserver(name)
{

}

TestClient::~TestClient()
{

}

void TestClient::handle_event(EventTest *e)
{
    printf("%s handle event test:%s %s\n", client_name().c_str(), e->value1().c_str(), e->value2().c_str());
}

int main(int argc, char**argv)
{
    TestClient testCl3("testClient3");
    testCl3.subscribe_event(0,EVENT_ID_NULL);
    EventTest et("Hello", "vkang");
    testCl3.publish_event(&et);
    while(1)
    {
        testCl3.exec();
    }
}
