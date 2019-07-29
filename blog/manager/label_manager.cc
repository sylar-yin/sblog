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
    std::unordered_map<int64_t, std::map<std::string, blog::data::LabelInfo::ptr> > users;

    for(auto& i : results) {
        datas[i->getId()] = i;
        users[i->getUserId()][i->getName()] = i;
    }

    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas.swap(datas);
    m_users.swap(users);
    return true;
}

void LabelManager::add(blog::data::LabelInfo::ptr info) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas[info->getId()] = info;
    m_users[info->getUserId()][info->getName()] = info;
}

#define XX(map, key) \
    sylar::RWMutex::ReadLock lock(m_mutex); \
    auto it = map.find(key); \
    return it == map.end() ? nullptr : it->second;

blog::data::LabelInfo::ptr LabelManager::get(int64_t id) {
    XX(m_datas, id);
}

blog::data::LabelInfo::ptr LabelManager::getByUserIdName(int64_t id, const std::string& name) {
    sylar::RWMutex::ReadLock lock(m_mutex);
    auto it = m_users.find(id);
    if(it == m_users.end()) {
        return nullptr;
    }
    auto iit = it->second.find(name);
    return iit == it->second.end() ? nullptr : iit->second;
}

bool LabelManager::listByUserId(std::vector<data::LabelInfo::ptr>& infos, int64_t id, bool valid) {
    sylar::RWMutex::ReadLock lock(m_mutex);
    auto it = m_users.find(id);
    if(it == m_users.end()) {
        return false;
    }
    if(valid) {
        for(auto& i : it->second) {
            if(i.second->getIsDeleted() == 0) {
                infos.push_back(i.second);
            }
        }
    } else {
        for(auto& i : it->second) {
            infos.push_back(i.second);
        }
    }
    return true;
}

std::string LabelManager::statusString() {
    std::stringstream ss;
    sylar::RWMutex::ReadLock lock(m_mutex);
    ss << "LabelManager total=" << m_datas.size()
       << " users=" << m_users.size()
       << std::endl;
    lock.unlock();
    return ss.str();
}

#undef XX

}
