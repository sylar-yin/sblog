#include "my_module.h"
#include "sylar/config.h"
#include "sylar/log.h"
#include "blog/data/user_info.h"
#include "sylar/application.h"
#include "blog/servlets/user_create_servlet.h"
#include "blog/manager/user_manager.h"

namespace blog {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
static sylar::ConfigVar<std::string>::ptr g_sqlite3_db_name =
    sylar::Config::Lookup("sqlite3.db_name",
            std::string("blog.db"), "sqlite3 db file name");

MyModule::MyModule()
    :sylar::Module("sblog", "1.0", "") {
}

bool MyModule::onLoad() {
    SYLAR_LOG_INFO(g_logger) << "onLoad";
    return true;
}

bool MyModule::onUnload() {
    SYLAR_LOG_INFO(g_logger) << "onUnload";
    return true;
}

int32_t handle_request(sylar::http::HttpRequest::ptr request
                       , sylar::http::HttpResponse::ptr response
                       , sylar::http::HttpSession::ptr session) {
    SYLAR_LOG_INFO(g_logger) << *request;
    response->setBody("ok");
    return 0;
}


bool MyModule::onServerReady() {
    SYLAR_LOG_INFO(g_logger) << "onServerReady";

    auto work_path = sylar::Config::Lookup<std::string>("server.work_path");
    auto db_path = work_path->getValue() + "/" + g_sqlite3_db_name->getValue();
    sylar::SQLite3::ptr db;
    db = sylar::SQLite3::Create(db_path, sylar::SQLite3::READWRITE);
    if(!db) {
        SYLAR_LOG_INFO(g_logger) << "init database begin";
        db = sylar::SQLite3::Create(db_path);
        if(!db) {
            SYLAR_LOG_ERROR(g_logger) << "open database db=" << db_path
                << " failed";
            return false;
        }
        if(blog::data::UserInfoDao::CreateTable(db)) {
            SYLAR_LOG_ERROR(g_logger) << "create table user failed";
            return false;
        }
        SYLAR_LOG_INFO(g_logger) << "init database end";
    }

    std::vector<sylar::TcpServer::ptr> servers;
    if(!sylar::Application::GetInstance()->getServer("http", servers)) {
        SYLAR_LOG_ERROR(g_logger) << "http_server not open";
        return false;
    }

    if(!UserMgr::GetInstance()->loadAll()) {
        SYLAR_LOG_ERROR(g_logger) << "user load all fail";
    }

    for(auto& i : servers) {
        auto hs = std::dynamic_pointer_cast<sylar::http::HttpServer>(i);
        auto dp = hs->getServletDispatch();

#define XX(clazz) sylar::http::Servlet::ptr(new servlet::clazz)

        dp->addServlet("/user/create", XX(UserCreateServlet));
        dp->addServlet("/user/active", handle_request);
        dp->addServlet("/user/login", handle_request);
        dp->addServlet("/user/update", handle_request);
        dp->addServlet("/user/exists", handle_request);
    }
    return true;
}

bool MyModule::onServerUp() {
    SYLAR_LOG_INFO(g_logger) << "onServerUp";
    return true;
}

sylar::SQLite3::ptr GetSQLite3() {
    auto work_path = sylar::Config::Lookup<std::string>("server.work_path");
    auto db_path = work_path->getValue() + "/" + g_sqlite3_db_name->getValue();
    auto db = sylar::SQLite3::Create(db_path);
    return db;
}

}

extern "C" {

sylar::Module* CreateModule() {
    sylar::Module* module = new blog::MyModule;
    SYLAR_LOG_INFO(blog::g_logger) << "CreateModule " << module;
    return module;
}

void DestoryModule(sylar::Module* module) {
    SYLAR_LOG_INFO(blog::g_logger) << "CreateModule " << module;
    delete module;
}

}
