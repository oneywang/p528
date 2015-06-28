#include "chrome/browser/ui/position_widget.h"
#include <QHeaderView>

//////////////////////////////////////////////////////////////////////////
PositionWidget::PositionWidget(QWidget* parent /* = 0 */)
  :QTableWidget(parent){
  InitUi();
}

PositionWidget::~PositionWidget(){

}

void PositionWidget::InitUi(){
  QMap<QString, QString> dictLabels;

  dictLabels["0-InstrumentID"] = QStringLiteral("合约代码");
  dictLabels["1-PosiDirection"] = QStringLiteral("方向");
  dictLabels["2-Position"] = QStringLiteral("持仓");
  dictLabels["3-LongFrozen"] = QStringLiteral("多头冻结");
  dictLabels["4-ShortFrozen"] = QStringLiteral("空头冻结");
  dictLabels["5-PositionCost"] = QStringLiteral("持仓成本");
  dictLabels["6-OpenCost"] = QStringLiteral("开仓成本");

  this->setColumnCount(dictLabels.size());
  this->setHorizontalHeaderLabels(dictLabels.values());
  this->verticalHeader()->setVisible(false);
  this->setEditTriggers(QTableWidget::NoEditTriggers);
}
