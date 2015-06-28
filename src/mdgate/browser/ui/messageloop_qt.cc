#include "mdgate/browser/ui/messageloop_qt.h"

#include "base/message_loop/message_loop.h"

#include <QAbstractEventDispatcher>
#include <QApplication>

#include <shellapi.h>

namespace{

  QApplication* qt_app_ = nullptr;
  int qt_argc_ = 0;
  char** qt_argv_ = nullptr;

  char* WideToMulti(int codePage, const wchar_t *aw){
    const int required = WideCharToMultiByte(codePage, 0, aw, -1, NULL, 0, NULL, NULL);
    char *result = new char[required + 1];
    memset(result, 0, required + 1);
    WideCharToMultiByte(codePage, 0, aw, -1, result, required, NULL, NULL);
    return result;
  }

  // Return a timeout suitable for the glib loop, -1 to block forever,
  // 0 to return right away, or a timeout in milliseconds from now.
  int GetTimeIntervalMilliseconds(const base::TimeTicks& from) {
    if (from.is_null())
      return -1;

    // Be careful here.  TimeDelta has a precision of microseconds, but we want a
    // value in milliseconds.  If there are 5.5ms left, should the delay be 5 or
    // 6?  It should be 6 to avoid executing delayed work too early.
    int delay = static_cast<int>(
      ceil((from - base::TimeTicks::Now()).InMillisecondsF()));

    // If this value is negative, then we need to run delayed work soon.
    return delay < 0 ? 0 : delay;
  }
}

//////////////////////////////////////////////////////////////////////////

MessagePumpForUIQt::MessagePumpForUIQt()
// Usually this gets passed through Run, but since we have
// our own event loop, attach it explicitly ourselves.
: m_delegate(base::MessageLoopForUI::current())
#if 0
, m_explicitLoop(0)
#endif
, m_timerId(0){
}

void MessagePumpForUIQt::Run(Delegate *delegate){
  Q_ASSERT(delegate == m_delegate);
#if 0
  QEventLoop loop;
  m_explicitLoop = &loop;
  loop.exec();
  m_explicitLoop = 0;
#else
  CHECK(qt_app_);
  connect(QAbstractEventDispatcher::instance(), SIGNAL(aboutToBlock()), this, SLOT(OnIdle()));
  qt_app_->exec();
#endif
}

void MessagePumpForUIQt::Quit(){
#if 0
  Q_ASSERT(m_explicitLoop);
  m_explicitLoop->quit();
#else
  CHECK(qt_app_);
  qt_app_->quit();
#endif
}

void MessagePumpForUIQt::ScheduleWork(){
  QCoreApplication::postEvent(this, new QEvent(QEvent::User));
}

void MessagePumpForUIQt::ScheduleDelayedWork(const base::TimeTicks &delayed_work_time){
  if (delayed_work_time.is_null()) {
    killTimer(m_timerId);
    m_timerId = 0;
    m_timerScheduledTime = base::TimeTicks();
  }
  else if (!m_timerId || delayed_work_time < m_timerScheduledTime) {
    killTimer(m_timerId);
    m_timerId = startTimer(GetTimeIntervalMilliseconds(delayed_work_time));
    m_timerScheduledTime = delayed_work_time;
  }
}

void MessagePumpForUIQt::customEvent(QEvent *ev){
  if (handleScheduledWork())
    QCoreApplication::postEvent(this, new QEvent(QEvent::User));
}

void MessagePumpForUIQt::timerEvent(QTimerEvent *ev){
  Q_ASSERT(m_timerId == ev->timerId());
  killTimer(m_timerId);
  m_timerId = 0;
  m_timerScheduledTime = base::TimeTicks();

  base::TimeTicks next_delayed_work_time;
  m_delegate->DoDelayedWork(&next_delayed_work_time);
  ScheduleDelayedWork(next_delayed_work_time);
}

void MessagePumpForUIQt::OnIdle(){
  m_delegate->DoIdleWork();
}

bool MessagePumpForUIQt::handleScheduledWork() {
  bool more_work_is_plausible = m_delegate->DoWork();

  base::TimeTicks delayed_work_time;
  more_work_is_plausible |= m_delegate->DoDelayedWork(&delayed_work_time);

  if (more_work_is_plausible)
    return true;

  more_work_is_plausible |= m_delegate->DoIdleWork();
  if (!more_work_is_plausible)
    ScheduleDelayedWork(delayed_work_time);

  return more_work_is_plausible;
}

//////////////////////////////////////////////////////////////////////////

scoped_ptr<base::MessagePump> QtMessagePumpFactory(){
  return scoped_ptr<base::MessagePump>(new MessagePumpForUIQt);
}

void QtInit(){
  wchar_t **argvW = CommandLineToArgvW(GetCommandLineW(), &qt_argc_);
  CHECK(!!argvW);
  qt_argv_ = new char *[qt_argc_ + 1];
  memset(qt_argv_, 0, qt_argc_ + 1);
  for (int i = 0; i < qt_argc_; ++i)
    qt_argv_[i] = WideToMulti(CP_ACP, argvW[i]);
  qt_argv_[qt_argc_] = nullptr;
  LocalFree(argvW);

  //todo(hege):one exception here,just ingore by pass 0
#if defined(COMPONENT_BUILD)
  int argc = 0;
  char** argv = nullptr;
  qt_app_ = new QApplication(argc, argv);
#else  
  qt_app_ = new QApplication(qt_argc_, qt_argv_);
#endif
}

void QtFini(){
  CHECK(qt_app_);
  delete qt_app_;
  qt_app_ = nullptr;

  for (int i = 0; i < qt_argc_ && qt_argv_[i]; ++i)
    delete[] qt_argv_[i];
  delete[] qt_argv_;
}

QApplication* QtApp(){
  CHECK(qt_app_);
  return qt_app_;
}
//////////////////////////////////////////////////////////////////////////
