#ifndef __CXMLSOURCEDIALOG_H__
#define __CXMLSOURCEDIALOG_H__

#include "../wxFormBuilder/CMainWindow.h"

class CXmlSourceDialog: public CXmlSourceDlg
{
public:
    CXmlSourceDialog(wxWindow* parent = NULL, wxWindowID id = -1);

private:
    virtual void OnToolbarApplyClicked(wxCommandEvent& event);
    virtual void OnToolbarCancelClicked(wxCommandEvent& event);
};

#endif //__CXMLSOURCEDIALOG_H__