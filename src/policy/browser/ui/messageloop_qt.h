#ifndef CHROME_BROWSER_MESSAGELOOP_QT_H
#define CHROME_BROWSER_MESSAGELOOP_QT_H

#include "base/message_loop/message_pump.h"
#include "base/time/time.h"
#include "base/memory/scoped_ptr.h"

#include <QObject>
#include <QEvent>
#include <QTimerEvent>
#include <QEventLoop>

class MessagePumpForUIQt : public QObject,
  public base::MessagePump
{
  Q_OBJECT
public:
  MessagePumpForUIQt();

  virtual void Run(Delegate *delegate) Q_DECL_OVERRIDE;

  virtual void Quit() Q_DECL_OVERRIDE;
  virtual void ScheduleWork() Q_DECL_OVERRIDE;

  virtual void ScheduleDelayedWork(const base::TimeTicks &delayed_work_time) Q_DECL_OVERRIDE;

protected:
  virtual void customEvent(QEvent *ev) Q_DECL_OVERRIDE;

  virtual void timerEvent(QTimerEvent *ev) Q_DECL_OVERRIDE;

private slots:
  void OnIdle();

private:
  bool handleScheduledWork();

private:
  Delegate *m_delegate;
#if 0
  QEventLoop *m_explicitLoop;
#endif
  int m_timerId;
  base::TimeTicks m_timerScheduledTime;
};

scoped_ptr<base::MessagePump> QtMessagePumpFactory();
void QtInit();
void QtFini();
QApplication* QtApp();


#endif //CHROME_BROWSER_MESSAGELOOP_QT_H