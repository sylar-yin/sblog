#include "comment_create_servlet.h"
#include "sylar/log.h"
#include "blog/manager/article_manager.h"
#include "blog/manager/comment_manager.h"
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
static sylar::ConfigVar<int64_t>::ptr g_comment_interval_time =
    sylar::Config::Lookup("comment.interval_time", (int64_t)60, "comment interval time second");

CommentCreateServlet::CommentCreateServlet()
    :BlogLoginedServlet("CommentCreate") {
}

int32_t CommentCreateServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        DEFINE_AND_CHECK_STRING(result, content, "content");
        int64_t comment_id = request->getParamAs<int64_t>("comment_id");

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
        auto data = getSessionData(request, response);
        if(!data) {
            result->setResult(502, "not login");
            break;
        }

        int64_t now = time(0);
        int64_t comment_time = data->getData<int64_t>(CookieKey::COMMENT_LAST_TIME);
        if((now - comment_time) < g_comment_interval_time->getValue()) {
            result->setResult(502, "comment too often");
            break;
        }
        //TODO check article state

        if(comment_id) {
            data::CommentInfo::ptr cinfo = CommentMgr::GetInstance()->get(comment_id);
            if(!cinfo) {
                result->setResult(401, "invalid comment_id");
                break;
            }
        }
        data::CommentInfo::ptr cinfo(new data::CommentInfo);
        cinfo->setContent(content);
        cinfo->setArticleId(id);
        cinfo->setUserId(uid);
        cinfo->setParentId(comment_id);
        cinfo->setState((int)State::VERIFYING);
        cinfo->setUpdateTime(time(0));
        auto db = getDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }
        if(data::CommentInfoDao::Insert(cinfo, db)) {
            result->setResult(500, "insert comment fail");

            SYLAR_LOG_ERROR(g_logger) << "db error errno=" << db->getErrno()
                << " errstr=" << db->getErrStr();
            break;
        }
        CommentMgr::GetInstance()->add(cinfo);
        result->setResult(200, "ok");
        result->set("id", cinfo->getId());

        data->setData<int64_t>(CookieKey::COMMENT_LAST_TIME, now);
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
