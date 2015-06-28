// Copyright by wangtianping. All rights reserved.

#ifndef CHROME_BROWSER_MDAPI_H
#define CHROME_BROWSER_MDAPI_H

#include <ThostFtdcUserApiDataType.h>
#include <ThostFtdcUserApiStruct.h>
#include <ThostFtdcMdApi.h>

// visit on UI
class MdSpiObserver;
class MdApiDelegate;

// visit on UI/IO/CTP-callback
class MdApi : public CThostFtdcMdSpi{
public:
  explicit MdApi();
  virtual ~MdApi();

public:
  // call on cache thread,and waitfor ctp-thread exit
  void Setup();

  // call on ui thread
  void Reset();
  void Connect();
  void Login();
  void Logout();

private:
  // callback on ctp thread,post data to io and other to ui thread
  // we run policy on io thread,so post ticks to io
  void OnFrontConnected() override; //ctp
  void OnFrontDisconnected(int nReason)override; //ui
  void OnHeartBeatWarning(int nTimeLapse)override;
  void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) override;
  void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp) override;

private:
  CThostFtdcMdApi* mdapi_ = nullptr; 
};

#endif //CHROME_BROWSER_MDAPI_H
