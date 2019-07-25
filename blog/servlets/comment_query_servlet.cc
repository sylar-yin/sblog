#include "comment_query_servlet.h"
#include "sylar/log.h"
#include "blog/manager/comment_manager.h"
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

CommentQueryServlet::CommentQueryServlet()
    :BlogServlet("CommentQuery") {
}

int32_t CommentQueryServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        int64_t page_from = request->getParamAs<int64_t>("page_from");
        int64_t page_size = request->getParamAs<int64_t>("page_size", 20);
        std::vector<data::CommentInfo::ptr> infos;
        auto total = CommentMgr::GetInstance()->listByArticleId(infos
                    ,id, page_from, page_size, true);
        result->setResult(200, "ok");
        result->jsondata["total"] = total;
        result->jsondata["page_from"] = page_from;
        result->jsondata["page_size"] = page_size;
        for(auto& i : infos) {
            result->jsondata["ids"].append(i->getId());
        }
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
