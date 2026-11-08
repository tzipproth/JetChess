/////////////////////////////////////////////////////////////////////////////
// Name:        hyperlink.cpp
// Purpose:     wxHyperLink
// Author:      Hans Van Leemputten
// Modified by:
// Created:     03/11/2001
// RCS-ID:      $Id: hyperlink.cpp,v 1.2 2006/02/08 05:02:13 spicerun Exp $
// Copyright:   (c) Hans Van Leemputten
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifndef WX_PRECOMP
#include "wx/event.h"
#endif

#if defined(__WXGTK__) || defined(__WKXMOTIF__)
#include <wx/dir.h>
#include <wx/utils.h>
#endif
#ifdef __WXMSW__    
#include "wx/msw/registry.h"
#endif
#ifdef __GNUWIN32__
#include "windows.h"
#endif
#include "wx/stattext.h"
#include "hyperlink.h"

// wxHyperLink.cpp : implementation file
//

IMPLEMENT_DYNAMIC_CLASS(wxHyperLink, wxStaticText)

BEGIN_EVENT_TABLE(wxHyperLink, wxStaticText)
    EVT_ENTER_WINDOW(wxHyperLink::OnActivate)
    EVT_LEAVE_WINDOW(wxHyperLink::OnDeActivate)
    EVT_LEFT_DOWN(wxHyperLink::OnClick)
    EVT_LEFT_UP(wxHyperLink::OnClick)
END_EVENT_TABLE()


/////////////////////////////////////////////////////////////////////////////
// wxHyperLink

/* Constructor */
wxHyperLink::wxHyperLink(wxWindow *parent, wxWindowID id, const wxString &url, const wxString &label, const wxColour &normal, const wxColour &active, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
{
    Create(parent, id, url, label, normal, active, pos, size, style, name);
}

/* Destructor */
wxHyperLink::~wxHyperLink()
{

}

bool wxHyperLink::Create(wxWindow *parent, wxWindowID id, const wxString &url, const wxString &label, const wxColour &normal, const wxColour &active, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
{
    bool bReturn = FALSE;

    bReturn = wxStaticText::Create(parent, id, label, pos, size, style, name);
    wxASSERT_MSG(bReturn, wxT("Failed to create wxStaticText, needed by wxHyperLink!"));    

    // Set the Mouse cursor to change to a hand when it passes over the lable...
    SetCursor(wxCursor(wxCURSOR_HAND));

    // Set the colour
    if (active == wxNullColour)
        bReturn = bReturn && SetForegroundColour(normal);
    else
        bReturn = bReturn && SetColour(normal, active);

    // Set the url and label
    SetURL(url, label);

    m_bActive = FALSE;

    return bReturn;
}

/* Set 2 colours with the use of one colour. */
bool wxHyperLink::SetForegroundColour(const wxColour &colour)
{
    wxColour normal(*wxBLUE);

    // Make sure the colour is ok!
    if (colour != wxNullColour)
        normal = colour;

    //wxASSERT_MSG(colour!=wxNullColour, wxT("wxHyperLink colour can not be wxNullColour!"));

    wxColour active(CallcColour(normal.Red()), CallcColour(normal.Green()), CallcColour(normal.Blue()));
    return SetColour(normal, active);
}

bool wxHyperLink::SetColour(const wxColour &colourNormal, const wxColour &colourActive)
{
    wxASSERT_MSG(colourNormal != wxNullColour && colourActive != wxNullColour, 
    wxT("wxHyperLink colour can not be wxNullColour!"));

    // Set members...
    m_ColourNormal = colourNormal;
    m_ColourActive = colourActive;

    return wxStaticText::SetForegroundColour(m_ColourNormal);
}


// Choose an alternate component value for the active link colour.
unsigned char wxHyperLink::CallcColour(unsigned char colour)
{
    if (colour != 0)
    {
        if (colour > 127)
            return colour - 100;
        else
            return colour + 100;
    }
    else
        return 0;
}

bool wxHyperLink::LoadLink()
{
    return wxLaunchDefaultBrowser(m_strURL);
}

void wxHyperLink::SetURL(const wxString &url, const wxString &label)
{
    // A link requires a non-empty URL.
    wxASSERT(!url.IsEmpty());

    if (label.IsEmpty())
        SetLabel(url);
    else
        SetLabel(label);

    m_strURL = url;
}

void wxHyperLink::GetURL(wxString &url, wxString &label)
{
    label = GetLabel();
    url = m_strURL;
}

void wxHyperLink::SetUnderline(bool bUnderline, bool bAlways)
{
    m_bUnderline = bUnderline;
    m_bUndelineAlways = bAlways;

    if (m_bUndelineAlways)
    {
        wxFont fontLink = wxControl::GetFont();
        fontLink.SetUnderlined(m_bUnderline);
        wxControl::SetFont(fontLink);
    }
    else
    {
        // test if we are not active.
        if (!m_bActive)
        {
            // if we are not active disable the line.
            wxFont fontLink = wxControl::GetFont();
            fontLink.SetUnderlined(FALSE);
            wxControl::SetFont(fontLink);
        }
    }
}

bool wxHyperLink::GetUnderline()
{
    if (m_bUndelineAlways && m_bUnderline)
    {
        return TRUE;
    }
    else
    {
        // Test if we are currently active
        // Depending on the result we return our restult
        return (m_bActive && m_bUndelineAlways && m_bUnderline);
    }

    // Default...
    //  return FALSE;  // cannot be reached

}

/////////////////////////////////////////////////////////////////////////////
// wxHyperLink message handlers

void wxHyperLink::OnActivate(wxMouseEvent &event)
{
    if (!m_bUndelineAlways && m_bUnderline)
    {
        wxFont fontLink = wxControl::GetFont();
        fontLink.SetUnderlined(TRUE);
        wxControl::SetFont(fontLink);
    }

    wxStaticText::SetForegroundColour(m_ColourActive);
    // ReDraw to correct the color
    Refresh();

    m_bActive = TRUE;
}

void wxHyperLink::OnDeActivate(wxMouseEvent &event)
{
    if (!m_bUndelineAlways && m_bUnderline)
    {
        wxFont fontLink = wxControl::GetFont();
        fontLink.SetUnderlined(FALSE);
        wxControl::SetFont(fontLink);
    }

    wxStaticText::SetForegroundColour(m_ColourNormal);
    // ReDraw to correct the color
    Refresh();  

    m_bActive = FALSE;
}

void wxHyperLink::OnClick(wxMouseEvent &event)
{
    // Open the link on button release only after a recorded button-down event.
    static bool bButtonDown = FALSE;

    if (event.ButtonDown(1))
        bButtonDown = TRUE;

    if (event.ButtonUp(1) && bButtonDown)
    {
        LoadLink();
        bButtonDown = FALSE;
    }
}

