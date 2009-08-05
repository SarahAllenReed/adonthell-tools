/* 
 Copyright (C) 2009 Kai Sterker <kaisterker@linuxgames.com>
 Part of the Adonthell Project http://adonthell.linuxgames.com
 
 Mapedit is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 Mapedit is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Mapedit; if not, write to the Free Software 
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/** 
 * @file mapedit/gui_entity_dialog.h
 *
 * @author Kai Sterker
 * @brief View and edit entity properties.
 */

#ifndef GUI_ENTITY_DIALOG_H
#define GUI_ENTITY_DIALOG_H

#include "gui_modal_dialog.h"
#include "map_entity.h"

/**
 * A dialog to display and edit map entity properties.
 */
class GuiEntityDialog : public GuiModalDialog
{
public:
    typedef enum
    {
        READ_ONLY,
        ADD_ENTITY_TO_MAP,
        DUPLICATE_NAMED_ENTITY
    } Mode;
    
    /**
     * Show dialog to view and edit the given entity.
     * @param entity a map object.
     */
    GuiEntityDialog (MapEntity *entity, const GuiEntityDialog::Mode & mode);
    
    /**
     * Destructor.
     */
    virtual ~GuiEntityDialog();
    
    /**
     * Apply all changes made by the user to the underlying entity.
     * Called when the ok button has been pressed.
     */
    void applyChanges();
    
    /**
     * @name Entity attribute update.
     */
    //@{
    /**
     * Set the object type. Required for adding 
     * a new entity to the map.
     * @param type the placeable type.
     */
    void set_object_type (const world::placeable_type & type);

    /**
     * Set the entity type. Required for adding 
     * a new entity to the map.
     * @param type 'A', 'U' or 'S'.
     */
    void set_entity_type (const char & type);
    //@}
    
protected:
    /// the object being displayed or edited
    MapEntity *Entity;
    /// the object type
    world::placeable_type ObjType;
    /// the entities type
    char EntityType;
    /// the user interface
    GtkBuilder *Ui;
};

#endif