#include "user_exists_servlet.h"
#include "blog/util.h"
#include "blog/struct.h"
#include "blog/manager/user_manager.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

UserExistsServlet::UserExistsServlet()
    :BlogServlet("UserExists") {
}

int32_t UserExistsServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, auth_id, "auth_id");
        data::UserInfo::ptr info;
        if(is_email(auth_id)) {
            info = UserMgr::GetInstance()->getByEmail(auth_id);
        } else if(is_valid_account(auth_id)) {
            info = UserMgr::GetInstance()->getByAccount(auth_id);
        } else {
            result->setResult(402, "invalid auth_id");
            break;
        }
        result->setResult(200, "ok");
        result->set("is_exists", info ? "1" : "0");
    } while(false);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
