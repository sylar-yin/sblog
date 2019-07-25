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
        cinfo->setState(1);
        cinfo->setUpdateTime(time(0));
        auto db = getDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }
        if(data::CommentInfoDao::Insert(cinfo, db)) {
            result->setResult(500, "insert comment fail");
            break;
        }
        CommentMgr::GetInstance()->add(cinfo);
        result->setResult(200, "ok");
        result->set("id", cinfo->getId());
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
