#include "article_nearby_servlet.h"
#include "sylar/log.h"
#include "blog/manager/article_manager.h"
#include "blog/manager/article_category_rel_manager.h"
#include "blog/manager/article_label_rel_manager.h"
#include "blog/manager/category_manager.h"
#include "blog/manager/label_manager.h"
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

ArticleNearbyServlet::ArticleNearbyServlet()
    :BlogServlet("ArticleNearby") {
}

int32_t ArticleNearbyServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result,int64_t, id, "id");
        auto v = ArticleMgr::GetInstance()->nearby(id);
        if(v.first) {
            result->set("prev_id", v.first->getId());
            result->set("prev_name", v.first->getTitle());
        }
        if(v.second) {
            result->set("next_id", v.second->getId());
            result->set("next_name", v.second->getTitle());
        }
        result->setResult(200, "ok");
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
