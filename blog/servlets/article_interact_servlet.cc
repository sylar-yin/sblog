#include "article_interact_servlet.h"
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

ArticleInteractServlet::ArticleInteractServlet()
    :BlogServlet("ArticleInteract") {
}

int32_t ArticleInteractServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        DEFINE_AND_CHECK_TYPE(result, int64_t, type, "type");
        int64_t cancel = request->getParamAs<int64_t>("cancel", 0);

        auto info = ArticleMgr::GetInstance()->get(id);
        if(!info) {
            result->setResult(400, "article not exists");
            break;
        }

        int64_t uid = getUserId(request);
        if(type == 1) { //浏览
            ArticleMgr::GetInstance()->incViews(id, getCookieId(request), uid);
        } else if(type == 2) { //点赞
            if(!uid) {
                result->setResult(500, "not login");
                break;
            }
            if(!cancel) {
                ArticleMgr::GetInstance()->incPraise(id, getCookieId(request), uid);
            } else {
                ArticleMgr::GetInstance()->decPraise(id, getCookieId(request), uid);
            }
        } else if(type == 3) { //收藏
            if(!uid) {
                result->setResult(500, "not login");
                break;
            }
            if(!cancel) {
                ArticleMgr::GetInstance()->incFavorites(id, getCookieId(request), uid);
            } else {
                ArticleMgr::GetInstance()->decFavorites(id, getCookieId(request), uid);
            }
        } else {
            result->setResult(400, "invalid type");
            break;
        }
        result->setResult(200, "ok");
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
