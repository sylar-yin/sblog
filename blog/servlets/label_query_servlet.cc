#include "label_query_servlet.h"
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

LabelQueryServlet::LabelQueryServlet()
    :BlogServlet("LabelQuery") {
}

int32_t LabelQueryServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, user_id, "user_id");
        std::vector<data::LabelInfo::ptr> infos;
        LabelMgr::GetInstance()->listByUserId(infos, user_id, true);
        result->setResult(0, "ok");
        for(auto& i : infos) {
            Json::Value v;
            v["id"] = i->getId();
            v["name"] = i->getName();
            result->jsondata.append(v);
        }
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
