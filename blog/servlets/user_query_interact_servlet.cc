#include "user_query_interact_servlet.h"
#include "blog/util.h"
#include "blog/struct.h"
#include "blog/manager/article_manager.h"
#include "blog/manager/user_manager.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

UserQueryInteractServlet::UserQueryInteractServlet()
    :BlogServlet("UserQueryInteract") {
}

int32_t UserQueryInteractServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        DEFINE_AND_CHECK_TYPE(result, int64_t, type, "type");
        data::UserInfo::ptr info = UserMgr::GetInstance()->get(id);
        if(!info) {
            result->setResult(400, "invalid user");
            break;
        }
        std::map<int64_t, int64_t> vals;
        if(type == 2) {
            ArticleMgr::GetInstance()->listUserPra(id, vals);
        } else if(type == 3) {
            ArticleMgr::GetInstance()->listUserFav(id, vals);
        } else {
            result->setResult(400, "invalid type");
            break;
        }

        result->setResult(200, "ok");
        for(auto& i : vals) {
            Json::Value v;
            v["id"] = std::to_string(i.first);
            v["time"] = std::to_string(i.second);
            result->jsondata.append(v);
        }
    } while(false);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
