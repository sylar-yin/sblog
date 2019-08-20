#include "article_query_interact_servlet.h"
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
#include "blog/index.h"
#include <regex>

namespace blog {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

ArticleQueryInteractServlet::ArticleQueryInteractServlet()
    :BlogServlet("ArticleQueryInteract") {
}

int32_t ArticleQueryInteractServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        DEFINE_AND_CHECK_TYPE(result, int64_t, type, "type");

        auto info = ArticleMgr::GetInstance()->get(id);
        if(!info) {
            result->setResult(400, "invalid article");
            break;
        }
        std::map<int64_t, int64_t> vals;
        if(type == 2) {
            ArticleMgr::GetInstance()->listArticlePra(id, vals);
        } else if(type == 3) {
            ArticleMgr::GetInstance()->listArticleFav(id, vals);
        } else {
            result->setResult(400, "invalid type");
            break;
        }

        result->setResult(200, "ok");
        for(auto& i : vals) {
            Json::Value v;
            v["id"] = std::to_string(i.first);
            auto uinfo = UserMgr::GetInstance()->get(i.first);
            if(uinfo) {
                v["name"] = uinfo->getName();
            }
            v["time"] = std::to_string(i.second);
            result->jsondata.append(v);
        }
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
