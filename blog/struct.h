#ifndef __BLOG_STRUCT_H__
#define __BLOG_STRUCT_H__

#include <string>
#include <map>
#include "sylar/util.h"
#include "sylar/http/session_data.h"
#include "sylar/db/db.h"
#include "sylar/sylar.h"

namespace blog {

struct Result {
    typedef std::shared_ptr<Result> ptr;
    Result(int32_t c = 200, const std::string& m = "ok");
    int32_t code;
    int64_t used;
    std::string msg;
    std::map<std::string, std::string> datas;
    Json::Value jsondata;

    template<class T>
    void set(const std::string& key, const T& v) {
        datas[key] = std::to_string(v);
    }
    void set(const std::string& key, const char* v) {
        datas[key] = v;
    }
    void set(const std::string& key, const std::string& v) {
        datas[key] = v;
    }

    void setResult(int32_t c, const std::string& m);

    std::string toJsonString() const;
};

struct CookieKey {
    static const std::string SESSION_KEY;
    static const std::string USER_ID;
    static const std::string TOKEN;
    static const std::string TOKEN_TIME;
    static const std::string IS_AUTH;
};

class BlogServlet : public sylar::http::Servlet {
public:
    BlogServlet(const std::string& name);
    int32_t handle(sylar::http::HttpRequest::ptr request
                   ,sylar::http::HttpResponse::ptr response
                   ,sylar::http::HttpSession::ptr session) override;
protected:
    virtual bool handlePre(sylar::http::HttpRequest::ptr request
                           ,sylar::http::HttpResponse::ptr response
                           ,sylar::http::HttpSession::ptr session
                           ,Result::ptr result);
    virtual int32_t handle(sylar::http::HttpRequest::ptr request
                           ,sylar::http::HttpResponse::ptr response
                           ,sylar::http::HttpSession::ptr session
                           ,Result::ptr result) = 0;
    virtual bool handlePost(sylar::http::HttpRequest::ptr request
                           ,sylar::http::HttpResponse::ptr response
                           ,sylar::http::HttpSession::ptr session
                           ,Result::ptr result);
protected:
    sylar::http::SessionData::ptr getSessionData(sylar::http::HttpRequest::ptr request
                                                 ,sylar::http::HttpResponse::ptr response);
    bool initLogin(sylar::http::HttpRequest::ptr request
                   ,sylar::http::HttpResponse::ptr response);
protected:
    sylar::IDB::ptr getDB();
};

}

#endif
