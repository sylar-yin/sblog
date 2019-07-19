#include "article_detail_servlet.h"
#include "sylar/log.h"
#include "blog/manager/article_manager.h"
#include "blog/manager/article_category_rel_manager.h"
#include "blog/manager/article_label_rel_manager.h"
#include "blog/manager/category_manager.h"
#include "blog/manager/label_manager.h"
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

ArticleDetailServlet::ArticleDetailServlet()
    :BlogServlet("ArticleDetail") {
}

int32_t ArticleDetailServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        data::ArticleInfo::ptr info = ArticleMgr::GetInstance()->get(id);
        if(!info) {
            result->setResult(404, "invalid id");
            break;
        }
        result->setResult(200, "ok");
        result->set("id", info->getId());
        result->set("title", info->getTitle());
        result->set("content", info->getContent());
        result->set("user_id", info->getUserId());
        result->set("type", info->getType());
        result->set("publish_time", info->getPublishTime());

        std::vector<data::ArticleCategoryRelInfo::ptr> cinfos;
        ArticleCategoryRelMgr::GetInstance()->listByArticleId(cinfos, id, true);

        for(auto& i : cinfos) {
            auto c = CategoryMgr::GetInstance()->get(i->getCategoryId());
            if(c && c->getIsDeleted() == 0) {
                result->append("categorys", c->getId());
            }
        }

        std::vector<data::ArticleLabelRelInfo::ptr> linfos;
        ArticleLabelRelMgr::GetInstance()->listByArticleId(linfos, id, true);

        for(auto& i : linfos) {
            auto l = LabelMgr::GetInstance()->get(i->getLabelId());
            if(l && l->getIsDeleted() == 0) {
                result->append("labels", l->getId());
            }
        }
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
