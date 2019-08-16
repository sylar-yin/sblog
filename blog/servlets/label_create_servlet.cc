#include "label_create_servlet.h"
#include "sylar/log.h"
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

LabelCreateServlet::LabelCreateServlet()
    :BlogLoginedServlet("LabelCreate") {
}

int32_t LabelCreateServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, name, "name");
        int64_t uid = getUserId(request);
        if(!uid) {
            result->setResult(500, "not login");
            break;
        }

        auto info = LabelMgr::GetInstance()->getByUserIdName(uid, name);
        bool new_label = false;
        if(!info) {
            info.reset(new data::LabelInfo);
            info->setUserId(uid);
            info->setName(name);
            new_label = true;
        }
        info->setIsDeleted(0);
        info->setCreateTime(time(0));
        info->setUpdateTime(time(0));

        auto db = getDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }
        if(data::LabelInfoDao::InsertOrUpdate(info, db)) {
            result->setResult(500, "insert or update label fail");
            SYLAR_LOG_ERROR(g_logger) << "db error errno=" << db->getErrno()
                << " errstr=" << db->getErrStr();
            break;
        }

        if(new_label) {
            LabelMgr::GetInstance()->add(info);
        }

        result->setResult(200, "ok");
        result->set("id", info->getId());
        result->set("name", info->getName());
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
