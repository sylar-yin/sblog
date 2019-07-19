#ifndef __BLOG_MANAGER_LABEL_MANAGER_H__
#define __BLOG_MANAGER_LABEL_MANAGER_H__

#include "blog/data/label_info.h"
#include "sylar/singleton.h"
#include "sylar/mutex.h"
#include <map>
#include <unordered_map>

namespace blog {

class LabelManager {
public:
    bool loadAll();
    void add(blog::data::LabelInfo::ptr info);
    blog::data::LabelInfo::ptr get(int64_t id);
    blog::data::LabelInfo::ptr getByUserIdName(int64_t id, const std::string& name);
    bool listByUserId(std::vector<data::LabelInfo::ptr>& infos, int64_t id, bool valid);
private:
    sylar::RWMutex m_mutex;
    std::unordered_map<int64_t, blog::data::LabelInfo::ptr> m_datas;
    std::unordered_map<int64_t, std::map<std::string, blog::data::LabelInfo::ptr> > m_users;
};

typedef sylar::Singleton<LabelManager> LabelMgr;

}

#endif
