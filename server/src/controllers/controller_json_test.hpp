#pragma once
#include <drogon/HttpSimpleController.h>


class TestJsonController: public drogon::HttpSimpleController<TestJsonController>
{
public:
    virtual void asyncHandleHttpRequest(const drogon::HttpRequestPtr &req,
                                        std::function<void (const drogon::HttpResponsePtr &)> &&callback)override;
    PATH_LIST_BEGIN
    //list path definitions here;
    //PATH_ADD("/path","filter1","filter2",HttpMethod1,HttpMethod2...);
    PATH_ADD("/test_json", drogon::Get);
    PATH_LIST_END
};
