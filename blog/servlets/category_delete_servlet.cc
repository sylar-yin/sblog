#include "category_delete_servlet.h"
#include "sylar/log.h"
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

CategoryDeleteServlet::CategoryDeleteServlet()
    :BlogLoginedServlet("CategoryDelete") {
}

void get_delete_values(std::map<int64_t, std::map<int64_t, data::CategoryInfo::ptr> > parent_map
                       ,int64_t id, std::set<data::CategoryInfo::ptr>& infos) {
    auto it = parent_map[0].find(id);
    if(it == parent_map[0].end()) {
        return;
    }
    if(!infos.insert(it->second).second) {
        return;
    }
    auto iit = parent_map.find(id);
    if(iit == parent_map.end()) {
        return;
    }
    for(auto& i : iit->second) {
        get_delete_values(parent_map, i.first, infos);
    }
}

int32_t CategoryDeleteServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, ids, "ids");
        std::set<int64_t> cat_ids;
        auto tmp = sylar::split(ids, ',');
        for(auto& i : tmp) {
            cat_ids.insert(sylar::TypeUtil::Atoi(i));
        }
        int64_t uid = getUserId(request);
        if(!uid) {
            result->setResult(500, "not login");
            break;
        }

        std::vector<data::CategoryInfo::ptr> infos;
        if(!CategoryMgr::GetInstance()->listByUserId(infos, uid, true)) {
            result->setResult(200, "ok");
            break;
        }

        std::map<int64_t, std::map<int64_t, data::CategoryInfo::ptr> > parent_map;
        for(auto& i : infos) {
            if(i->getParentId()) {
                parent_map[i->getParentId()][i->getId()] = i;
            }
            parent_map[0][i->getId()] = i;
        }

        std::set<data::CategoryInfo::ptr> del_cats;
        for(auto& i : cat_ids) {
            get_delete_values(parent_map, i, del_cats);
        }

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
        time_t now = time(0);
        for(auto& i : del_cats) {
            i->setIsDeleted(1);
            i->setUpdateTime(now);
            data::CategoryInfoDao::Update(i, db);
        }
        if(!trans->commit()) {
            SYLAR_LOG_ERROR(g_logger) << "commit fail";
            result->setResult(500, "commit fail");

            for(auto& i : del_cats) {
                i->setIsDeleted(0);
            }
            break;
        }
        result->setResult(200, "ok");
        if(!del_cats.empty()) {
            auto& jids = result->jsondata["ids"];
            for(auto& i : del_cats) {
                jids.append(i->getId());
            }
        }
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
