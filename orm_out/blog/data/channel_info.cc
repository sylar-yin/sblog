#include "channel_info.h"
#include "sylar/log.h"

namespace blog {
namespace data {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("orm");

ChannelInfo::ChannelInfo()
    :m_id()
    ,m_name()
    ,m_createTime()
    ,m_updateTime() {
}

std::string ChannelInfo::toJsonString() const {
    Json::Value jvalue;
    jvalue["id"] = std::to_string(m_id);
    jvalue["name"] = m_name;
    jvalue["create_time"] = sylar::Time2Str(m_createTime);
    jvalue["update_time"] = sylar::Time2Str(m_updateTime);
    return sylar::JsonUtil::ToString(jvalue);
}

void ChannelInfo::setId(const int64_t& v) {
    m_id = v;
}

void ChannelInfo::setName(const std::string& v) {
    m_name = v;
}

void ChannelInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void ChannelInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int ChannelInfoDao::Update(ChannelInfo::ptr info, sylar::IDB::ptr conn) {
    std::string sql = "update channel set name = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_name);
    stmt->bindTime(2, info->m_createTime);
    stmt->bindTime(3, info->m_updateTime);
    stmt->bindInt64(4, info->m_id);
    return stmt->execute();
}

int ChannelInfoDao::Insert(ChannelInfo::ptr info, sylar::IDB::ptr conn) {
    std::string sql = "insert into channel (name, create_time, update_time) values (?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_name);
    stmt->bindTime(2, info->m_createTime);
    stmt->bindTime(3, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int ChannelInfoDao::InsertOrUpdate(ChannelInfo::ptr info, sylar::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into channel (id, name, create_time, update_time) values (?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindString(2, info->m_name);
    stmt->bindTime(3, info->m_createTime);
    stmt->bindTime(4, info->m_updateTime);
    return stmt->execute();
}

int ChannelInfoDao::Delete(ChannelInfo::ptr info, sylar::IDB::ptr conn) {
    std::string sql = "delete from channel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int ChannelInfoDao::DeleteById( const int64_t& id, sylar::IDB::ptr conn) {
    std::string sql = "delete from channel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int ChannelInfoDao::QueryAll(std::vector<ChannelInfo::ptr>& results, sylar::IDB::ptr conn) {
    std::string sql = "select id, name, create_time, update_time from channel";
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
        ChannelInfo::ptr v(new ChannelInfo);
        v->m_id = rt->getInt64(0);
        v->m_name = rt->getString(1);
        v->m_createTime = rt->getTime(2);
        v->m_updateTime = rt->getTime(3);
        results.push_back(v);
    }
    return 0;
}

ChannelInfo::ptr ChannelInfoDao::Query( const int64_t& id, sylar::IDB::ptr conn) {
    std::string sql = "select id, name, create_time, update_time from channel where id = ?";
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
    ChannelInfo::ptr v(new ChannelInfo);
    v->m_id = rt->getInt64(0);
    v->m_name = rt->getString(1);
    v->m_createTime = rt->getTime(2);
    v->m_updateTime = rt->getTime(3);
    return v;
}

int ChannelInfoDao::CreateTableSQLite3(sylar::IDB::ptr conn) {
    return conn->execute("CREATE TABLE channel("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL DEFAULT '',"
            "create_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            );
}

int ChannelInfoDao::CreateTableMySQL(sylar::IDB::ptr conn) {
    return conn->execute("CREATE TABLE channel("
            "`id` bigint AUTO_INCREMENT COMMENT '频道id',"
            "`name` varchar(256) NOT NULL DEFAULT '' COMMENT '频道名称',"
            "`create_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`)) COMMENT='频道'");
}
} //namespace data
} //namespace blog
