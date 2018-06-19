#include <thread>
#include <signal.h>
#include <sys/signalfd.h>
#include <string>
#include "FC_Logic.h"

using namespace std;

//Run in own thread as read() blocks thread
void signalHandler(AutoFan& Driver)
{
    sigset_t sigBlock;
    sigaddset( &sigBlock, SIGINT );
    sigaddset( &sigBlock, SIGTERM );
    sigaddset( &sigBlock, SIGHUP );

    int sigFD;

    auto quit = [&] () -> void
    {
        Driver.stop();
        close(sigFD);
    };
    auto restart = [&] () -> void
    {
        //First, this reloads the config. Next, it restarts the logic loop
        if( !Driver.restart() )
            quit();
    };
    auto log = [&] (string message, unsigned line) -> void
    {
        Driver.log(message, __FILE__, __FUNCTION__, line);
    };

    sigFD = signalfd(-1, &sigBlock, 0);
    if(sigFD < 0)
    {
        log("signalfd failed to create file descriptor", __LINE__);
        quit();
        return;
    }

    struct signalfd_siginfo sigInfo;
    ssize_t result;
    do
    {
        result = read(sigFD, &sigInfo, sizeof(sigInfo));
        if(result < 0)
        {
            log("read of sigFD failed", __LINE__);
            quit();
            return;
        }
        else if(result != sizeof(sigInfo))
        {
            log("Size of sigInfo doesn't match result", __LINE__);
            quit();
            return;
        }

        if(sigInfo.ssi_signo      == SIGTERM)
        {
            quit();
            return;
        }
        else if(sigInfo.ssi_signo == SIGINT)
        {
            quit();
            return;
        }
        else if(sigInfo.ssi_signo == SIGHUP)
        {
            Driver.log("Restarting", 2);//Verbosity = debug
            restart();
        }
        else
        {
            log("Received unexpected signal", __LINE__);
            quit();
            return;
        }

    } while(true);

}

bool setup(AutoFan& FDriver)
{
    string driverError;
    //Logging is still okay to use even if this fails
    if( !FDriver.initialise(driverError) )
    {
        FDriver.log(driverError);
        return false;
    }
    return true;

}
int main()
{
    sigset_t sigMask;
    bool bSignals = true;

    //Block all signals. File descriptor thread handles this
    sigfillset(&sigMask);
    if( sigprocmask( SIG_BLOCK, &sigMask, NULL ) < 0 )
        bSignals = false;

    AutoFan FanDriver;
    if( !setup(FanDriver) )
        return 1;

    if(!bSignals)
    {
        FanDriver.log("Unable to block signals. Terminating now.");
        return 1;
    }

    thread sigHandle(signalHandler, ref(FanDriver));

    if( !FanDriver.start() )
        return 1;

    sigHandle.join();
    return 0;
}
