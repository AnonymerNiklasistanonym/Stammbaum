#include "controller_json_test.hpp"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>


std::string generateJsonString(const rapidjson::Document &jsonDoc)
{
    // Generate string
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    jsonDoc.Accept(writer);
    // Output generated string
    return std::string(strbuf.GetString());
}


void TestJsonController::asyncHandleHttpRequest(const drogon::HttpRequestPtr &req,
                                                std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{
    // Create rapidjson document
    rapidjson::Document jsonDoc;
    // Set rapidjson::document to a JSON object
    jsonDoc.SetObject();
    // Get the allocator of this rapidjson::document
    auto &alloc = jsonDoc.GetAllocator();
    // Add values to the rapidjson::document
    jsonDoc.AddMember(rapidjson::GenericStringRef<char>("test"),
                      "Hello World :)", alloc);
    const auto jsonString = generateJsonString(jsonDoc);

    std::cout << "body: " << req->body() << std::endl;
    //write your application logic here
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    resp->setBody(jsonString);
    callback(resp);
}
