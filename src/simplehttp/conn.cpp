//
// Created by ztk on 2022/3/31.
//

#include <simplehttp.hpp>

namespace mycs::simplehttp {
void do_remove_conn_from_server(Server* server, Conn* conn) { ServConnCtrlObj::remove_conn(server, conn); }
}  // namespace mycs::simplehttp