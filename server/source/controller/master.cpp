#include <controller/master.hpp>

namespace web
{
    void master::home(const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback)
    {
        drogon::HttpViewData view_data;
        view_data.insert("title", "Home");

        auto resp = drogon::HttpResponse::newHttpViewResponse("view::main", view_data);
        callback(resp);
    }
} // web