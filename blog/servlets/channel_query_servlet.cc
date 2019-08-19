#include "channel_query_servlet.h"
#include "sylar/log.h"
#include "blog/manager/channel_manager.h"
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

ChannelQueryServlet::ChannelQueryServlet()
    :BlogServlet("ChannelQuery") {
}

int32_t ChannelQueryServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        std::map<int64_t, data::ChannelInfo::ptr> infos;
        ChannelMgr::GetInstance()->listAll(infos);

        result->setResult(200, "ok");
        for(auto& i : infos) {
            Json::Value v;
            v["id"] = i.second->getId();
            v["name"] = i.second->getName();
            result->jsondata.append(v);
        }
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
