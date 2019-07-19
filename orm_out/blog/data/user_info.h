#ifndef __BLOG_DATAUSER_INFO_H__
#define __BLOG_DATAUSER_INFO_H__

#include <json/json.h>
#include <vector>
#include "sylar/db/db.h"
#include "sylar/util.h"


namespace blog {
namespace data {

class UserInfoDao;
class UserInfo {
friend class UserInfoDao;
public:
    typedef std::shared_ptr<UserInfo> ptr;

    UserInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const std::string& getAccount() { return m_account; }
    void setAccount(const std::string& v);

    const std::string& getEmail() { return m_email; }
    void setEmail(const std::string& v);

    const std::string& getPasswd() { return m_passwd; }
    void setPasswd(const std::string& v);

    const std::string& getName() { return m_name; }
    void setName(const std::string& v);

    const std::string& getCode() { return m_code; }
    void setCode(const std::string& v);

    const int32_t& getRole() { return m_role; }
    void setRole(const int32_t& v);

    const int32_t& getState() { return m_state; }
    void setState(const int32_t& v);

    const int64_t& getLoginTime() { return m_loginTime; }
    void setLoginTime(const int64_t& v);

    const int32_t& getIsDeleted() { return m_isDeleted; }
    void setIsDeleted(const int32_t& v);

    const int64_t& getCreateTime() { return m_createTime; }
    void setCreateTime(const int64_t& v);

    const int64_t& getUpdateTime() { return m_updateTime; }
    void setUpdateTime(const int64_t& v);

    std::string toJsonString() const;

private:
    int32_t m_role;
    int32_t m_state;
    int32_t m_isDeleted;
    int64_t m_id;
    std::string m_account;
    std::string m_email;
    std::string m_passwd;
    std::string m_name;
    std::string m_code;
    int64_t m_loginTime;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class UserInfoDao {
public:
    typedef std::shared_ptr<UserInfoDao> ptr;
    static int Update(UserInfo::ptr info, sylar::IDB::ptr conn);
    static int Insert(UserInfo::ptr info, sylar::IDB::ptr conn);
    static int InsertOrUpdate(UserInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(UserInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(const int64_t& id, sylar::IDB::ptr conn);
    static int DeleteById( const int64_t& id, sylar::IDB::ptr conn);
    static int DeleteByAccount( const std::string& account, sylar::IDB::ptr conn);
    static int DeleteByEmail( const std::string& email, sylar::IDB::ptr conn);
    static int DeleteByName( const std::string& name, sylar::IDB::ptr conn);
    static int QueryAll(std::vector<UserInfo::ptr>& results, sylar::IDB::ptr conn);
    static UserInfo::ptr Query( const int64_t& id, sylar::IDB::ptr conn);
    static UserInfo::ptr QueryByAccount( const std::string& account, sylar::IDB::ptr conn);
    static UserInfo::ptr QueryByEmail( const std::string& email, sylar::IDB::ptr conn);
    static UserInfo::ptr QueryByName( const std::string& name, sylar::IDB::ptr conn);
    static int CreateTableSQLite3(sylar::IDB::ptr info);
    static int CreateTableMySQL(sylar::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //__BLOG_DATAUSER_INFO_H__
