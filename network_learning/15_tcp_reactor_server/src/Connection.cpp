#include "Connection.hpp"


void Connection::RegisterCallBack(callback r, callback w, callback e){
  _read_cb = r;
  _write_cb = w;
  _except_cb = e;
}