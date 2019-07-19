#include "category_manager.h"
#include "sylar/log.h"
#include "sylar/util.h"
#include "blog/util.h"

namespace blog {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

bool CategoryManager::loadAll() {
    auto db = GetDB();
    if(!db) {
        SYLAR_LOG_ERROR(g_logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<data::CategoryInfo::ptr> results;
    if(blog::data::CategoryInfoDao::QueryAll(results, db)) {
        SYLAR_LOG_ERROR(g_logger) << "CategoryManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, blog::data::CategoryInfo::ptr> datas;
    std::unordered_map<int64_t, blog::data::CategoryInfo::ptr> users;

    for(auto& i : results) {
        datas[i->getId()] = i;
        users[i->getUserId()] = i;
    }

    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas.swap(datas);
    m_users.swap(users);
    return true;
}

void CategoryManager::add(blog::data::CategoryInfo::ptr info) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas[info->getId()] = info;
    m_users[info->getUserId()] = info;
}

#define XX(map, key) \
    sylar::RWMutex::ReadLock lock(m_mutex); \
    auto it = map.find(key); \
    return it == map.end() ? nullptr : it->second;

blog::data::CategoryInfo::ptr CategoryManager::get(int64_t id) {
    XX(m_datas, id);
}

blog::data::CategoryInfo::ptr CategoryManager::getByUserId(int64_t id) {
    XX(m_users, id);
}

#undef XX

}
