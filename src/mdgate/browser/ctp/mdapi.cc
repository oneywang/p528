// Copyright by wangtianping. All rights reserved.

#include "mdgate/browser/ctp/mdapi.h"

#include "base/logging.h"
#include "content/browser_thread.h"

#include <ctp_account.h>
#include <windows.h>

using content::BrowserThread;

namespace{
  int g_reqid = 1;
}

//////////////////////////////////////////////////////////////////////////
void MdApi::OnFrontConnected() {
  LOG(INFO) << "OnFrontConnected";
}

//release会同步调用到这个OnFrontDisconnected
void MdApi::OnFrontDisconnected(int nReason){
  LOG(INFO) << "OnFrontDisconnected:" << nReason;
}

//有可能是自动重连这里要订阅之前订阅的行情-
void MdApi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  CHECK(pRspUserLogin);
  CHECK(pRspInfo);

  LOG(INFO) << "OnRspUserLogin:" << nRequestID;
  if (pRspInfo->ErrorID == 0){
    LOG(INFO) << "LoginTime:" << pRspUserLogin->LoginTime;
    LOG(INFO) << "TradingDay:" << pRspUserLogin->TradingDay;
  }else{
    LOG(INFO) << "login error:" << pRspInfo->ErrorID << pRspInfo->ErrorMsg;
  }
}

void MdApi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID,bool bIsLast) {
  CHECK(pUserLogout);
  CHECK(pRspInfo);

  LOG(INFO) << "OnRspUserLogout:" << nRequestID;
}

void MdApi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,  bool bIsLast) {
  CHECK(pRspInfo);

  LOG(INFO) << "OnRspError:" << nRequestID << pRspInfo->ErrorID << pRspInfo->ErrorMsg;
}

// not impl in ctp!
void MdApi::OnHeartBeatWarning(int nTimeLapse){
  LOG(INFO) << "OnHeartBeatWarning" << nTimeLapse;
}

// 合约要通过tradeapi的ReqQryInstrument来获取=
void MdApi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast) {
  CHECK(pSpecificInstrument);
  CHECK(pRspInfo);

  LOG(INFO) << "OnRspSubMarketData";
}

void MdApi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast) {
  CHECK(pSpecificInstrument);
  CHECK(pRspInfo);

  LOG(INFO) << "OnRspUnSubMarketData";
}

void MdApi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
  CHECK(pDepthMarketData);

  LOG(INFO) << "OnRtnDepthMarketData";
}

void MdApi::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  CHECK(pSpecificInstrument);
  CHECK(pRspInfo);

  LOG(INFO) << "OnRspSubForQuoteRsp";
}

void MdApi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  CHECK(pSpecificInstrument);
  CHECK(pRspInfo);

  LOG(INFO) << "OnRspUnSubForQuoteRsp";
}

void MdApi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp) {
  CHECK(pForQuoteRsp);

  LOG(INFO) << "OnRtnForQuoteRsp";
}

//////////////////////////////////////////////////////////////////////////

MdApi::MdApi(){
  CHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
}

MdApi::~MdApi(){
  CHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
}

void MdApi::Setup(){
  CHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  CHECK(mdapi_ == nullptr);
  mdapi_ = CThostFtdcMdApi::CreateFtdcMdApi();
  CHECK(mdapi_);
  mdapi_->Init();
  mdapi_->RegisterSpi(this);

  LOG(INFO) << "RegisterSpi:" << GetCurrentThreadId();
}

void MdApi::Reset(){
  CHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  //mdapi_->Join();
  mdapi_->RegisterSpi(nullptr);
  mdapi_->Release();
  mdapi_ = nullptr;

  LOG(INFO) << "Release:" << GetCurrentThreadId();
}

void MdApi::Connect(){
  CHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  mdapi_->RegisterFront(CTP_MdFront);
  LOG(INFO) << "RegisterFront:" << GetCurrentThreadId();
}

void MdApi::Login(){
  CHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  CThostFtdcReqUserLoginField myreq = CThostFtdcReqUserLoginField();
  memset(&myreq, 0, sizeof(myreq));
  strcpy(myreq.BrokerID, CTP_BrokerID);
  strcpy(myreq.UserID, CTP_UserID);
  strcpy(myreq.Password, CTP_Password);

  int ret = mdapi_->ReqUserLogin(&myreq, g_reqid++);
  switch(ret){
      case 0:break;
      case -1:break;
      case -2:break;
      case -3:break;
      default:break;
  }

  LOG(INFO) << "ReqUserLogin:" << ret;
}

// not impl in ctp?
void MdApi::Logout(){
  CHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  CThostFtdcUserLogoutField myreq = CThostFtdcUserLogoutField();
  memset(&myreq, 0, sizeof(myreq));
  strcpy(myreq.BrokerID, CTP_BrokerID);
  strcpy(myreq.UserID, CTP_UserID);

  int ret = mdapi_->ReqUserLogout(&myreq, g_reqid++);
  switch (ret){
    case 0:break;
    case -1:break;
    case -2:break;
    case -3:break;
    default:break;
  }

  LOG(INFO) << "ReqUserLogout:" << ret;
}
