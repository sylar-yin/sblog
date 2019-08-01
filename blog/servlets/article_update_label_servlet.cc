#include "article_update_label_servlet.h"
#include "sylar/log.h"
#include "blog/manager/article_manager.h"
#include "blog/manager/article_label_rel_manager.h"
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

ArticleUpdateLabelServlet::ArticleUpdateLabelServlet()
    :BlogLoginedServlet("ArticleUpdateLabel") {
}

int32_t ArticleUpdateLabelServlet::handle(sylar::http::HttpRequest::ptr request
                                             ,sylar::http::HttpResponse::ptr response
                                             ,sylar::http::HttpSession::ptr session
                                             ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        std::string adds = request->getParam("add_label_ids");
        std::string dels = request->getParam("del_label_ids");

        if(adds.empty() && dels.empty()) {
            result->setResult(400, "add_label_ids and del_label_ids is null");
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

        std::vector<data::ArticleLabelRelInfo::ptr> new_infos;
        std::vector<data::ArticleLabelRelInfo::ptr> update_add_infos;
        std::vector<data::ArticleLabelRelInfo::ptr> update_del_infos;
        for(auto& i : tmps) {
            int64_t cid = sylar::TypeUtil::Atoi(i);
            auto cinfo = LabelMgr::GetInstance()->get(cid);
            if(!cinfo) {
                continue;
            }
            if(cinfo->getUserId() != uid) {
                continue;
            }
            auto acinfo = ArticleLabelRelMgr::GetInstance()->getByArticleIdLabelId(id, cid);
            if(acinfo) {
                if(acinfo->getIsDeleted()) {
                    acinfo->setIsDeleted(0);
                    acinfo->setUpdateTime(now);
                    update_add_infos.push_back(acinfo);
                    data::ArticleLabelRelInfoDao::Update(acinfo, db);
                    SYLAR_LOG_ERROR(g_logger) << "db error errno=" << db->getErrno()
                        << " errstr=" << db->getErrStr();
                }
            } else {
                acinfo.reset(new data::ArticleLabelRelInfo);
                acinfo->setArticleId(id);
                acinfo->setLabelId(cid);
                acinfo->setUpdateTime(now);
                new_infos.push_back(acinfo);
                data::ArticleLabelRelInfoDao::Insert(acinfo, db);
                SYLAR_LOG_ERROR(g_logger) << "db error errno=" << db->getErrno()
                    << " errstr=" << db->getErrStr();
            }
        }
        tmps.clear();
        if(!dels.empty()) {
            tmps = sylar::split(dels, ',');
        }
        for(auto& i : tmps) {
            int64_t cid = sylar::TypeUtil::Atoi(i);
            auto cinfo = LabelMgr::GetInstance()->get(cid);
            if(!cinfo) {
                continue;
            }
            if(cinfo->getUserId() != uid) {
                continue;
            }
            auto acinfo = ArticleLabelRelMgr::GetInstance()->getByArticleIdLabelId(id, cid);
            if(acinfo) {
                if(acinfo->getIsDeleted() == 0) {
                    acinfo->setIsDeleted(1);
                    acinfo->setUpdateTime(now);
                    new_infos.push_back(acinfo);
                    update_del_infos.push_back(acinfo);
                    data::ArticleLabelRelInfoDao::Update(acinfo, db);
                    SYLAR_LOG_ERROR(g_logger) << "db error errno=" << db->getErrno()
                        << " errstr=" << db->getErrStr();
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
            ArticleLabelRelMgr::GetInstance()->add(i);
        }
        result->setResult(200, "ok");
        if(!update_add_infos.empty()) {
            auto& v = result->jsondata["add_label_ids"];
            for(auto& i : update_add_infos) {
                v.append(i->getId());
            }
        }
        if(!new_infos.empty()) {
            auto& v = result->jsondata["add_label_ids"];
            for(auto& i : new_infos) {
                v.append(i->getId());
            }
        }
        if(!update_del_infos.empty()) {
            auto& v = result->jsondata["del_label_ids"];
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
