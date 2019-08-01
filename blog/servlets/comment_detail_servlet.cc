#include "comment_detail_servlet.h"
#include "sylar/log.h"
#include "blog/manager/comment_manager.h"
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

CommentDetailServlet::CommentDetailServlet()
    :BlogServlet("CommentDetail") {
}

int32_t CommentDetailServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, ids, "ids");
        auto tmp = sylar::split(ids, ',');
        for(auto& i : tmp) {
            auto id = sylar::TypeUtil::Atoi(i);
            if(!id) {
                continue;
            }
            Json::Value v;
            data::CommentInfo::ptr info = CommentMgr::GetInstance()->get(id);
            if(!info) {
                continue;
            }

            v["id"] = info->getId();
            v["content"] = info->getContent();
            v["user_id"] = info->getUserId();
            v["time"] = info->getCreateTime();
            if(info->getParentId()) {
                v["parent_id"] = info->getParentId();
            }
            data::UserInfo::ptr uinfo = UserMgr::GetInstance()->get(info->getUserId());
            if(uinfo) {
                v["user_name"] = uinfo->getName();
            }
            v["article_id"] = info->getArticleId();
            result->jsondata.append(v);
        }
        result->setResult(200, "ok");
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
