#include "article_manager.h"
#include "sylar/log.h"
#include "sylar/util.h"
#include "blog/util.h"

namespace blog {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

bool ArticleManager::loadAll() {
    auto db = GetDB();
    if(!db) {
        SYLAR_LOG_ERROR(g_logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<data::ArticleInfo::ptr> results;
    if(blog::data::ArticleInfoDao::QueryAll(results, db)) {
        SYLAR_LOG_ERROR(g_logger) << "ArticleManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, blog::data::ArticleInfo::ptr> datas;
    std::unordered_map<int64_t, blog::data::ArticleInfo::ptr> users;

    for(auto& i : results) {
        datas[i->getId()] = i;
        users[i->getUserId()] = i;
    }

    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas.swap(datas);
    m_users.swap(users);
    return true;
}

void ArticleManager::add(blog::data::ArticleInfo::ptr info) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas[info->getId()] = info;
    m_users[info->getUserId()] = info;
}

#define XX(map, key) \
    sylar::RWMutex::ReadLock lock(m_mutex); \
    auto it = map.find(key); \
    return it == map.end() ? nullptr : it->second;

blog::data::ArticleInfo::ptr ArticleManager::get(int64_t id) {
    XX(m_datas, id);
}

blog::data::ArticleInfo::ptr ArticleManager::getByUserId(int64_t id) {
    XX(m_users, id);
}

#undef XX

}
