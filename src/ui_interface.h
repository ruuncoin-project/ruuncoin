#ifndef BITCOIN_UI_INTERFACE_H
#define BITCOIN_UI_INTERFACE_H

#include <string>
#include "util.h"
#include <boost/signals2/signal.hpp>
#include <boost/signals2/last_value.hpp>

class CBasicKeyStore;
class CWallet;
class uint256;

enum ChangeType
{
    CT_NEW,
    CT_UPDATED,
    CT_DELETED
};

class CClientUIInterface
{
public:
    enum MessageBoxFlags
    {
        ICON_INFORMATION    = 0,
        ICON_WARNING        = (1U << 0),
        ICON_ERROR          = (1U << 1),
        ICON_MASK = (ICON_INFORMATION | ICON_WARNING | ICON_ERROR),

        BTN_OK      = 0x00000400U,
        BTN_YES     = 0x00004000U,
        BTN_NO      = 0x00010000U,
        BTN_ABORT   = 0x00040000U,
        BTN_RETRY   = 0x00080000U,
        BTN_IGNORE  = 0x00100000U,
        BTN_CLOSE   = 0x00200000U,
        BTN_CANCEL  = 0x00400000U,
        BTN_DISCARD = 0x00800000U,
        BTN_HELP    = 0x01000000U,
        BTN_APPLY   = 0x02000000U,
        BTN_RESET   = 0x04000000U,
        BTN_MASK = (BTN_OK | BTN_YES | BTN_NO | BTN_ABORT | BTN_RETRY | BTN_IGNORE |
                    BTN_CLOSE | BTN_CANCEL | BTN_DISCARD | BTN_HELP | BTN_APPLY | BTN_RESET),

        MODAL               = 0x10000000U,

        MSG_INFORMATION = ICON_INFORMATION,
        MSG_WARNING = (ICON_WARNING | BTN_OK | MODAL),
        MSG_ERROR = (ICON_ERROR | BTN_OK | MODAL)
    };

    boost::signals2::signal<bool (const std::string& message, const std::string& caption, unsigned int style), boost::signals2::last_value<bool> > ThreadSafeMessageBox;

    boost::signals2::signal<bool (int64 nFeeRequired), boost::signals2::last_value<bool> > ThreadSafeAskFee;

    boost::signals2::signal<void (const std::string& strURI)> ThreadSafeHandleURI;

    boost::signals2::signal<void (const std::string &message)> InitMessage;

    boost::signals2::signal<std::string (const char* psz)> Translate;

    boost::signals2::signal<void ()> NotifyBlocksChanged;

    boost::signals2::signal<void (int newNumConnections)> NotifyNumConnectionsChanged;

    boost::signals2::signal<void (const uint256 &hash, ChangeType status)> NotifyAlertChanged;
};

extern CClientUIInterface uiInterface;

inline std::string _(const char* psz)
{
    boost::optional<std::string> rv = uiInterface.Translate(psz);
    return rv ? (*rv) : psz;
}

#endif
