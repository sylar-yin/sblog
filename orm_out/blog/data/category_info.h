#ifndef __BLOG_DATACATEGORY_INFO_H__
#define __BLOG_DATACATEGORY_INFO_H__

#include <json/json.h>
#include <vector>
#include "sylar/db/db.h"
#include "sylar/util.h"


namespace blog {
namespace data {

class CategoryInfoDao;
class CategoryInfo {
friend class CategoryInfoDao;
public:
    typedef std::shared_ptr<CategoryInfo> ptr;

    CategoryInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const int64_t& getUserId() { return m_userId; }
    void setUserId(const int64_t& v);

    const std::string& getName() { return m_name; }
    void setName(const std::string& v);

    const int64_t& getParentId() { return m_parentId; }
    void setParentId(const int64_t& v);

    const int32_t& getIsDeleted() { return m_isDeleted; }
    void setIsDeleted(const int32_t& v);

    const int64_t& getCreateTime() { return m_createTime; }
    void setCreateTime(const int64_t& v);

    const int64_t& getUpdateTime() { return m_updateTime; }
    void setUpdateTime(const int64_t& v);

    std::string toJsonString() const;

private:
    int32_t m_isDeleted;
    int64_t m_id;
    int64_t m_userId;
    int64_t m_parentId;
    std::string m_name;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class CategoryInfoDao {
public:
    typedef std::shared_ptr<CategoryInfoDao> ptr;
    static int Update(CategoryInfo::ptr info, sylar::IDB::ptr conn);
    static int Insert(CategoryInfo::ptr info, sylar::IDB::ptr conn);
    static int InsertOrUpdate(CategoryInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(CategoryInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(const int64_t& id, sylar::IDB::ptr conn);
    static int DeleteById( const int64_t& id, sylar::IDB::ptr conn);
    static int DeleteByUserId( const int64_t& user_id, sylar::IDB::ptr conn);
    static int DeleteByUserIdName( const int64_t& user_id,  const std::string& name, sylar::IDB::ptr conn);
    static int QueryAll(std::vector<CategoryInfo::ptr>& results, sylar::IDB::ptr conn);
    static CategoryInfo::ptr Query( const int64_t& id, sylar::IDB::ptr conn);
    static int QueryByUserId(std::vector<CategoryInfo::ptr>& results,  const int64_t& user_id, sylar::IDB::ptr conn);
    static CategoryInfo::ptr QueryByUserIdName( const int64_t& user_id,  const std::string& name, sylar::IDB::ptr conn);
    static int CreateTableSQLite3(sylar::IDB::ptr info);
    static int CreateTableMySQL(sylar::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //__BLOG_DATACATEGORY_INFO_H__
