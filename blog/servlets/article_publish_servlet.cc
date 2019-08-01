#include "article_publish_servlet.h"
#include "sylar/log.h"
#include "blog/manager/article_manager.h"
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

ArticlePublishServlet::ArticlePublishServlet()
    :BlogLoginedServlet("ArticlePublish") {
}

int32_t ArticlePublishServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        DEFINE_AND_CHECK_TYPE(result, int64_t, publish_time, "publish_time");

        int64_t uid = getUserId(request);
        if(!uid) {
            result->setResult(500, "not login");
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

        if(info->getState() != 0) {
            result->setResult(401, "invalid state");
            break;
        }
        info->setState((int)State::VERIFYING);
        info->setPublishTime(publish_time);
        info->setUpdateTime(time(0));
        auto db = getDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }
        if(data::ArticleInfoDao::Update(info, db)) {
            result->setResult(500, "update article fail");

            SYLAR_LOG_ERROR(g_logger) << "db error errno=" << db->getErrno()
                << " errstr=" << db->getErrStr();
            break;
        }
        ArticleMgr::GetInstance()->add(info);
        result->setResult(200, "ok");
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
