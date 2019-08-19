#ifndef __BLOG_DATACHANNEL_INFO_H__
#define __BLOG_DATACHANNEL_INFO_H__

#include <json/json.h>
#include <vector>
#include "sylar/db/db.h"
#include "sylar/util.h"


namespace blog {
namespace data {

class ChannelInfoDao;
class ChannelInfo {
friend class ChannelInfoDao;
public:
    typedef std::shared_ptr<ChannelInfo> ptr;

    ChannelInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const std::string& getName() { return m_name; }
    void setName(const std::string& v);

    const int64_t& getCreateTime() { return m_createTime; }
    void setCreateTime(const int64_t& v);

    const int64_t& getUpdateTime() { return m_updateTime; }
    void setUpdateTime(const int64_t& v);

    std::string toJsonString() const;

private:
    int64_t m_id;
    std::string m_name;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class ChannelInfoDao {
public:
    typedef std::shared_ptr<ChannelInfoDao> ptr;
    static int Update(ChannelInfo::ptr info, sylar::IDB::ptr conn);
    static int Insert(ChannelInfo::ptr info, sylar::IDB::ptr conn);
    static int InsertOrUpdate(ChannelInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(ChannelInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(const int64_t& id, sylar::IDB::ptr conn);
    static int DeleteById( const int64_t& id, sylar::IDB::ptr conn);
    static int QueryAll(std::vector<ChannelInfo::ptr>& results, sylar::IDB::ptr conn);
    static ChannelInfo::ptr Query( const int64_t& id, sylar::IDB::ptr conn);
    static int CreateTableSQLite3(sylar::IDB::ptr info);
    static int CreateTableMySQL(sylar::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //__BLOG_DATACHANNEL_INFO_H__
