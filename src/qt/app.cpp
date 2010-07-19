/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/app.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/app.h"
#include "wx/apptrait.h"
#include "wx/qt/utils.h"
#include "wx/qt/converter.h"

IMPLEMENT_DYNAMIC_CLASS( wxApp, wxAppBase )

wxApp::wxApp()
{
}


wxApp::~wxApp()
{
    delete m_qtApplication;

    delete [] m_qtArgv;
}

bool wxApp::Initialize( int &argc, wxChar **argv )
{
    if ( !wxAppBase::Initialize( argc, argv ))
        return false;

    wxConvCurrent = &wxConvUTF8;
    wxMISSING_IMPLEMENTATION( "Unicode argc, argv" );

    // (See: http://bugreports.qt.nokia.com/browse/QTBUG-7551)
    // Need to store argc, argv. The argc, argv from wxAppBase are
    // being initialized *after* Initialize();

    // Clone and store arguments
    m_qtArgv = new char *[argc + 1];
    for ( int i = 0; i < argc; i++ )
    {
        m_qtArgv[i] = wxStrdupA(wxConvUTF8.cWX2MB(argv[i]));
    }
    m_qtArgv[argc] = NULL;
    m_qtArgc = argc;

    m_qtApplication = new QApplication( m_qtArgc, m_qtArgv, QApplication::GuiClient );

    // Use the args returned by Qt as it may have deleted (processed) some of them
    // Using QApplication::arguments() forces argument processing
    QStringList qtArgs = m_qtApplication->arguments();
    if ( qtArgs.size() != argc )
    {
        /* As per Qt 4.6: Here, qtArgc and qtArgv have been modified and can
         * be used to replace our args (with Qt-flags removed). Also, they can be
         * deleted as they are internally kept by Qt in a list after calling arguments().
         * However, there isn't any guarantee of that in the docs, so we keep arguments
         * ourselves and only delete then after the QApplication is deleted */
        
        // Qt changed the arguments
        delete [] argv;
        argv = new wxChar *[qtArgs.size() + 1];
        for ( int i = 0; i < qtArgs.size(); i++ )
        {
#if wxUSE_UNICODE
            argv[i] = wxStrdupW( wxConvUTF8.cMB2WX( qtArgs[i].toUtf8().data() ) );
#else // wxUSE_UNICODE
            argv[i] = wxStrdupA( wxConvUTF8.cMB2WX( qtArgs[i].toUtf8().data() ) );
#endif // wxUSE_UNICODE
        }

        argc = m_qtApplication->arguments().size();
        argv[argc] = NULL;
    }
    

    return true;
}

#if wxUSE_GUI

wxGUIAppTraits::wxGUIAppTraits()
{
}

wxGUIAppTraits::~wxGUIAppTraits()
{
}

#endif // wxUSE_GUI