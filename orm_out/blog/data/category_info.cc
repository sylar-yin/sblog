#include "category_info.h"
#include "sylar/log.h"

namespace blog {
namespace data {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("orm");

CategoryInfo::CategoryInfo()
    :m_isDeleted()
    ,m_id()
    ,m_userId()
    ,m_parentId()
    ,m_name()
    ,m_createTime()
    ,m_updateTime() {
}

std::string CategoryInfo::toJsonString() const {
    Json::Value jvalue;
    jvalue["id"] = std::to_string(m_id);
    jvalue["user_id"] = std::to_string(m_userId);
    jvalue["name"] = m_name;
    jvalue["parent_id"] = std::to_string(m_parentId);
    jvalue["is_deleted"] = m_isDeleted;
    jvalue["create_time"] = sylar::Time2Str(m_createTime);
    jvalue["update_time"] = sylar::Time2Str(m_updateTime);
    return sylar::JsonUtil::ToString(jvalue);
}

void CategoryInfo::setId(const int64_t& v) {
    m_id = v;
}

void CategoryInfo::setUserId(const int64_t& v) {
    m_userId = v;
}

void CategoryInfo::setName(const std::string& v) {
    m_name = v;
}

void CategoryInfo::setParentId(const int64_t& v) {
    m_parentId = v;
}

void CategoryInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void CategoryInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void CategoryInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int CategoryInfoDao::Update(CategoryInfo::ptr info, sylar::IDB::ptr conn) {
    std::string sql = "update category set user_id = ?, name = ?, parent_id = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_userId);
    stmt->bindString(2, info->m_name);
    stmt->bindInt64(3, info->m_parentId);
    stmt->bindInt32(4, info->m_isDeleted);
    stmt->bindTime(5, info->m_createTime);
    stmt->bindTime(6, info->m_updateTime);
    stmt->bindInt64(7, info->m_id);
    return stmt->execute();
}

int CategoryInfoDao::Insert(CategoryInfo::ptr info, sylar::IDB::ptr conn) {
    std::string sql = "insert into category (user_id, name, parent_id, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_userId);
    stmt->bindString(2, info->m_name);
    stmt->bindInt64(3, info->m_parentId);
    stmt->bindInt32(4, info->m_isDeleted);
    stmt->bindTime(5, info->m_createTime);
    stmt->bindTime(6, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int CategoryInfoDao::InsertOrUpdate(CategoryInfo::ptr info, sylar::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into category (id, user_id, name, parent_id, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_userId);
    stmt->bindString(3, info->m_name);
    stmt->bindInt64(4, info->m_parentId);
    stmt->bindInt32(5, info->m_isDeleted);
    stmt->bindTime(6, info->m_createTime);
    stmt->bindTime(7, info->m_updateTime);
    return stmt->execute();
}

int CategoryInfoDao::Delete(CategoryInfo::ptr info, sylar::IDB::ptr conn) {
    std::string sql = "delete from category where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int CategoryInfoDao::DeleteById( const int64_t& id, sylar::IDB::ptr conn) {
    std::string sql = "delete from category where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int CategoryInfoDao::DeleteByUserId( const int64_t& user_id, sylar::IDB::ptr conn) {
    std::string sql = "delete from category where user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    return stmt->execute();
}

int CategoryInfoDao::DeleteByUserIdName( const int64_t& user_id,  const std::string& name, sylar::IDB::ptr conn) {
    std::string sql = "delete from category where user_id = ? and name = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    stmt->bindString(1, name);
    return stmt->execute();
}

int CategoryInfoDao::QueryAll(std::vector<CategoryInfo::ptr>& results, sylar::IDB::ptr conn) {
    std::string sql = "select id, user_id, name, parent_id, is_deleted, create_time, update_time from category";
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
        CategoryInfo::ptr v(new CategoryInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_name = rt->getString(2);
        v->m_parentId = rt->getInt64(3);
        v->m_isDeleted = rt->getInt32(4);
        v->m_createTime = rt->getTime(5);
        v->m_updateTime = rt->getTime(6);
        results.push_back(v);
    }
    return 0;
}

CategoryInfo::ptr CategoryInfoDao::Query( const int64_t& id, sylar::IDB::ptr conn) {
    std::string sql = "select id, user_id, name, parent_id, is_deleted, create_time, update_time from category where id = ?";
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
    CategoryInfo::ptr v(new CategoryInfo);
    v->m_id = rt->getInt64(0);
    v->m_userId = rt->getInt64(1);
    v->m_name = rt->getString(2);
    v->m_parentId = rt->getInt64(3);
    v->m_isDeleted = rt->getInt32(4);
    v->m_createTime = rt->getTime(5);
    v->m_updateTime = rt->getTime(6);
    return v;
}

int CategoryInfoDao::QueryByUserId(std::vector<CategoryInfo::ptr>& results,  const int64_t& user_id, sylar::IDB::ptr conn) {
    std::string sql = "select id, user_id, name, parent_id, is_deleted, create_time, update_time from category where user_id = ?";
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
        CategoryInfo::ptr v(new CategoryInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_name = rt->getString(2);
        v->m_parentId = rt->getInt64(3);
        v->m_isDeleted = rt->getInt32(4);
        v->m_createTime = rt->getTime(5);
        v->m_updateTime = rt->getTime(6);
        results.push_back(v);
    };
    return 0;
}

CategoryInfo::ptr CategoryInfoDao::QueryByUserIdName( const int64_t& user_id,  const std::string& name, sylar::IDB::ptr conn) {
    std::string sql = "select id, user_id, name, parent_id, is_deleted, create_time, update_time from category where user_id = ? and name = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindInt64(1, user_id);
    stmt->bindString(2, name);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    CategoryInfo::ptr v(new CategoryInfo);
    v->m_id = rt->getInt64(0);
    v->m_userId = rt->getInt64(1);
    v->m_name = rt->getString(2);
    v->m_parentId = rt->getInt64(3);
    v->m_isDeleted = rt->getInt32(4);
    v->m_createTime = rt->getTime(5);
    v->m_updateTime = rt->getTime(6);
    return v;
}

int CategoryInfoDao::CreateTableSQLite3(sylar::IDB::ptr conn) {
    return conn->execute("CREATE TABLE category("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "user_id INTEGER NOT NULL DEFAULT 0,"
            "name TEXT NOT NULL DEFAULT '',"
            "parent_id INTEGER NOT NULL DEFAULT 0,"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE INDEX category_user_id ON category(user_id);"
            "CREATE UNIQUE INDEX category_user_id_name ON category(user_id,name);"
            );
}

int CategoryInfoDao::CreateTableMySQL(sylar::IDB::ptr conn) {
    return conn->execute("CREATE TABLE category("
            "`id` bigint AUTO_INCREMENT COMMENT '类目id',"
            "`user_id` bigint NOT NULL DEFAULT 0 COMMENT '用户id',"
            "`name` varchar(100) NOT NULL DEFAULT '' COMMENT '名称',"
            "`parent_id` bigint NOT NULL DEFAULT 0 COMMENT '父类目id',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "KEY `category_user_id` (`user_id`),"
            "UNIQUE KEY `category_user_id_name` (`user_id`,`name`)) COMMENT='类目表'");
}
} //namespace data
} //namespace blog
