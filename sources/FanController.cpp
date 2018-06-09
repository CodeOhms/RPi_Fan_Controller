#include <thread>
#include <signal.h>
#include <sys/signalfd.h>
#include <string>
#include "FC_Logic.h"

using namespace std;

static sigset_t sigMask;

//Run in own thread as read() blocks thread
void signalHandler(AutoFan& Driver)
{
    auto quit = [&] () ->void
        { Driver.stop(); };
    auto log = [&] (string message, unsigned line)
    {
        Driver.log(message, __FILE__, __FUNCTION__, line);
    };

    int sigFD = signalfd(-1, &sigMask, 0);
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

        if(sigInfo.ssi_signo = SIGTERM)
        {
            quit();
            return;
        }
        else if(sigInfo.ssi_signo = SIGINT)
        {
            quit();
            return;
        }
        else
        {
            log("Received unexpected signal", __LINE__);
            quit();
            return;
        }

    } while(true);

}

int main()
{
    bool bStart = true;

    //Block signals. File descriptor thread handles this
    sigemptyset(&sigMask);
    sigaddset( &sigMask, SIGINT );
    sigaddset( &sigMask, SIGTERM );
    if( sigprocmask( SIG_BLOCK, &sigMask, NULL ) < 0 )
        bStart = false;

    AutoFan FanDriver;
    string driverError;
    //Logging is still okay to use even if this fails
    if( !FanDriver.initialise(driverError) )
    {
        bStart = false;
        FanDriver.log(driverError);
    }
    else if(!bStart)
    {
        bStart = false;
        FanDriver.log("Unable to block signals. Terminating now.");
    }
    if(!bStart)
        return 1;

    thread sigHandle(signalHandler, ref(FanDriver));

    FanDriver.start();

    sigHandle.join();
    return 0;
}
