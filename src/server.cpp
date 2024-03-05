#include "httplib.h"
#include "server.h"

int32_t Servertest()
{
    httplib::Server server;

    // 处理 GET 请求
    server.Get("/hello", [](const httplib::Request &req, httplib::Response &res)
               { res.set_content("Hello, World!", "text/plain"); });

    // 处理 POST 请求
    server.Post("/echo", [](const httplib::Request &req, httplib::Response &res)
                { res.set_content(req.body, "text/plain"); });

    // 启动服务器，监听端口8080
    if (server.listen("172.20.143.35", 8081))
    {
        std::cout << "Server started at http://172.20.143.35:8081" << std::endl;
    }
    else
    {
        std::cerr << "Error starting server" << std::endl;
    }

    return 0;
}
