#ifndef __BLOG_DATALABEL_INFO_H__
#define __BLOG_DATALABEL_INFO_H__

#include <json/json.h>
#include <vector>
#include "sylar/db/db.h"
#include "sylar/util.h"


namespace blog {
namespace data {

class LabelInfoDao;
class LabelInfo {
friend class LabelInfoDao;
public:
    typedef std::shared_ptr<LabelInfo> ptr;

    LabelInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const int64_t& getUserId() { return m_userId; }
    void setUserId(const int64_t& v);

    const std::string& getName() { return m_name; }
    void setName(const std::string& v);

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
    std::string m_name;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class LabelInfoDao {
public:
    typedef std::shared_ptr<LabelInfoDao> ptr;
    static int Update(LabelInfo::ptr info, sylar::IDB::ptr conn);
    static int Insert(LabelInfo::ptr info, sylar::IDB::ptr conn);
    static int InsertOrUpdate(LabelInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(LabelInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(const int64_t& id, sylar::IDB::ptr conn);
    static int DeleteById( const int64_t& id, sylar::IDB::ptr conn);
    static int DeleteByUserId( const int64_t& user_id, sylar::IDB::ptr conn);
    static int DeleteByUserIdName( const int64_t& user_id,  const std::string& name, sylar::IDB::ptr conn);
    static int QueryAll(std::vector<LabelInfo::ptr>& results, sylar::IDB::ptr conn);
    static LabelInfo::ptr Query( const int64_t& id, sylar::IDB::ptr conn);
    static int QueryByUserId(std::vector<LabelInfo::ptr>& results,  const int64_t& user_id, sylar::IDB::ptr conn);
    static LabelInfo::ptr QueryByUserIdName( const int64_t& user_id,  const std::string& name, sylar::IDB::ptr conn);
    static int CreateTableSQLite3(sylar::IDB::ptr info);
    static int CreateTableMySQL(sylar::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //__BLOG_DATALABEL_INFO_H__
