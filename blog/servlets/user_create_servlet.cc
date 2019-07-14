#include "user_create_servlet.h"
#include "sylar/log.h"
#include "blog/manager/user_manager.h"
#include "sylar/util.h"
#include "blog/my_module.h"
#include "sylar/email/smtp.h"
#include "sylar/env.h"
#include <regex>

namespace blog {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

UserCreateServlet::UserCreateServlet()
    :sylar::http::Servlet("UserCreate") {
}

bool is_email(const std::string& str) {
    static const std::regex s_email_regex("[0-9A-Za-z\\-_\\.]+@[0-9a-z]+\\.[a-z]{2,8}\\.[a-z]{2,8}?");
    return std::regex_match(str, s_email_regex);
}

bool is_valid_account(const std::string& str) {
    static const std::regex s_account_regex("[A-Za-z][0-9A-Za-z\\-_\\.]{4, 15}");
    return std::regex_match(str, s_account_regex);
}

int32_t UserCreateServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session) {
//account | string | 是 | 账号
//email | string | 是 | 邮箱
//passwd | string | 是 | 密码（MD5）
    int code = 200;
    std::string msg = "ok";
    do {
        auto account = request->getParam("account");
        if(account.empty()) {
            code = 400;
            msg = "param account is null";
            break;
        }
        auto email = request->getParam("email");
        if(email.empty()) {
            code = 400;
            msg = "param email is null";
            break;
        }
        auto passwd = request->getParam("passwd");
        if(passwd.empty()) {
            code = 400;
            msg = "param passwd is null";
            break;
        }

        if(UserMgr::GetInstance()->getByAccount(account)) {
            code = 401;
            msg = "account exists";
            break;
        }
        if(UserMgr::GetInstance()->getByEmail(email)) {
            code = 401;
            msg = "email exists";
            break;
        }

        auto db = GetSQLite3();
        if(!db) {
            code = 500;
            msg = "get db connection fail";
            break;
        }
        sylar::SQLite3Transaction trans(db);
        trans.begin();

        //TODO 验证账号是否合法，验证邮箱是否合法

        std::string v = sylar::random_string(16);
        data::UserInfo::ptr info(new data::UserInfo);
        info->setAccount(account);
        info->setEmail(email);
        info->setPasswd(passwd);
        info->setState(1);
        info->setName(account);
        info->setCode(v);

        if(data::UserInfoDao::Insert(info, db)) {
            code = 500;
            msg = "insert user fail";
            break;
        }
        auto mail = sylar::EMail::Create(
                sylar::EnvMgr::GetInstance()->getEnv("blog_email")
                ,sylar::EnvMgr::GetInstance()->getEnv("blog_email_passwd")
                ,"SBlog Create Account Auth"
                ,"Auth Code[" + v + "]"
                ,{email}, {}, {"ydj564628276@163.com"});

        auto client = sylar::SmtpClient::Create("smtp.163.com", 25);
        if(!client) {
            SYLAR_LOG_ERROR(g_logger) << "connect email server fail";
            code = 501;
            msg = "connect email server fail";
            break;
        }

        auto r = client->send(mail);
        if(r->result != 0) {
            code = 501;
            msg = std::to_string(r->result) + " " + r->msg;
            break;
        }

        trans.commit();
        UserMgr::GetInstance()->add(info);
        SYLAR_LOG_INFO(g_logger) << info->toJsonString();
    } while(false);
    
    Json::Value v;
    v["code"] = std::to_string(code);
    v["msg"] = msg;
    response->setBody(sylar::JsonUtil::ToString(v));
    
    SYLAR_LOG_INFO(g_logger) << "UserCreateServlet: " << *request
        << " - " << *response;
    return 0;
}

}
}
