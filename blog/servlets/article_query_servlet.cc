#include "article_query_servlet.h"
#include "sylar/log.h"
#include "blog/manager/article_manager.h"
#include "sylar/util.h"
#include "blog/my_module.h"
#include "sylar/email/smtp.h"
#include "sylar/env.h"
#include "sylar/sylar.h"
#include "blog/util.h"
#include "blog/struct.h"
#include "blog/index.h"
#include <regex>

namespace blog {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

ArticleQueryServlet::ArticleQueryServlet()
    :BlogServlet("ArticleQuery") {
}

int32_t ArticleQueryServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
    do {
        request->initParam();
        auto m = request->getParams();

        std::map<std::string, std::string> args;
        for(auto& i : m) {
            args.insert(i);
        }

        //int64_t user_id = request->getParamAs<int64_t>("user_id");
        ////int64_t state = (int64_t)State::PUBLISH;
        ////if(request->hasParam("state")) {
        //int64_t state = request->getParamAs<int64_t>("state");
        ////int64_t state = request->getParamAs<int64_t>("state", (int64_t)State::PUBLISH);
        ////}

        auto index = IndexMgr::GetInstance()->get();
        if(!index) {
            result->setResult(500, "index not ready");
            break;
        }
        std::map<uint64_t, std::set<uint64_t> > params;
        ParseParams(params, args);
        //if(user_id) {
        //    params[(uint64_t)IndexType::USER_ID].insert(user_id);
        //}
        //if(state) {
        //    params[(uint64_t)IndexType::STATE].insert(state);
        //}


        int64_t page_from = request->getParamAs<int64_t>("page_from");
        int64_t page_size = request->getParamAs<int64_t>("page_size", 20);
        std::vector<uint64_t> ids;
        int32_t total = index->search(ids, params, 1000);

        //std::vector<data::ArticleInfo::ptr> infos;
        //auto total = ArticleMgr::GetInstance()->listByUserIdPages(infos
        //            ,user_id, page_from, page_size, true, state);
        result->setResult(200, "ok");
        result->jsondata["total"] = total;
        result->jsondata["page_from"] = page_from;
        result->jsondata["page_size"] = page_size;
        for(size_t i = page_from, c = 0; (int64_t)c < page_size && i < ids.size(); ++i, ++c) {
            result->jsondata["ids"].append(ids[i]);
        }
        //for(auto& i : infos) {
        //    result->jsondata["ids"].append(i->getId());
        //}
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
