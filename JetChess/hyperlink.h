/////////////////////////////////////////////////////////////////////////////
// Name:        hyperlink.h
// Purpose:     hyperlink, a clickable URL derived from wxStaticText
// Author:      Hans Van Leemputten
// Modified by:
// Created:     03/11/2001
// RCS-ID:      $Id: hyperlink.h,v 1.3 2006/02/08 05:02:10 spicerun Exp $
// Copyright:   (c) Hans Van Leemputten
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WXHYPERLINK_H
#define _WXHYPERLINK_H

/////////////////////////////////////////////////////////////////////////////
// wxHyperLink window

class wxHyperLink : public wxStaticText
{
    DECLARE_DYNAMIC_CLASS(wxHyperLink)

public:
    // Default constructor.
    wxHyperLink() {}
    // Create constructor
    // Param: parent    Pointer to the parent object. See wxStaticText Doc.
    // Param: id        Is the id the widget will respond to. See wxStaticText Doc.
    // Param: url       This string is the URL that will be loaded in the default web browser, if it starts with "mailto:" (at least on Windows) it will open the default e-mail client...
    // Param: label     This is the string that will be displayed... if empty this string is empty the URL string is displayed.
    // Param: normal    The color we display the hyper link in
    // Param: active    Color that is used when the mouse is over the widget.
    // Param: pos       The position on the parent window... See wxStaticText Doc.
    // Param: size      The size of the widget... See wxStaticText Doc.
    // Param: style     Extra styles on can give to wxStaticText. See wxStaticText Doc.
    // Param: name      The name of the widget.
    wxHyperLink(wxWindow *parent, wxWindowID id, const wxString &url, const wxString &label = wxT(""), const wxColour &normal = ( *wxBLUE), const wxColour &active = wxNullColour, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = 0, const wxString &name = wxT("HyperLink"));       

    // Destructor
    virtual         ~wxHyperLink();

    // This will create a usable widget... 
    // See Create constructor for mor info on the parameters.
    bool            Create(wxWindow *parent, wxWindowID id, const wxString &url, const wxString &label = wxT(""), const wxColour &normal = ( *wxBLUE), const wxColour &active = wxNullColour, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = 0, const wxString &name = wxT("HyperLinkt"));

    // Re-define this to allow to set 2 colours in with one base colour
    virtual bool    SetForegroundColour(const wxColour &colour);
    // Set Normal and Active colour of the Text.
    bool            SetColour(const wxColour &colourNormal, const wxColour &colourActive);
    // Set a new URL and/or lable. 
    // If no lable is set, url will be used as label.
    void            SetURL(const wxString &url, const wxString &label = wxT(""));
    // Set the text displayed with a line under it or not.
    void            SetUnderline(bool bUnderline, bool bAlways = TRUE);

    // Get URL and label
    void            GetURL(wxString &url, wxString &label);
    // Get if there is currently a line?
    bool            GetUnderline();

    // Load the default browser or email client...
    bool            LoadLink();

protected:
    unsigned char   CallcColour(unsigned char colour);

private: // All Events...
    void    OnActivate(wxMouseEvent &event);
    void    OnDeActivate(wxMouseEvent &event);
    void    OnClick(wxMouseEvent &event);

private: // All Private members
    // Normal color, used when text is not Active.
    wxColour    m_ColourNormal;
    // Active color, used when text is Active.
    wxColour    m_ColourActive;
    // The URL we will try to feed to the standard browser.
    wxString    m_strURL;
    // bools to know when to display a line under the text...
    bool        m_bUnderline;
    bool        m_bUndelineAlways;
    // Are we active?
    bool        m_bActive;

protected:
    DECLARE_EVENT_TABLE()
};

#endif // _WXHYPERLINK_H


