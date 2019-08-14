#include "article_property_servlet.h"
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

ArticlePropertyServlet::ArticlePropertyServlet()
    :BlogServlet("ArticleProperty") {
}

int32_t ArticlePropertyServlet::handle(sylar::http::HttpRequest::ptr request
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

        
        DEFINE_AND_CHECK_STRING(result, fields, "fields") \

        auto index = IndexMgr::GetInstance()->get();
        if(!index) {
            result->setResult(500, "index not ready");
            break;
        }
        std::map<uint64_t, std::set<uint64_t> > params;
        std::map<uint64_t, std::set<uint64_t> > query_params;
        ParseParams(params, args);
        ParseFields(query_params, fields);
        std::map<uint64_t, std::map<uint64_t, uint64_t> > props;
        int idx = index->property(props, params, query_params);

        if(idx <= 0) {
            result->setResult(400, "ok");
            break;
        }
        result->setResult(200, "ok");
        for(auto& i : props) {
            for(auto& n : i.second) {
                if(GetIndexTypeType(i.first) == 2) {
                    result->jsondata[GetIndexTypeName(i.first)][index->getStr(n.first)] = n.second;
                } else {
                    result->jsondata[GetIndexTypeName(i.first)][std::to_string(n.first)] = n.second;
                }
            }
        }
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
