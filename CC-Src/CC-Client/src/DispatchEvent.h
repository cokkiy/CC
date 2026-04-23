#include <QEvent>
class DispatchEvent : public QEvent {
public:
  DispatchEvent(std::function<void()> call)
      : QEvent(QEvent::Type(CUSTOM_EVENT_TYPE)), _call(call) {}

  void dispatch() { _call(); }

private:
  std::function<void()> _call;
};