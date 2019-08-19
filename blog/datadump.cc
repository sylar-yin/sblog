#include "blog/data/article_info.h"
#include "blog/data/article_category_rel_info.h"
#include "blog/data/article_label_rel_info.h"
#include "blog/data/category_info.h"
#include "blog/data/comment_info.h"
#include "blog/data/label_info.h"
#include "blog/data/user_info.h"
#include "blog/data/channel_info.h"

#include "sylar/db/db.h"
#include "sylar/db/sqlite3.h"
#include "sylar/db/mysql.h"

#include "sylar/log.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

template<class Dao, class T>
bool trans_data(sylar::IDB::ptr from, sylar::IDB::ptr to) {
    std::vector<typename T::ptr> vals;
    if(Dao::QueryAll(vals, from)) {
        SYLAR_LOG_ERROR(g_logger) << "query_all fail type:"
            << sylar::TypeToName<Dao>() << " " << from->getErrStr()
            << " (" << from->getErrno() << ")";
        return false;
    }

    auto trans = to->openTransaction();
    if(!trans) {
        SYLAR_LOG_ERROR(g_logger) << "openTransaction fail type:"
            << sylar::TypeToName<Dao>() << " " << from->getErrStr()
            << " (" << from->getErrno() << ")";
        return false;
    }
    bool has_error = false;
    for(auto& i : vals) {
        if(Dao::InsertOrUpdate(i, to)) {
            SYLAR_LOG_ERROR(g_logger) << "InsertOrUpdate fail type:"
                << sylar::TypeToName<Dao>() << " " << from->getErrStr()
                << " (" << from->getErrno() << ") data=" << i->toJsonString();
            has_error = true;
            break;
        }
    }

    if(!has_error) {
        if(!trans->commit()) {
            SYLAR_LOG_ERROR(g_logger) << "commit fail type:"
                << sylar::TypeToName<Dao>() << " " << from->getErrStr()
                << " (" << from->getErrno() << ")";
            return false;
        }
        return true;
    }
    return false;
}

struct ConnInfo {
    std::string type;
    std::string path;
    std::map<std::string, std::string> args;
};

bool ParserString(const std::string& str, ConnInfo& info) {
    auto parts = sylar::split(str, '$');
    if(parts.size() < 2) {
        return false;
    }
    info.type = parts[0];
    if(parts[0] == "sqlite3") {
        info.path = parts[1];
        return true;
    } else if(parts[0] == "mysql") {
        auto t = sylar::split(parts[1], '&');
        for(auto& x : t) {
            auto v = sylar::split(x, '=');
            if(v.size() != 2) {
                continue;
            }
            info.args[v[0]] = v[1];
        }
        return true;
    }
    return false;
}

sylar::IDB::ptr createConn(const ConnInfo& info) {
    if(info.type == "sqlite3") {
        return sylar::SQLite3::Create(info.path);
    }
    if(info.type == "mysql") {
        auto minfo = sylar::MySQL::ptr(new sylar::MySQL(info.args));
        if(!minfo->connect()) {
            return nullptr;
        }
        return minfo;
    }
    return nullptr;
}

int main(int argc, char** argv) {
    if(argc < 3) {
        std::cout << "Use as[" << argv[0] << " uri uri2" << std::endl;
        std::cout << "  uri,uri2: 连接地址,sqlite3$/path/to/db" << std::endl;
        std::cout << "   mysql为mysql$host=xxx&user=xxx&passwd=xx&dbname=xxx" << std::endl;
        return 0;
    }
    sylar::IDB::ptr from, to;
    ConnInfo from_info, to_info;
    if(!ParserString(argv[1], from_info)) {
        std::cout << "invalid uri: " << argv[1] << std::endl;
        return 0;
    }
    if(!ParserString(argv[2], to_info)) {
        std::cout << "invalid uri: " << argv[2] << std::endl;
        return 0;
    }

    from = createConn(from_info);
    to = createConn(to_info);
    if(!from || !to) {
        return 0;
    }
#define XX(type) \
    if(std::dynamic_pointer_cast<sylar::SQLite3>(to)) { \
        if(!type ## Dao::CreateTableSQLite3(to)) { \
            SYLAR_LOG_ERROR(g_logger) << to->getErrStr() << "(" << to->getErrno() << ")"; \
        } \
    } else {\
        if(!type ## Dao::CreateTableMySQL(to)) { \
            SYLAR_LOG_ERROR(g_logger) << to->getErrStr() << "(" << to->getErrno() << ")"; \
        } \
    } \
    if(!trans_data<type ## Dao, type>(from, to)) { \
        SYLAR_LOG_ERROR(g_logger) << "trans_data fail " #type; \
    }
    XX(blog::data::ArticleInfo);
    XX(blog::data::ArticleCategoryRelInfo);
    XX(blog::data::ArticleLabelRelInfo);
    XX(blog::data::CategoryInfo);
    XX(blog::data::CommentInfo);
    XX(blog::data::LabelInfo);
    XX(blog::data::UserInfo);
    XX(blog::data::ChannelInfo);
#undef XX
    return 0;
}
