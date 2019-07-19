#include "user_query_servlet.h"
#include "blog/util.h"
#include "blog/struct.h"
#include "blog/manager/user_manager.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

UserQueryServlet::UserQueryServlet()
    :BlogServlet("UserQuery") {
}

int32_t UserQueryServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, user_ids, "user_ids");
        auto ids = sylar::split(user_ids, ',');
        std::vector<data::UserInfo::ptr> infos;

        for(auto& i : ids) {
            data::UserInfo::ptr info = UserMgr::GetInstance()->get(sylar::TypeUtil::Atoi(i));
            if(info) {
                infos.push_back(info);
            }
        }

        result->setResult(200, "ok");
        for(auto& i : infos) {
            Json::Value v;
            v["id"] = i->getId();
            v["name"] = i->getName();
            result->jsondata.append(v);
        }
        SYLAR_LOG_INFO(g_logger) << "infos.size=" << infos.size()
            << " - " << sylar::JsonUtil::ToString(result->jsondata);
    } while(false);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
