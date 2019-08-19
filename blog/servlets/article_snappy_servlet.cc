#include "article_snappy_servlet.h"
#include "sylar/log.h"
#include "blog/manager/article_manager.h"
#include "blog/manager/channel_manager.h"
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

ArticleSnappyServlet::ArticleSnappyServlet()
    :BlogServlet("ArticleSnappy") {
}

int32_t ArticleSnappyServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, ids, "ids");
        auto tmp = sylar::split(ids, ",");
        for(auto& x : tmp) {
            auto id = sylar::TypeUtil::Atoi(x);
            if(!id) {
                continue;
            }
            data::ArticleInfo::ptr info = ArticleMgr::GetInstance()->get(id);
            if(!info) {
                continue;
            }
            Json::Value v;
            v["id"] = info->getId();
            v["title"] = info->getTitle();
            v["content"] = get_max_length_string(info->getContent(), 100);
            v["user_id"] = info->getUserId();
            auto uinfo = UserMgr::GetInstance()->get(info->getUserId());
            if(uinfo) {
                v["user_name"] = uinfo->getName();
            } else {
                v["user_name"] = "system";
            }
            v["type"] = info->getType();
            v["publish_time"] = info->getPublishTime();
            v["views"] = info->getViews();
            v["praise"] = info->getPraise();
            v["favorites"] = info->getFavorites();

            v["channel_id"] = info->getChannel();
            auto cinfo = ChannelMgr::GetInstance()->get(info->getChannel());
            if(cinfo) {
                v["channel_name"] = cinfo->getName();
            }

            std::vector<data::ArticleCategoryRelInfo::ptr> cinfos;
            ArticleCategoryRelMgr::GetInstance()->listByArticleId(cinfos, id, true);

            for(auto& i : cinfos) {
                auto c = CategoryMgr::GetInstance()->get(i->getCategoryId());
                if(c && c->getIsDeleted() == 0) {
                    Json::Value vv;
                    vv["id"] = c->getId();
                    vv["name"] = c->getName();
                    v["categorys"].append(vv);
                    //result->append("categorys", c->getId());
                }
            }

            std::vector<data::ArticleLabelRelInfo::ptr> linfos;
            ArticleLabelRelMgr::GetInstance()->listByArticleId(linfos, id, true);

            for(auto& i : linfos) {
                auto l = LabelMgr::GetInstance()->get(i->getLabelId());
                if(l && l->getIsDeleted() == 0) {
                    //result->append("labels", l->getId());
                    Json::Value vv;
                    vv["id"] = l->getId();
                    vv["name"] = l->getName();
                    v["labels"].append(vv);
                }
            }
            result->jsondata.append(v);
        }
        result->setResult(200, "ok");
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
