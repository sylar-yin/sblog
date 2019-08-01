#include "article_verify_servlet.h"
#include "sylar/log.h"
#include "blog/manager/article_manager.h"
#include "blog/manager/user_manager.h"
#include "sylar/util.h"
#include "blog/my_module.h"
#include "sylar/email/smtp.h"
#include "sylar/env.h"
#include "sylar/sylar.h"
#include "blog/util.h"
#include "blog/struct.h"
#include <regex>

namespace blog {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

ArticleVerifyServlet::ArticleVerifyServlet()
    :BlogLoginedServlet("ArticleVerify") {
}

int32_t ArticleVerifyServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        DEFINE_AND_CHECK_TYPE(result, int64_t, state, "state");

        if(state != 2 
                && state != 3) {
            result->setResult(401, "invalid state");
            break;
        }

        int64_t uid = getUserId(request);
        if(!uid) {
            result->setResult(500, "not login");
            break;
        }

        data::UserInfo::ptr uinfo = UserMgr::GetInstance()->get(uid);
        if(!uinfo) {
            result->setResult(401, "invalid user");
            break;
        }

        if(uinfo->getRole() != (int)UserManager::RoleType::ADMIN) {
            result->setResult(401, "permission denied");
            break;
        }

        data::ArticleInfo::ptr info = ArticleMgr::GetInstance()->get(id);
        if(!info) {
            result->setResult(401, "invalid id");
            break;
        }

        if(info->getIsDeleted()) {
            result->setResult(401, "invalid article");
            break;
        }

        if(info->getState() != 1) {
            result->setResult(401, "invalid article state");
            break;
        }

        if(state == (int)State::NOT_PASS) {
            info->setState(state);
        } else if(state == (int)State::PUBLISH) {
            if(info->getPublishTime() <= time(0)) {
                info->setState((int)State::PUBLISH);
            } else {
                info->setState((int)State::UNPUBLISH);
            }
        }
        info->setUpdateTime(time(0));

        auto db = getDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }
        if(data::ArticleInfoDao::Update(info, db)) {
            result->setResult(500, "insert article fail");
            info->setState((int)State::VERIFYING);

            SYLAR_LOG_ERROR(g_logger) << "db error errno=" << db->getErrno()
                << " errstr=" << db->getErrStr();
            break;
        }
        result->setResult(200, "ok");
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
