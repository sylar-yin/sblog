#include "user_logout_servlet.h"
#include "blog/util.h"
#include "blog/struct.h"
#include "blog/manager/user_manager.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

UserLogoutServlet::UserLogoutServlet()
    :BlogServlet("UserLogout") {
}

int32_t UserLogoutServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        auto sdata = getSessionData(request, response);
        if(!sdata->getData<int64_t>(CookieKey::USER_ID)) {
            result->setResult(410, "not login");
            break;
        }

        result->setResult(200, "ok");
        int64_t token_time = time(0) - 3600 * 24;
        response->setCookie(CookieKey::USER_ID, "", token_time);
        response->setCookie(CookieKey::TOKEN, "", token_time);
        response->setCookie(CookieKey::TOKEN_TIME, "", token_time);
        sdata->setData(CookieKey::USER_ID, (int64_t)0);
        sylar::http::SessionDataMgr::GetInstance()->del(sdata->getId());
    } while(false);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
