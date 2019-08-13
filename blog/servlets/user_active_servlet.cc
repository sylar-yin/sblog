#include "user_active_servlet.h"
#include "blog/util.h"
#include "blog/struct.h"
#include "blog/manager/user_manager.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

UserActiveServlet::UserActiveServlet()
    :BlogServlet("UserActive") {
}

int32_t UserActiveServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, auth_id, "auth_id");
        DEFINE_AND_CHECK_STRING(result, auth_code, "auth_code");

        data::UserInfo::ptr info;
        if(is_email(auth_id)) {
            info = UserMgr::GetInstance()->getByEmail(auth_id);
        } else if(is_valid_account(auth_id)) {
            info = UserMgr::GetInstance()->getByAccount(auth_id);
        } else {
            result->setResult(402, "invalid auth_id");
            break;
        }
        if(!info) {
            result->setResult(403, "invalid auth_id");
            break;
        }
        if(info->getState() != 1) {
            result->setResult(410, "account invalid state");
            break;
        }

        if(info->getCode() != auth_code) {
            result->setResult(410, "invalid auth code");
            break;
        }

        info->setCode("");
        info->setState((int)State::PUBLISH);
        info->setUpdateTime(time(0));

        auto db = getDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }
        if(data::UserInfoDao::Update(info, db)) {
            result->setResult(500, "db update error");
            SYLAR_LOG_ERROR(g_logger) << "user_data: " << info->toJsonString()
                << " errno: " << db->getErrno()
                << " errstr: " << db->getErrStr();
            break;
        }
        SendWX("blog", "用户激活成功[" + auth_id + "]");
        result->setResult(200, "ok");
    } while(false);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
