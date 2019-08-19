#ifndef __BLOG_SERVLETS_CHANNEL_QUERY_SERVLET_H__
#define __BLOG_SERVLETS_CHANNEL_QUERY_SERVLET_H__

#include "sylar/http/servlet.h"
#include "blog/struct.h"

namespace blog {
namespace servlet {

class ChannelQueryServlet : public BlogServlet {
public:
    typedef std::shared_ptr<ChannelQueryServlet> ptr;
    ChannelQueryServlet();
    virtual int32_t handle(sylar::http::HttpRequest::ptr request
                   ,sylar::http::HttpResponse::ptr response
                   ,sylar::http::HttpSession::ptr session
                   ,Result::ptr result) override;
};

}
}

#endif
