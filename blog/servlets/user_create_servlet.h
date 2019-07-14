#ifndef __BLOG_SERVLETS_USER_CREATE_SERVLET_H__
#define __BLOG_SERVLETS_USER_CREATE_SERVLET_H__

#include "sylar/http/servlet.h"

namespace blog {
namespace servlet {

class UserCreateServlet : public sylar::http::Servlet {
public:
    typedef std::shared_ptr<UserCreateServlet> ptr;
    UserCreateServlet();
    virtual int32_t handle(sylar::http::HttpRequest::ptr request
                   , sylar::http::HttpResponse::ptr response
                   , sylar::http::HttpSession::ptr session) override;
};

}
}

#endif
