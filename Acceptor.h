#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "Connection.h"
#include "EventLoop.h"
#include <string>
#include <memory>
#include <functional>

namespace snet
{

class Acceptor final
{
public:
    using ConnectionPtr = std::unique_ptr<Connection>;
    using OnNewConnection = std::function<void (ConnectionPtr)>;

    Acceptor(const std::string &ip, unsigned short port,
             EventLoop *loop);
    ~Acceptor();

    Acceptor(const Acceptor &) = delete;
    void operator = (const Acceptor &) = delete;

    void SetOnNewConnection(const OnNewConnection &onc);

private:
    class AcceptorEventHandler final : public EventHandler
    {
    public:
        explicit AcceptorEventHandler(Acceptor *acceptor)
            : acceptor_(acceptor)
        {
        }

        AcceptorEventHandler(const AcceptorEventHandler &) = delete;
        void operator = (const AcceptorEventHandler &) = delete;

        virtual int Fd() const override
        {
            return acceptor_->fd_;
        }

        virtual Event Events() const override
        {
            return Event::Read;
        }

        virtual Event EnabledEvents() const override
        {
            return Event::Read;
        }

        virtual void HandleRead() override
        {
            acceptor_->HandleAccept();
        }

        virtual void HandleWrite() override { }

    private:
        Acceptor *acceptor_;
    };

    bool CreateListenSocket(const std::string &ip, unsigned short port);
    void HandleAccept();

    int fd_;
    EventLoop *loop_;
    OnNewConnection onc_;
    AcceptorEventHandler eh_;
};

} // namespace snet

#endif // ACCEPTOR_H