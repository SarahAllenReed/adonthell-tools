/*
 Copyright (C) 2009 Kai Sterker <kaisterker@linuxgames.com>
 Part of the Adonthell Project http://adonthell.linuxgames.com
 
 Adonthell is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 Adonthell is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Adonthell; if not, write to the Free Software 
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/** 
 * @file questedit/gui_questedit.h
 *
 * @author Kai Sterker
 * @brief The modeller main window.
 */


#ifndef GUI_QUESTEDIT_H
#define GUI_QUESTEDIT_H

/**
 * The application main window.
 */
class GuiQuestedit
{
public:
    /**
     * Create new instance of the GUI
     */
    GuiQuestedit();
    
    /**
     * Get the application main window.
     * @return the main window.
     */
    GtkWidget *getWindow () const { return Window; }
    
private:
    /// the main window
    GtkWidget *Window;
    /// the user interface
    GtkBuilder *Ui;
};

#endif
