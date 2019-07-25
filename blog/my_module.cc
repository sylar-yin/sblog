#include "my_module.h"
#include "sylar/config.h"
#include "sylar/log.h"
#include "blog/data/user_info.h"
#include "sylar/application.h"
#include "blog/servlets/article_create_servlet.h"
#include "blog/servlets/article_delete_servlet.h"
#include "blog/servlets/article_detail_servlet.h"
#include "blog/servlets/article_query_servlet.h"
#include "blog/servlets/article_publish_servlet.h"
#include "blog/servlets/article_update_category_servlet.h"
#include "blog/servlets/article_update_label_servlet.h"
#include "blog/servlets/article_verify_list_servlet.h"
#include "blog/servlets/article_verify_servlet.h"
#include "blog/servlets/category_create_servlet.h"
#include "blog/servlets/category_delete_servlet.h"
#include "blog/servlets/category_query_servlet.h"
#include "blog/servlets/comment_create_servlet.h"
#include "blog/servlets/comment_delete_servlet.h"
#include "blog/servlets/comment_detail_servlet.h"
#include "blog/servlets/comment_verify_servlet.h"
#include "blog/servlets/comment_verify_list_servlet.h"
#include "blog/servlets/comment_query_servlet.h"
#include "blog/servlets/label_create_servlet.h"
#include "blog/servlets/label_delete_servlet.h"
#include "blog/servlets/label_query_servlet.h"
#include "blog/servlets/user_active_servlet.h"
#include "blog/servlets/user_create_servlet.h"
#include "blog/servlets/user_change_passwd_servlet.h"
#include "blog/servlets/user_exists_servlet.h"
#include "blog/servlets/user_forget_passwd_servlet.h"
#include "blog/servlets/user_info_servlet.h"
#include "blog/servlets/user_login_servlet.h"
#include "blog/servlets/user_logout_servlet.h"
#include "blog/servlets/user_query_servlet.h"
#include "blog/servlets/user_update_servlet.h"
#include "blog/manager/article_category_rel_manager.h"
#include "blog/manager/article_label_rel_manager.h"
#include "blog/manager/article_manager.h"
#include "blog/manager/category_manager.h"
#include "blog/manager/comment_manager.h"
#include "blog/manager/label_manager.h"
#include "blog/manager/user_manager.h"
#include "blog/util.h"

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
#define XX(clazz, t) \
        if(blog::data::clazz::CreateTableSQLite3(db)) { \
            SYLAR_LOG_ERROR(g_logger) << "create table " t " failed"; \
            return false; \
        }
        XX(UserInfoDao, "user");
        XX(ArticleInfoDao, "article");
        XX(ArticleCategoryRelInfoDao, "article_category_rel");
        XX(ArticleLabelRelInfoDao, "article_label_rel");
        XX(CategoryInfoDao, "category");
        XX(LabelInfoDao, "label");
        XX(CommentInfoDao, "comment");
#undef XX
        SYLAR_LOG_INFO(g_logger) << "init database end";
    }

    std::vector<sylar::TcpServer::ptr> servers;
    if(!sylar::Application::GetInstance()->getServer("http", servers)) {
        SYLAR_LOG_ERROR(g_logger) << "http_server not open";
        return false;
    }

#define XX(clazz) \
    if(!clazz::GetInstance()->loadAll()) { \
        SYLAR_LOG_ERROR(g_logger) << #clazz " load all fail"; \
    }
    XX(UserMgr);
    XX(ArticleMgr);
    XX(ArticleCategoryRelMgr);
    XX(ArticleLabelRelMgr);
    XX(CategoryMgr);
    XX(LabelMgr);
    XX(CommentMgr);
#undef XX

    for(auto& i : servers) {
        auto hs = std::dynamic_pointer_cast<sylar::http::HttpServer>(i);
        auto dp = hs->getServletDispatch();

//#define XX(clazz) sylar::http::Servlet::ptr(new servlet::clazz)
#define XX(uri, clazz) dp->addServletCreator<servlet::clazz>(uri)
        //user system
        XX("/user/create", UserCreateServlet);
        XX("/user/active", UserActiveServlet);
        XX("/user/login", UserLoginServlet);
        XX("/user/logout", UserLogoutServlet);
        XX("/user/exists", UserExistsServlet);
        XX("/user/info", UserInfoServlet);
        XX("/user/update", UserUpdateServlet);
        XX("/user/forget_passwd", UserForgetPasswdServlet);
        XX("/user/change_passwd", UserChangePasswdServlet);
        XX("/user/query", UserQueryServlet);

        //aritcle system
        XX("/category/create", CategoryCreateServlet);
        XX("/category/delete", CategoryDeleteServlet);
        XX("/category/query",  CategoryQueryServlet);

        XX("/label/create", LabelCreateServlet);
        XX("/label/delete", LabelDeleteServlet);
        XX("/label/query",  LabelQueryServlet);

        XX("/article/create", ArticleCreateServlet);
        XX("/article/delete", ArticleDeleteServlet);
        XX("/article/query",  ArticleQueryServlet);
        XX("/article/detail",  ArticleDetailServlet);

        XX("/article/update_category",  ArticleUpdateCategoryServlet);
        XX("/article/update_label",  ArticleUpdateLabelServlet);
        XX("/article/publish",  ArticlePublishServlet);

        XX("/article/verify",  ArticleVerifyServlet);
        XX("/article/verify_list",  ArticleVerifyListServlet);


        XX("/comment/create",       CommentCreateServlet);
        XX("/comment/delete",       CommentDeleteServlet);
        XX("/comment/detail",       CommentDetailServlet);
        XX("/comment/verify",       CommentVerifyServlet);
        XX("/comment/verify_list",  CommentVerifyListServlet);
        XX("/comment/query",        CommentQueryServlet);
    }
    return true;
}

bool MyModule::onServerUp() {
    SYLAR_LOG_INFO(g_logger) << "onServerUp";
    return true;
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
