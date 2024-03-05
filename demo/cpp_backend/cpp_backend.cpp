#include <httplib.h>

int main() {
    httplib::Server server;

    server.Get("/api/data", [](const httplib::Request& req, httplib::Response& res) {
        // 模拟与数据库的交互
        // 实际项目中这里应该处理数据库操作
        std::string data_from_cpp = "{\"message\": \"Hello from C++ backend\"}";

        res.set_content(data_from_cpp, "application/json");
    });

    server.listen("localhost", 8080);

    return 0;
}
