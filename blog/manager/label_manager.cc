#include "label_manager.h"
#include "sylar/log.h"
#include "sylar/util.h"
#include "blog/util.h"

namespace blog {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

bool LabelManager::loadAll() {
    auto db = GetDB();
    if(!db) {
        SYLAR_LOG_ERROR(g_logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<data::LabelInfo::ptr> results;
    if(blog::data::LabelInfoDao::QueryAll(results, db)) {
        SYLAR_LOG_ERROR(g_logger) << "LabelManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, blog::data::LabelInfo::ptr> datas;
    std::unordered_map<int64_t, blog::data::LabelInfo::ptr> users;

    for(auto& i : results) {
        datas[i->getId()] = i;
        users[i->getUserId()] = i;
    }

    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas.swap(datas);
    m_users.swap(users);
    return true;
}

void LabelManager::add(blog::data::LabelInfo::ptr info) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas[info->getId()] = info;
    m_users[info->getUserId()] = info;
}

#define XX(map, key) \
    sylar::RWMutex::ReadLock lock(m_mutex); \
    auto it = map.find(key); \
    return it == map.end() ? nullptr : it->second;

blog::data::LabelInfo::ptr LabelManager::get(int64_t id) {
    XX(m_datas, id);
}

blog::data::LabelInfo::ptr LabelManager::getByUserId(int64_t id) {
    XX(m_users, id);
}

#undef XX

}
