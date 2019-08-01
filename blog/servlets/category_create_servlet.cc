#include "category_create_servlet.h"
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

CategoryCreateServlet::CategoryCreateServlet()
    :BlogLoginedServlet("CategoryCreate") {
}

int32_t CategoryCreateServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, name, "name");
        int64_t parent_id = request->getParamAs<int64_t>("parent_id");

        int64_t uid = getUserId(request);
        if(!uid) {
            result->setResult(500, "not login");
            break;
        }

        data::CategoryInfo::ptr parent_info;
        if(parent_id) {
            parent_info = CategoryMgr::GetInstance()->get(parent_id);
            if(!parent_info) {
                result->setResult(401, "invalid parent_id");
                break;
            }
        }

        auto info = CategoryMgr::GetInstance()->getByUserIdName(uid, name);
        bool new_cat = false;
        if(!info) {
            info.reset(new data::CategoryInfo);
            info->setUserId(uid);
            info->setName(name);
            new_cat = true;
        }
        info->setParentId(parent_id);
        info->setIsDeleted(0);
        info->setUpdateTime(time(0));

        auto db = getDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }

        if(data::CategoryInfoDao::InsertOrUpdate(info, db)) {
            result->setResult(500, "insert or update category fail");
            SYLAR_LOG_ERROR(g_logger) << "db error errno=" << db->getErrno()
                << " errstr=" << db->getErrStr();
            break;
        }

        if(new_cat) {
            CategoryMgr::GetInstance()->add(info);
        }

        result->setResult(200, "ok");
        result->set("id", info->getId());
        result->set("name", info->getName());
        if(parent_id) {
            result->set("parent_id", parent_id);
        }
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
