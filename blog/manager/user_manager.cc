#include "user_manager.h"
#include "blog/my_module.h"
#include "sylar/log.h"

namespace blog {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

bool UserManager::loadAll() {
    auto db = GetSQLite3();
    if(!db) {
        SYLAR_LOG_ERROR(g_logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<data::UserInfo::ptr> results;
    if(blog::data::UserInfoDao::QueryAll(results, db)) {
        SYLAR_LOG_ERROR(g_logger) << "UserManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, blog::data::UserInfo::ptr> datas;
    std::unordered_map<std::string, blog::data::UserInfo::ptr> accounts;
    std::unordered_map<std::string, blog::data::UserInfo::ptr> emails;
    std::unordered_map<std::string, blog::data::UserInfo::ptr> names;

    for(auto& i : results) {
        datas[i->getId()] = i;
        accounts[i->getAccount()] = i;
        emails[i->getEmail()] = i;
        names[i->getName()] = i;
    }

    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas.swap(datas);
    m_accounts.swap(accounts);
    m_emails.swap(emails);
    m_names.swap(names);
    return true;
}

void UserManager::add(blog::data::UserInfo::ptr info) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas[info->getId()] = info;
    m_accounts[info->getAccount()] = info;
    m_emails[info->getEmail()] = info;
    m_names[info->getName()] = info;
}

#define XX(map, key) \
    sylar::RWMutex::ReadLock lock(m_mutex); \
    auto it = map.find(key); \
    return it == map.end() ? nullptr : it->second;

blog::data::UserInfo::ptr UserManager::get(int64_t id) {
    XX(m_datas, id);
}

blog::data::UserInfo::ptr UserManager::getByAccount(const std::string& v) {
    XX(m_accounts, v);
}

blog::data::UserInfo::ptr UserManager::getByEmail(const std::string& v) {
    XX(m_emails, v);
}

blog::data::UserInfo::ptr UserManager::getByName(const std::string& v) {
    XX(m_names, v);
}

#undef XX

}
