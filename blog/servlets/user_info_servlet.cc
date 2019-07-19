#include "user_info_servlet.h"
#include "blog/util.h"
#include "blog/struct.h"
#include "blog/manager/user_manager.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

UserInfoServlet::UserInfoServlet()
    :BlogServlet("UserInfo") {
}

int32_t UserInfoServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        auto sdata = getSessionData(request, response);
        int64_t uid = sdata->getData<int64_t>(CookieKey::USER_ID);
        if(!uid) {
            result->setResult(410, "not login");
            break;
        }

        data::UserInfo::ptr info = UserMgr::GetInstance()->get(uid);
        if(!info) {
            result->setResult(403, "invalid account");
            break;
        }
        result->setResult(200, "ok");
        result->set("account", info->getAccount());
        result->set("email", info->getEmail());
        result->set("name", info->getName());
        result->set("login_time", info->getLoginTime());
    } while(false);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
