#include "controller_test.hpp"

void TestController::asyncHandleHttpRequest(const drogon::HttpRequestPtr &req,
                                            std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{
    std::cout << "body: " << req->body() << std::endl;
    //write your application logic here
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    resp->setContentTypeCode(drogon::CT_TEXT_HTML);
    resp->setBody("Hello World!!!!!!!!!");
    callback(resp);
}
