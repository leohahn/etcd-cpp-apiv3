#include "etcd/Response.hpp"

#include <iostream>


etcd::Response::Response(const etcdv3::V3Response& reply)
{
  _index = reply.index;
  _action = reply.action;
  _error_code = reply.error_code;
  _error_message = reply.error_message;
  int size = reply.values.size();
  //with prefix means that we expect that
  //values could have at least one result(e.g. ls, rmdir)
  if(size)
  {
    for(int index = 0; index < size; index++)
    {
      _values.push_back(Value(reply.values[index]));
      _keys.push_back(reply.values[index].key());
    }
  }
  //values where we expect that 
  // at most one result.(e.g. set, add, modify, rm, watch)
  else
  {
    _value = Value(reply.value);
  }

  _prev_value = Value(reply.prev_value);

}


etcd::Response::Response()
  : _error_code(0),
    _index(0)
{
}

etcd::Response::Response(int error_code, char const * error_message)
  : _error_code(error_code),
    _error_message(error_message),
    _index(0)
{
}

int etcd::Response::error_code() const
{
  return _error_code;
}

std::string const & etcd::Response::error_message() const
{
  return _error_message;
}

int etcd::Response::index() const
{
  return _index;
}

std::string const & etcd::Response::action() const
{
  return _action;
}

bool etcd::Response::is_ok() const
{
  return error_code() == 0;
}

etcd::Value const & etcd::Response::value() const
{
  return _value;
}

etcd::Value const & etcd::Response::prev_value() const
{
  return _prev_value;
}

etcd::Values const & etcd::Response::values() const
{
  return _values;
}

etcd::Value const & etcd::Response::value(int index) const
{
  return _values[index];
}

etcd::Keys const & etcd::Response::keys() const
{
  return _keys;
}

std::string const & etcd::Response::key(int index) const
{
  return _keys[index];
}
