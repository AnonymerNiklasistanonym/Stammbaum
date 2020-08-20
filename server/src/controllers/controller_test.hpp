#pragma once
#include <drogon/HttpSimpleController.h>


class TestController: public drogon::HttpSimpleController<TestController>
{
public:
    virtual void asyncHandleHttpRequest(const drogon::HttpRequestPtr &req,
                                        std::function<void (const drogon::HttpResponsePtr &)> &&callback)override;
    PATH_LIST_BEGIN
    //list path definitions here;
    //PATH_ADD("/path","filter1","filter2",HttpMethod1,HttpMethod2...);
    PATH_ADD("/", drogon::Get, drogon::Post);
    PATH_ADD("/test", drogon::Get);
    PATH_LIST_END
};
