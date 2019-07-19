#include "user_change_passwd_servlet.h"
#include "blog/util.h"
#include "blog/struct.h"
#include "blog/manager/user_manager.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

UserChangePasswdServlet::UserChangePasswdServlet()
    :BlogServlet("UserChangePasswd") {
}

int32_t UserChangePasswdServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, email, "email");
        DEFINE_AND_CHECK_STRING(result, passwd, "passwd");
        DEFINE_AND_CHECK_STRING(result, code, "code");

        data::UserInfo::ptr info;
        if(is_email(email)) {
            info = UserMgr::GetInstance()->getByEmail(email);
        } else {
            result->setResult(402, "invalid email");
            break;
        }
        if(!info) {
            result->setResult(403, "email not register");
            break;
        }
        if(info->getCode() != code) {
            result->setResult(410, "code error");
            break;
        }
        if(info->getPasswd() == passwd) {
            result->setResult(410, "same passwd");
            break;
        }
        info->setCode("");
        info->setPasswd(passwd);
        info->setUpdateTime(time(0));
        auto db = getDB();
        if(data::UserInfoDao::Update(info, db)) {
            result->setResult(500, "db update error");
            break;
        }
        result->setResult(200, "ok");
    } while(false);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
