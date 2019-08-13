#include "user_create_servlet.h"
#include "sylar/log.h"
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

UserCreateServlet::UserCreateServlet()
    :BlogServlet("UserCreate") {
}

int32_t UserCreateServlet::handle(sylar::http::HttpRequest::ptr request
                                  ,sylar::http::HttpResponse::ptr response
                                  ,sylar::http::HttpSession::ptr session
                                  ,Result::ptr result) {
//account | string | 是 | 账号
//email | string | 是 | 邮箱
//passwd | string | 是 | 密码（MD5）
    do {
        DEFINE_AND_CHECK_STRING(result, account, "account");
        DEFINE_AND_CHECK_STRING(result, email, "email");
        DEFINE_AND_CHECK_STRING(result, passwd, "passwd");

        if(UserMgr::GetInstance()->getByAccount(account)) {
            result->setResult(401, "account exists");
            break;
        }
        if(UserMgr::GetInstance()->getByEmail(email)) {
            result->setResult(401, "email exists");
            break;
        }

        if(!is_email(email)) {
            result->setResult(402, "invalid email format");
            break;
        }
        if(!is_valid_account(account)) {
            result->setResult(402, "invalid account");
            break;
        }

        auto db = getDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }
        if(!db) {
            result->setResult(500, "get db connection fail");
            break;
        }
        sylar::ITransaction::ptr trans = db->openTransaction();
        std::string v = sylar::random_string(16);
        SYLAR_LOG_INFO(g_logger) << "[" << v << "]";
        for(auto& i : v) {
            SYLAR_ASSERT(i);
        }
        data::UserInfo::ptr info(new data::UserInfo);
        info->setAccount(account);
        info->setEmail(email);
        info->setPasswd(passwd);
        info->setState((int)State::VERIFYING);
        info->setName(account);
        info->setCode(v);

        if(data::UserInfoDao::Insert(info, db)) {
            result->setResult(500, "insert user fail");
            //TODO log
            break;
        }
        std::string send_email = sylar::EnvMgr::GetInstance()->getEnv("blog_email");
        std::string send_email_passwd = sylar::EnvMgr::GetInstance()->getEnv("blog_email_passwd");
        auto mail = sylar::EMail::Create(
                send_email, send_email_passwd
                ,"SBlog Create Account Auth - 验证码"
                ,"验证码[" + v + "]"
                ,{email}, {}, {send_email});

        auto client = sylar::SmtpClient::Create("smtp.163.com", 465, true);
        if(!client) {
            SYLAR_LOG_ERROR(g_logger) << "connect email server fail";
            result->setResult(501, "connect email server fail");
            break;
        }

        auto r = client->send(mail, 5000);
        if(r->result != 0) {
            result->setResult(501, std::to_string(r->result) + " " + r->msg);
            SendWX("blog", "邮件发送失败: " + std::to_string(r->result) + " " + r->msg + " email:" + email);
            break;
        }

        trans->commit();
        UserMgr::GetInstance()->add(info);
        SYLAR_LOG_INFO(g_logger) << info->toJsonString();
    } while(false);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}
