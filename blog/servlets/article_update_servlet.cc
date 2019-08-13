#include "article_update_servlet.h"
#include "sylar/log.h"
#include "blog/manager/article_manager.h"
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

ArticleUpdateServlet::ArticleUpdateServlet()
    :BlogLoginedServlet("ArticleUpdate") {
}

int32_t ArticleUpdateServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        std::string title = request->getParam("title");
        std::string content = request->getParam("content");
        int32_t type = request->getParamAs<int32_t>("type");

        if(type && type != 1
                && type != 2) {
            result->setResult(401, "invalid type");
            break;
        }

        if(title.empty()
                && content.empty()
                && type == 0) {
            result->setResult(401, "param title,content,type all is null");
            break;
        }

        int64_t uid = getUserId(request);
        if(!uid) {
            result->setResult(500, "not login");
            break;
        }

        data::ArticleInfo::ptr info = ArticleMgr::GetInstance()->get(id);
        if(!info) {
            result->setResult(402, "article not exists");
            break;
        }

        if(uid != info->getUserId()) {
            result->setResult(402, "invalid author");
            break;
        }

        if(!title.empty()) {
            info->setTitle(title);
        }
        if(!content.empty()) {
            info->setContent(content);
        }
        if(type) {
            info->setType(type);
        }
        info->setState((int)State::VERIFYING);
        ArticleMgr::GetInstance()->addVerify(info);
        info->setUpdateTime(time(0));
        auto db = getDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }
        if(data::ArticleInfoDao::Update(info, db)) {
            result->setResult(500, "update article fail");
            SYLAR_LOG_ERROR(g_logger) << "db error errno=" << db->getErrno()
                << " errstr=" << db->getErrStr();
            break;
        }
        result->setResult(200, "ok");
        SendWX("blog", "[" + std::to_string(uid) + "]更新文章[" + title + "], 需要审核");
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
