#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "singleton.h"
#include "ConfigMgr.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

// 前置声明 RPConPool
class RPConPool;

class VerifyGrpcClient :public Singleton<VerifyGrpcClient>
{
    friend class Singleton<VerifyGrpcClient>;
public:
    GetVarifyRsp GetVarifyCode(std::string email);
    ~VerifyGrpcClient(); //新增析构函数声明
private:
    VerifyGrpcClient();
    std::unique_ptr<RPConPool> pool_;
};