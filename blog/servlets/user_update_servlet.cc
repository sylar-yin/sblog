#include "user_update_servlet.h"
#include "blog/util.h"
#include "blog/struct.h"
#include "blog/manager/user_manager.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

UserUpdateServlet::UserUpdateServlet()
    :BlogServlet("UserUpdate") {
}

int32_t UserUpdateServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        std::string name = request->getParam("name");
        std::string passwd = request->getParam("passwd");

        if(name.empty() && passwd.empty()) {
            result->setResult(400, "no param");
            break;
        }

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

        if(!name.empty()) {
            info->setName(name);
        }
        if(!passwd.empty()) {
            info->setPasswd(passwd);
        }

        auto db = getDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }
        if(data::UserInfoDao::Update(info, db)) {
            result->setResult(500, "update user fail");
            SYLAR_LOG_ERROR(g_logger) << "db error errno=" << db->getErrno()
                << " errstr=" << db->getErrStr();
            //TODO log
            break;
        }
        result->setResult(200, "ok");

        if(!passwd.empty()) {
            int64_t token_time = time(0) + 3600 * 24;
            response->setCookie(CookieKey::USER_ID, std::to_string(info->getId()), token_time, "/");
            auto token = UserManager::GetToken(info, token_time);
            response->setCookie(CookieKey::TOKEN, token, token_time, "/");
            response->setCookie(CookieKey::TOKEN_TIME, std::to_string(token_time), token_time, "/");
        }
    } while(false);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
