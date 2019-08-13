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
static sylar::ConfigVar<int64_t>::ptr g_article_interval_time =
    sylar::Config::Lookup("article.interval_time", (int64_t)60, "article interval time second");

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

        auto data = getSessionData(request, response);
        if(!data) {
            result->setResult(502, "not login");
            break;
        }

        int64_t now = time(0);
        int64_t article_time = data->getData<int64_t>(CookieKey::ARTICLE_LAST_TIME);
        if((now - article_time) < g_article_interval_time->getValue()) {
            result->setResult(502, "article too often");
            break;
        }

        data::ArticleInfo::ptr info(new data::ArticleInfo);
        info->setTitle(title);
        info->setContent(content);
        info->setType(type);
        info->setUserId(uid);
        info->setUpdateTime(time(0));
        auto db = getDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }
        if(data::ArticleInfoDao::Insert(info, db)) {
            result->setResult(500, "insert article fail");
            SYLAR_LOG_ERROR(g_logger) << "db error errno=" << db->getErrno()
                << " errstr=" << db->getErrStr();
            break;
        }
        ArticleMgr::GetInstance()->add(info);
        result->setResult(200, "ok");
        result->set("id", info->getId());
        data->setData<int64_t>(CookieKey::ARTICLE_LAST_TIME, now);
        SendWX("blog", "[" + std::to_string(uid) + "]创建文章成功[" + title + "]");
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
