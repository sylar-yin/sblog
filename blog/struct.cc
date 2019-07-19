#include "struct.h"
#include "blog/manager/user_manager.h"
#include "blog/util.h"
#include "sylar/db/sqlite3.h"

namespace blog {

static sylar::Logger::ptr g_logger_access = SYLAR_LOG_NAME("access");

const std::string CookieKey::SESSION_KEY = "SSESSIONID";
const std::string CookieKey::USER_ID = "S_UID";
const std::string CookieKey::TOKEN = "S_TOKEN";
const std::string CookieKey::TOKEN_TIME = "S_TOKEN_TIME";
const std::string CookieKey::IS_AUTH= "IS_AUTH";

Result::Result(int32_t c, const std::string& m)
    :code(c)
    ,used(sylar::GetCurrentUS())
    ,msg(m){
}

std::string Result::toJsonString() const {
    Json::Value v;
    v["code"] = std::to_string(code);
    v["msg"] = msg;
    v["used"] = ((sylar::GetCurrentUS() - used) / 1000.0);
    if(!jsondata.isNull()) {
        v["data"] = jsondata;
    } else {
        if(!datas.empty()) {
            auto& d = v["data"];
            for(auto& i : datas) {
                d[i.first] = i.second;
            }
        }
    }
    return sylar::JsonUtil::ToString(v);
}

void Result::setResult(int32_t c, const std::string& m) {
    code = c;
    msg = m;
}

BlogServlet::BlogServlet(const std::string& name)
    :sylar::http::Servlet(name) {
}

int32_t BlogServlet::handle(sylar::http::HttpRequest::ptr request
                           ,sylar::http::HttpResponse::ptr response
                           ,sylar::http::HttpSession::ptr session) {
    Result::ptr result = std::make_shared<Result>();
    if(handlePre(request, response, session, result)) {
        handle(request, response, session, result);
    }
    handlePost(request, response, session, result);
    return 0;
}

bool BlogServlet::handlePre(sylar::http::HttpRequest::ptr request
                           ,sylar::http::HttpResponse::ptr response
                           ,sylar::http::HttpSession::ptr session
                           ,Result::ptr result) {
    if(request->getPath() != "/user/login"
            && request->getPath() != "/user/logout") {
        initLogin(request, response);
    }
    if(request->getMethod() != sylar::http::HttpMethod::GET
            && request->getMethod() != sylar::http::HttpMethod::POST) {
        result->setResult(300, "invalid method");
        return false;
    }
    return true;
}

bool BlogServlet::handlePost(sylar::http::HttpRequest::ptr request
                           ,sylar::http::HttpResponse::ptr response
                           ,sylar::http::HttpSession::ptr session
                           ,Result::ptr result) {
    SYLAR_LOG_INFO(g_logger_access) << result->code << "\t"
        << result->msg << "\t" << request->getPath()
        << "\t" << (!request->getQuery().empty() ? request->getQuery() : "-");
    return true;
}

sylar::http::SessionData::ptr BlogServlet::getSessionData(sylar::http::HttpRequest::ptr request
                                                          ,sylar::http::HttpResponse::ptr response) {
    std::string sid = request->getCookie(CookieKey::SESSION_KEY);
    if(!sid.empty()) {
        auto data = sylar::http::SessionDataMgr::GetInstance()->get(sid);
        if(data) {
            return data;
        }
    }
    sylar::http::SessionData::ptr data(new sylar::http::SessionData(true));
    sylar::http::SessionDataMgr::GetInstance()->add(data);
    response->setCookie(CookieKey::SESSION_KEY, data->getId());
    request->setCookie(CookieKey::SESSION_KEY, data->getId());
    return data;
}

bool BlogServlet::initLogin(sylar::http::HttpRequest::ptr request
                           ,sylar::http::HttpResponse::ptr response) {
    auto data = getSessionData(request, response);
    int64_t uid = data->getData<int64_t>(CookieKey::USER_ID);
    if(uid) {
        return true;
    }
    int32_t is_auth = data->getData<int32_t>(CookieKey::IS_AUTH);
    if(is_auth) {
        return false;
    }
    bool is_login = false;
    do {
        int64_t uid = request->getCookieAs<int64_t>(CookieKey::USER_ID);
        if(!uid) {
            break;
        }
        auto token = request->getCookie(CookieKey::TOKEN);
        if(token.empty()) {
            break;
        }
        int64_t token_time = request->getCookieAs<int64_t>(CookieKey::TOKEN_TIME);
        if(token_time <= time(0)) {
            break;
        }

        data::UserInfo::ptr uinfo = UserMgr::GetInstance()->get(uid);
        if(!uinfo) {
            break;
        }
        if(uinfo->getState() != 2) {
            break;
        }
        auto md5 = UserManager::GetToken(uinfo, token_time);
        if(md5 != token) {
            SYLAR_LOG_INFO(g_logger_access) << 310 << "\t"
                << "invalid_token" << "\tauto_login" << request->getPath()
                << "\t" << (!request->getQuery().empty() ? request->getQuery() : "-");
            break;
        }
        data->setData(CookieKey::USER_ID, uid);
        is_login = true;
        SYLAR_LOG_INFO(g_logger_access) << 200 << "\t"
            << "ok" << "\tauto_login" << request->getPath()
            << "\t" << (!request->getQuery().empty() ? request->getQuery() : "-");

        uinfo->setLoginTime(time(0));
        data::UserInfoDao::Update(uinfo, getDB());
    } while(0);
    data->setData(CookieKey::IS_AUTH, (int32_t)1);
    return is_login;
}

sylar::IDB::ptr BlogServlet::getDB() {
    return GetDB();
}

}
