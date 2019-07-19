#ifndef __BLOG_MANAGER_CATEGORY_MANAGER_H__
#define __BLOG_MANAGER_CATEGORY_MANAGER_H__

#include "blog/data/category_info.h"
#include "sylar/singleton.h"
#include "sylar/mutex.h"
#include <map>
#include <unordered_map>

namespace blog {

class CategoryManager {
public:
    bool loadAll();
    void add(blog::data::CategoryInfo::ptr info);
    blog::data::CategoryInfo::ptr get(int64_t id);
    bool listByUserId(std::vector<blog::data::CategoryInfo::ptr>& infos, int64_t id, bool valid);
    blog::data::CategoryInfo::ptr getByUserIdName(int64_t id, const std::string& name);
    bool exists(int64_t id, const std::string& name);
private:
    sylar::RWMutex m_mutex;
    std::unordered_map<int64_t, blog::data::CategoryInfo::ptr> m_datas;
    std::unordered_map<int64_t, std::map<std::string, blog::data::CategoryInfo::ptr> > m_users;
};

typedef sylar::Singleton<CategoryManager> CategoryMgr;

}

#endif
