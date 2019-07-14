#ifndef __BLOG_MANAGER_USER_MANAGER_H__
#define __BLOG_MANAGER_USER_MANAGER_H__

#include "blog/data/user_info.h"
#include "sylar/singleton.h"
#include "sylar/mutex.h"
#include <unordered_map>

namespace blog {

class UserManager {
public:
    bool loadAll();
    void add(blog::data::UserInfo::ptr info);
    blog::data::UserInfo::ptr get(int64_t id);
    blog::data::UserInfo::ptr getByAccount(const std::string& v);
    blog::data::UserInfo::ptr getByEmail(const std::string& v);
    blog::data::UserInfo::ptr getByName(const std::string& v);
private:
    sylar::RWMutex m_mutex;
    std::unordered_map<int64_t, blog::data::UserInfo::ptr> m_datas;
    std::unordered_map<std::string, blog::data::UserInfo::ptr> m_accounts;
    std::unordered_map<std::string, blog::data::UserInfo::ptr> m_emails;
    std::unordered_map<std::string, blog::data::UserInfo::ptr> m_names;
};

typedef sylar::Singleton<UserManager> UserMgr;

}

#endif
