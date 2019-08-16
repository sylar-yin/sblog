#include "comment_info.h"
#include "sylar/log.h"

namespace blog {
namespace data {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("orm");

CommentInfo::CommentInfo()
    :m_state()
    ,m_isDeleted()
    ,m_id()
    ,m_userId()
    ,m_articleId()
    ,m_parentId()
    ,m_content()
    ,m_createTime()
    ,m_updateTime() {
}

std::string CommentInfo::toJsonString() const {
    Json::Value jvalue;
    jvalue["id"] = std::to_string(m_id);
    jvalue["user_id"] = std::to_string(m_userId);
    jvalue["article_id"] = std::to_string(m_articleId);
    jvalue["content"] = m_content;
    jvalue["parent_id"] = std::to_string(m_parentId);
    jvalue["state"] = m_state;
    jvalue["is_deleted"] = m_isDeleted;
    jvalue["create_time"] = sylar::Time2Str(m_createTime);
    jvalue["update_time"] = sylar::Time2Str(m_updateTime);
    return sylar::JsonUtil::ToString(jvalue);
}

void CommentInfo::setId(const int64_t& v) {
    m_id = v;
}

void CommentInfo::setUserId(const int64_t& v) {
    m_userId = v;
}

void CommentInfo::setArticleId(const int64_t& v) {
    m_articleId = v;
}

void CommentInfo::setContent(const std::string& v) {
    m_content = v;
}

void CommentInfo::setParentId(const int64_t& v) {
    m_parentId = v;
}

void CommentInfo::setState(const int32_t& v) {
    m_state = v;
}

void CommentInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void CommentInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void CommentInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int CommentInfoDao::Update(CommentInfo::ptr info, sylar::IDB::ptr conn) {
    std::string sql = "update comment set user_id = ?, article_id = ?, content = ?, parent_id = ?, state = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_userId);
    stmt->bindInt64(2, info->m_articleId);
    stmt->bindString(3, info->m_content);
    stmt->bindInt64(4, info->m_parentId);
    stmt->bindInt32(5, info->m_state);
    stmt->bindInt32(6, info->m_isDeleted);
    stmt->bindTime(7, info->m_createTime);
    stmt->bindTime(8, info->m_updateTime);
    stmt->bindInt64(9, info->m_id);
    return stmt->execute();
}

int CommentInfoDao::Insert(CommentInfo::ptr info, sylar::IDB::ptr conn) {
    std::string sql = "insert into comment (user_id, article_id, content, parent_id, state, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_userId);
    stmt->bindInt64(2, info->m_articleId);
    stmt->bindString(3, info->m_content);
    stmt->bindInt64(4, info->m_parentId);
    stmt->bindInt32(5, info->m_state);
    stmt->bindInt32(6, info->m_isDeleted);
    stmt->bindTime(7, info->m_createTime);
    stmt->bindTime(8, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int CommentInfoDao::InsertOrUpdate(CommentInfo::ptr info, sylar::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into comment (id, user_id, article_id, content, parent_id, state, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_userId);
    stmt->bindInt64(3, info->m_articleId);
    stmt->bindString(4, info->m_content);
    stmt->bindInt64(5, info->m_parentId);
    stmt->bindInt32(6, info->m_state);
    stmt->bindInt32(7, info->m_isDeleted);
    stmt->bindTime(8, info->m_createTime);
    stmt->bindTime(9, info->m_updateTime);
    return stmt->execute();
}

int CommentInfoDao::Delete(CommentInfo::ptr info, sylar::IDB::ptr conn) {
    std::string sql = "delete from comment where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int CommentInfoDao::DeleteById( const int64_t& id, sylar::IDB::ptr conn) {
    std::string sql = "delete from comment where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int CommentInfoDao::DeleteByUserId( const int64_t& user_id, sylar::IDB::ptr conn) {
    std::string sql = "delete from comment where user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    return stmt->execute();
}

int CommentInfoDao::DeleteByArticleId( const int64_t& article_id, sylar::IDB::ptr conn) {
    std::string sql = "delete from comment where article_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, article_id);
    return stmt->execute();
}

int CommentInfoDao::QueryAll(std::vector<CommentInfo::ptr>& results, sylar::IDB::ptr conn) {
    std::string sql = "select id, user_id, article_id, content, parent_id, state, is_deleted, create_time, update_time from comment";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    auto rt = stmt->query();
    if(!rt) {
        return stmt->getErrno();
    }
    while (rt->next()) {
        CommentInfo::ptr v(new CommentInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_articleId = rt->getInt64(2);
        v->m_content = rt->getString(3);
        v->m_parentId = rt->getInt64(4);
        v->m_state = rt->getInt32(5);
        v->m_isDeleted = rt->getInt32(6);
        v->m_createTime = rt->getTime(7);
        v->m_updateTime = rt->getTime(8);
        results.push_back(v);
    }
    return 0;
}

CommentInfo::ptr CommentInfoDao::Query( const int64_t& id, sylar::IDB::ptr conn) {
    std::string sql = "select id, user_id, article_id, content, parent_id, state, is_deleted, create_time, update_time from comment where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindInt64(1, id);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    CommentInfo::ptr v(new CommentInfo);
    v->m_id = rt->getInt64(0);
    v->m_userId = rt->getInt64(1);
    v->m_articleId = rt->getInt64(2);
    v->m_content = rt->getString(3);
    v->m_parentId = rt->getInt64(4);
    v->m_state = rt->getInt32(5);
    v->m_isDeleted = rt->getInt32(6);
    v->m_createTime = rt->getTime(7);
    v->m_updateTime = rt->getTime(8);
    return v;
}

int CommentInfoDao::QueryByUserId(std::vector<CommentInfo::ptr>& results,  const int64_t& user_id, sylar::IDB::ptr conn) {
    std::string sql = "select id, user_id, article_id, content, parent_id, state, is_deleted, create_time, update_time from comment where user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        CommentInfo::ptr v(new CommentInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_articleId = rt->getInt64(2);
        v->m_content = rt->getString(3);
        v->m_parentId = rt->getInt64(4);
        v->m_state = rt->getInt32(5);
        v->m_isDeleted = rt->getInt32(6);
        v->m_createTime = rt->getTime(7);
        v->m_updateTime = rt->getTime(8);
        results.push_back(v);
    };
    return 0;
}

int CommentInfoDao::QueryByArticleId(std::vector<CommentInfo::ptr>& results,  const int64_t& article_id, sylar::IDB::ptr conn) {
    std::string sql = "select id, user_id, article_id, content, parent_id, state, is_deleted, create_time, update_time from comment where article_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, article_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        CommentInfo::ptr v(new CommentInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_articleId = rt->getInt64(2);
        v->m_content = rt->getString(3);
        v->m_parentId = rt->getInt64(4);
        v->m_state = rt->getInt32(5);
        v->m_isDeleted = rt->getInt32(6);
        v->m_createTime = rt->getTime(7);
        v->m_updateTime = rt->getTime(8);
        results.push_back(v);
    };
    return 0;
}

int CommentInfoDao::CreateTableSQLite3(sylar::IDB::ptr conn) {
    return conn->execute("CREATE TABLE comment(id INTEGER PRIMARY KEY AUTOINCREMENT, user_id INTEGER NOT NULL DEFAULT 0, article_id INTEGER NOT NULL DEFAULT 0, content TEXT NOT NULL DEFAULT '', parent_id INTEGER NOT NULL DEFAULT 0, state INTEGER NOT NULL DEFAULT 0, is_deleted INTEGER NOT NULL DEFAULT 0, create_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00', update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');CREATE INDEX comment_user_id ON comment(user_id);CREATE INDEX comment_article_id ON comment(article_id);");
}int CommentInfoDao::CreateTableMySQL(sylar::IDB::ptr conn) {
    return conn->execute("CREATE TABLE comment(id bigint AUTO_INCREMENT, user_id bigint NOT NULL DEFAULT 0, article_id bigint NOT NULL DEFAULT 0, content varchar(300) NOT NULL DEFAULT '', parent_id bigint NOT NULL DEFAULT 0, state int NOT NULL DEFAULT 0, is_deleted int NOT NULL DEFAULT 0, create_time timestamp NOT NULL DEFAULT '1980-01-01 00:00:00', update_time timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp , PRIMARY KEY(id), KEY comment_user_id (user_id), KEY comment_article_id (article_id))");
}
} //namespace data
} //namespace blog
