#include "article_update_category_servlet.h"
#include "sylar/log.h"
#include "blog/manager/article_manager.h"
#include "blog/manager/article_category_rel_manager.h"
#include "blog/manager/category_manager.h"
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

ArticleUpdateCategoryServlet::ArticleUpdateCategoryServlet()
    :BlogLoginedServlet("ArticleUpdateCategory") {
}

int32_t ArticleUpdateCategoryServlet::handle(sylar::http::HttpRequest::ptr request
                                             ,sylar::http::HttpResponse::ptr response
                                             ,sylar::http::HttpSession::ptr session
                                             ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        std::string adds = request->getParam("add_category_ids");
        std::string dels = request->getParam("del_category_ids");

        if(adds.empty() && dels.empty()) {
            result->setResult(400, "add_category_ids and del_category_ids is null");
            break;
        }

        auto ainfo = ArticleMgr::GetInstance()->get(id);
        if(!ainfo) {
            result->setResult(401, "invalid id");
            break;
        }
        int64_t uid = getUserId(request);
        if(ainfo->getUserId() != uid) {
            result->setResult(401, "invalid article");
            break;
        }

        std::vector<std::string> tmps;
        if(!adds.empty()) {
            tmps = sylar::split(adds, ',');
        }
        time_t now = time(0);

        auto db = getDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }

        auto trans = db->openTransaction();
        if(!trans) {
            result->setResult(500, "open transaction fail");
            break;
        }

        std::vector<data::ArticleCategoryRelInfo::ptr> new_infos;
        std::vector<data::ArticleCategoryRelInfo::ptr> update_add_infos;
        std::vector<data::ArticleCategoryRelInfo::ptr> update_del_infos;
        for(auto& i : tmps) {
            int64_t cid = sylar::TypeUtil::Atoi(i);
            auto cinfo = CategoryMgr::GetInstance()->get(cid);
            if(!cinfo) {
                continue;
            }
            if(cinfo->getUserId() != uid) {
                continue;
            }
            auto acinfo = ArticleCategoryRelMgr::GetInstance()->getByArticleIdCategoryId(id, cid);
            if(acinfo) {
                if(acinfo->getIsDeleted()) {
                    acinfo->setIsDeleted(0);
                    acinfo->setUpdateTime(now);
                    update_add_infos.push_back(acinfo);
                    data::ArticleCategoryRelInfoDao::Update(acinfo, db);
                }
            } else {
                acinfo.reset(new data::ArticleCategoryRelInfo);
                acinfo->setArticleId(id);
                acinfo->setCategoryId(cid);
                acinfo->setUpdateTime(now);
                new_infos.push_back(acinfo);
                data::ArticleCategoryRelInfoDao::Insert(acinfo, db);
            }
        }
        tmps.clear();
        if(!dels.empty()) {
            tmps = sylar::split(dels, ',');
        }
        for(auto& i : tmps) {
            int64_t cid = sylar::TypeUtil::Atoi(i);
            auto cinfo = CategoryMgr::GetInstance()->get(cid);
            if(!cinfo) {
                continue;
            }
            if(cinfo->getUserId() != uid) {
                continue;
            }
            auto acinfo = ArticleCategoryRelMgr::GetInstance()->getByArticleIdCategoryId(id, cid);
            if(acinfo) {
                if(acinfo->getIsDeleted() == 0) {
                    acinfo->setIsDeleted(1);
                    acinfo->setUpdateTime(now);
                    new_infos.push_back(acinfo);
                    update_del_infos.push_back(acinfo);
                    data::ArticleCategoryRelInfoDao::Update(acinfo, db);
                }
            }
        }

        if(!trans->commit()) {
            for(auto& i : update_add_infos) {
                i->setIsDeleted(1);
            }
            for(auto& i : update_del_infos) {
                i->setIsDeleted(0);
            }
            result->setResult(500, "commit fail");
            break;
        }

        for(auto& i : new_infos) {
            ArticleCategoryRelMgr::GetInstance()->add(i);
        }
        result->setResult(200, "ok");
        if(!update_add_infos.empty()) {
            auto& v = result->jsondata["add_category_ids"];
            for(auto& i : update_add_infos) {
                v.append(i->getId());
            }
        }
        if(!new_infos.empty()) {
            auto& v = result->jsondata["add_category_ids"];
            for(auto& i : new_infos) {
                v.append(i->getId());
            }
        }
        if(!update_del_infos.empty()) {
            auto& v = result->jsondata["del_category_ids"];
            for(auto& i : update_del_infos) {
                v.append(i->getId());
            }
        }
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
