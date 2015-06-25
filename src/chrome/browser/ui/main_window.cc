#include "chrome/browser/ui/main_window.h"

#include "chrome/browser/ui/trading_widget.h"
#include "chrome/browser/ui/marketdata_widget.h"
#include "chrome/browser/ui/order_widget.h"
#include "chrome/browser/ui/trade_widget.h"
#include "chrome/browser/ui/log_widget.h"
#include "chrome/browser/ui/position_widget.h"
#include "chrome/browser/ui/account_widget.h"

#include "base/logging.h"

#include <QTabWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

//////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget* parent /* = 0 */)
  :QMainWindow(parent){
  InitUi();
}

MainWindow::~MainWindow(){

}

void MainWindow::InitUi(){

  this->setWindowTitle(QStringLiteral("p528量化交易系统"));

  TradingWidget* trading_tab = new TradingWidget(this);
  MarketDataWidget* marketdata_tab = new MarketDataWidget(this);
  OrderWidget* order_tab = new OrderWidget(this);
  TradeWidget* trade_tab = new TradeWidget(this);
  LogWidget* log_tab = new LogWidget(this);
  PositionWidget* position_tab = new PositionWidget(this);
  AccountWidget* account_tab = new AccountWidget(this);

  //up: 
  trading_tab->setMaximumWidth(400);
  QVBoxLayout* tradingVBox = new QVBoxLayout();
  tradingVBox->addWidget(trading_tab);
  tradingVBox->addStretch();

  QHBoxLayout* upHBox = new QHBoxLayout();
  upHBox->addWidget(marketdata_tab);
  upHBox->addLayout(tradingVBox);

  //down:
  QTabWidget* right_tabs = new QTabWidget(this);
  right_tabs->addTab(position_tab, QStringLiteral("持仓"));
  right_tabs->addTab(account_tab, QStringLiteral("账户"));

  QTabWidget* left_tabs = new QTabWidget(this);
  left_tabs->addTab(order_tab, QStringLiteral("报单"));
  left_tabs->addTab(trade_tab, QStringLiteral("成交"));
  left_tabs->addTab(log_tab, QStringLiteral("日志"));

  QHBoxLayout* downHBox = new QHBoxLayout();
  downHBox->addWidget(left_tabs);
  downHBox->addWidget(right_tabs);

  //main
  QVBoxLayout* vBox = new QVBoxLayout();
  vBox->addLayout(upHBox);
  vBox->addLayout(downHBox);

  QWidget* central_widget = new QWidget(this);
  central_widget->setLayout(vBox);

  this->setCentralWidget(central_widget);
}
