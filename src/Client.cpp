#include <memory>
#include <etcd/Utils.h>
#include <etcd/Client.hpp>
#include <etcd/v3/AsyncTxnResponse.hpp>
#include <etcd/v3/AsyncRangeResponse.hpp>
#include <etcd/v3/AsyncWatchResponse.hpp>
#include <etcd/v3/AsyncDeleteRangeResponse.hpp>
#include <etcd/v3/Transaction.hpp>
#include <iostream>

#include <etcd/v3/AsyncSetAction.hpp>
#include <etcd/v3/AsyncCompareAndSwapAction.hpp>
#include <etcd/v3/AsyncCompareAndDeleteAction.hpp>
#include <etcd/v3/AsyncUpdateAction.hpp>
#include <etcd/v3/AsyncGetAction.hpp>
#include <etcd/v3/AsyncDeleteAction.hpp>
#include <etcd/v3/AsyncWatchAction.hpp>
#include <etcd/v3/AsyncLeaseGrantAction.hpp>


using grpc::Channel;


etcd::Client::Client(std::string const & address)
	: channel_(etcd::utils::createChannel(address))
	, stub_(KV::NewStub(channel_))
	, watchServiceStub(Watch::NewStub(channel_))
	, leaseServiceStub(Lease::NewStub(channel_))
{}

std::shared_ptr<Channel> etcd::Client::channel() const
{
	return channel_;
}

pplx::task<etcd::Response> etcd::Client::get(std::string const & key)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.withPrefix = false;
  params.kv_stub = stub_.get();
  std::shared_ptr<etcdv3::AsyncGetAction> call(new etcdv3::AsyncGetAction(params));
  return Response::create(call);
}

pplx::task<etcd::Response> etcd::Client::set(std::string const & key, std::string const & value, int ttl)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.value.assign(value);
  params.kv_stub = stub_.get();

  if(ttl > 0)
  {
    auto res = leasegrant(ttl).get();
    if(!res.is_ok()) 
    {
      return pplx::task<etcd::Response>([res]()
      {
        return etcd::Response(res.error_code(), res.error_message().c_str());    
      });
    }
    else
    {
      params.lease_id = res.value().lease();
    }
  }

  std::shared_ptr<etcdv3::AsyncSetAction> call(new etcdv3::AsyncSetAction(params));
  return Response::create(call);
}

pplx::task<etcd::Response> etcd::Client::set(std::string const & key, std::string const & value, int64_t leaseid)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.value.assign(value);
  params.lease_id = leaseid;
  params.kv_stub = stub_.get();
  std::shared_ptr<etcdv3::AsyncSetAction> call(new etcdv3::AsyncSetAction(params));
  return Response::create(call);
}


pplx::task<etcd::Response> etcd::Client::add(std::string const & key, std::string const & value, int ttl)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.value.assign(value);
  params.kv_stub = stub_.get();

  if(ttl > 0)
  {
    auto res = leasegrant(ttl).get();
    if(!res.is_ok()) 
    {
      return pplx::task<etcd::Response>([res]()
      {
        return etcd::Response(res.error_code(), res.error_message().c_str());    
      });
    }
    else
    {
      params.lease_id = res.value().lease();
    }
  }
  std::shared_ptr<etcdv3::AsyncSetAction> call(new etcdv3::AsyncSetAction(params,true));
  return Response::create(call);
}

pplx::task<etcd::Response> etcd::Client::add(std::string const & key, std::string const & value, int64_t leaseid)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.value.assign(value);
  params.lease_id = leaseid;
  params.kv_stub = stub_.get();
  std::shared_ptr<etcdv3::AsyncSetAction> call(new etcdv3::AsyncSetAction(params,true));
  return Response::create(call);
}


pplx::task<etcd::Response> etcd::Client::modify(std::string const & key, std::string const & value, int ttl)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.value.assign(value);
  params.kv_stub = stub_.get();

  if(ttl > 0)
  {
    auto res = leasegrant(ttl).get();
    if(!res.is_ok()) 
    {
      return pplx::task<etcd::Response>([res]()
      {
        return etcd::Response(res.error_code(), res.error_message().c_str());    
      });
    }
    else
    {
      params.lease_id = res.value().lease();
    }
  }
  std::shared_ptr<etcdv3::AsyncUpdateAction> call(new etcdv3::AsyncUpdateAction(params));
  return Response::create(call);
}

pplx::task<etcd::Response> etcd::Client::modify(std::string const & key, std::string const & value, int64_t leaseid)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.value.assign(value);
  params.lease_id = leaseid;
  params.kv_stub = stub_.get();
  std::shared_ptr<etcdv3::AsyncUpdateAction> call(new etcdv3::AsyncUpdateAction(params));
  return Response::create(call);
}


pplx::task<etcd::Response> etcd::Client::modify_if(std::string const & key, std::string const & value, std::string const & old_value, int ttl)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.value.assign(value);
  params.old_value.assign(old_value);
  params.kv_stub = stub_.get();

  if(ttl > 0)
  {
    auto res = leasegrant(ttl).get();
    if(!res.is_ok()) 
    {
      return pplx::task<etcd::Response>([res]()
      {
        return etcd::Response(res.error_code(), res.error_message().c_str());    
      });
    }
    else
    {
      params.lease_id = res.value().lease();
    }
  }
  std::shared_ptr<etcdv3::AsyncCompareAndSwapAction> call(new etcdv3::AsyncCompareAndSwapAction(params,etcdv3::Atomicity_Type::PREV_VALUE));
  return Response::create(call);
}

pplx::task<etcd::Response> etcd::Client::modify_if(std::string const & key, std::string const & value, std::string const & old_value, int64_t leaseid)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.value.assign(value);
  params.old_value.assign(old_value);
  params.lease_id = leaseid;
  params.kv_stub = stub_.get();
  std::shared_ptr<etcdv3::AsyncCompareAndSwapAction> call(new etcdv3::AsyncCompareAndSwapAction(params,etcdv3::Atomicity_Type::PREV_VALUE));
  return Response::create(call);
}



pplx::task<etcd::Response> etcd::Client::modify_if(std::string const & key, std::string const & value, int old_index, int ttl)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.value.assign(value);
  params.old_revision = old_index;
  params.kv_stub = stub_.get();
  if(ttl > 0)
  {
    auto res = leasegrant(ttl).get();
    if(!res.is_ok()) 
    {
      return pplx::task<etcd::Response>([res]()
      {
        return etcd::Response(res.error_code(), res.error_message().c_str());    
      });
    }
    else
    {
      params.lease_id = res.value().lease();
    }
  }
  std::shared_ptr<etcdv3::AsyncCompareAndSwapAction> call(new etcdv3::AsyncCompareAndSwapAction(params,etcdv3::Atomicity_Type::PREV_INDEX));
  return Response::create(call);
}

pplx::task<etcd::Response> etcd::Client::modify_if(std::string const & key, std::string const & value, int old_index, int64_t leaseid)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.value.assign(value);
  params.lease_id = leaseid;
  params.old_revision = old_index;
  params.kv_stub = stub_.get();
  std::shared_ptr<etcdv3::AsyncCompareAndSwapAction> call(new etcdv3::AsyncCompareAndSwapAction(params,etcdv3::Atomicity_Type::PREV_INDEX));
  return Response::create(call);
}


pplx::task<etcd::Response> etcd::Client::rm(std::string const & key)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.withPrefix = false;
  params.kv_stub = stub_.get();
  std::shared_ptr<etcdv3::AsyncDeleteAction> call(new etcdv3::AsyncDeleteAction(params));
  return Response::create(call);
}


pplx::task<etcd::Response> etcd::Client::rm_if(std::string const & key, std::string const & old_value)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.old_value.assign(old_value);
  params.kv_stub = stub_.get();
  std::shared_ptr<etcdv3::AsyncCompareAndDeleteAction> call(new etcdv3::AsyncCompareAndDeleteAction(params,etcdv3::Atomicity_Type::PREV_VALUE));
  return Response::create(call);
}

pplx::task<etcd::Response> etcd::Client::rm_if(std::string const & key, int old_index)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.old_revision = old_index;
  params.kv_stub = stub_.get();
  std::shared_ptr<etcdv3::AsyncCompareAndDeleteAction> call(new etcdv3::AsyncCompareAndDeleteAction(params, etcdv3::Atomicity_Type::PREV_INDEX));;
  return Response::create(call);

}

pplx::task<etcd::Response> etcd::Client::rmdir(std::string const & key, bool recursive)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.withPrefix = recursive;
  params.kv_stub = stub_.get();
  std::shared_ptr<etcdv3::AsyncDeleteAction> call(new etcdv3::AsyncDeleteAction(params));
  return Response::create(call);
}

pplx::task<etcd::Response> etcd::Client::ls(std::string const & key)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.withPrefix = true;
  params.kv_stub = stub_.get();
  std::shared_ptr<etcdv3::AsyncGetAction> call(new etcdv3::AsyncGetAction(params));
  return Response::create(call);
}

pplx::task<etcd::Response> etcd::Client::watch(std::string const & key, bool recursive)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.withPrefix = recursive;
  params.watch_stub = watchServiceStub.get();
  std::shared_ptr<etcdv3::AsyncWatchAction> call(new etcdv3::AsyncWatchAction(params));
  return Response::create(call);
}

pplx::task<etcd::Response> etcd::Client::watch(std::string const & key, int fromIndex, bool recursive)
{
  etcdv3::ActionParameters params;
  params.key.assign(key);
  params.withPrefix = recursive;
  params.revision = fromIndex;
  params.watch_stub = watchServiceStub.get();
  std::shared_ptr<etcdv3::AsyncWatchAction> call(new etcdv3::AsyncWatchAction(params));
  return Response::create(call);
}

pplx::task<etcd::Response> etcd::Client::leasegrant(int ttl)
{
  etcdv3::ActionParameters params;
  params.ttl = ttl;
  params.lease_stub = leaseServiceStub.get();
  std::shared_ptr<etcdv3::AsyncLeaseGrantAction> call(new etcdv3::AsyncLeaseGrantAction(params));
  return Response::create(call);
}



