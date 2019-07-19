#include "article_create_servlet.h"
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

ArticleCreateServlet::ArticleCreateServlet()
    :BlogLoginedServlet("ArticleCreate") {
}

int32_t ArticleCreateServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, title, "title");
        DEFINE_AND_CHECK_STRING(result, content, "content");
        DEFINE_AND_CHECK_TYPE(result, int32_t, type, "type");

        if(type != 1
                && type != 2) {
            result->setResult(401, "invalid type");
            break;
        }

        int64_t uid = getUserId(request);
        if(!uid) {
            result->setResult(500, "not login");
            break;
        }
        data::ArticleInfo::ptr info(new data::ArticleInfo);
        info->setTitle(title);
        info->setContent(content);
        info->setType(type);
        info->setUserId(uid);
        info->setUpdateTime(time(0));
        auto db = getDB();
        if(data::ArticleInfoDao::Insert(info, db)) {
            result->setResult(500, "insert article fail");
            break;
        }
        ArticleMgr::GetInstance()->add(info);
        result->setResult(200, "ok");
        result->set("id", info->getId());
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
