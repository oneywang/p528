#include "chrome/browser/ui/marketdata_widget.h"
#include <QHeaderView>

//////////////////////////////////////////////////////////////////////////

MarketDataWidget::MarketDataWidget(QWidget* parent /* = 0 */)
  :QTableWidget(parent){
  InitUi();
}

MarketDataWidget::~MarketDataWidget(){

}

void MarketDataWidget::InitUi(){
  QMap<QString, QString> dictLabels;

  dictLabels["0-Name"] = QStringLiteral("合约名称");
  dictLabels["1-InstrumentID"] = QStringLiteral("合约代码");
  dictLabels["2-ExchangeInstID"] = QStringLiteral("合约交易所代码");
  dictLabels["3-BidPrice1"] = QStringLiteral("买一价");
  dictLabels["4-BidVolume1"] = QStringLiteral("买一量");
  dictLabels["5-AskPrice1"] = QStringLiteral("卖一价");
  dictLabels["6-AskVolume1"] = QStringLiteral("卖一量");
  dictLabels["7-LastPrice"] = QStringLiteral("最新价");
  dictLabels["8-Volume"] = QStringLiteral("成交量");
  dictLabels["9-UpdateTime"] = QStringLiteral("更新时间");
  
  this->setColumnCount(dictLabels.size());
  this->setHorizontalHeaderLabels(dictLabels.values());
  this->verticalHeader()->setVisible(false);
  this->setEditTriggers(QTableWidget::NoEditTriggers);
}
