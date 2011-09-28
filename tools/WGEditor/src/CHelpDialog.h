#ifndef __CHELPDIALOG_H__
#define __CHELPDIALOG_H__

#include "../wxFormBuilder/CMainWindow.h"

class CHelpDialog: public CHelpWindow
{
public:
    CHelpDialog(wxWindow* parent = NULL, wxWindowID id = -1);

private:
    virtual void OnCloseWindow(wxCloseEvent& event);

    virtual void OnTreeSelChanged(wxTreeEvent& event);
    virtual void OnHtmlLinkClicked(wxHtmlLinkEvent& event);
};

#endif // __CHELPWDIALOG_H__