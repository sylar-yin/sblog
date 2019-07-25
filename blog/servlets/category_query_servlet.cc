#include "category_query_servlet.h"
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

CategoryQueryServlet::CategoryQueryServlet()
    :BlogServlet("CategoryQuery") {
}

int32_t CategoryQueryServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        int64_t user_id = request->getParamAs<int64_t>("user_id");
        std::string ids = request->getParam("ids");
        if(user_id == 0 && ids.empty()) {
            result->setResult(400, "param user_id and ids is null");
            break;
        }

        std::vector<data::CategoryInfo::ptr> infos;
        if(user_id) {
            CategoryMgr::GetInstance()->listByUserId(infos, user_id, true);
        } else {
            auto tmp = sylar::split(ids, ',');
            for(auto& i : tmp) {
                auto id = sylar::TypeUtil::Atoi(i);
                if(id) {
                    auto info = CategoryMgr::GetInstance()->get(id);
                    if(info) {
                        infos.push_back(info);
                    }
                }
            }
        }
        result->setResult(200, "ok");
        for(auto& i : infos) {
            Json::Value v;
            v["id"] = i->getId();
            v["name"] = i->getName();
            if(i->getParentId()) {
                v["parent_id"] = i->getParentId();
            }
            result->jsondata.append(v);
        }
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
