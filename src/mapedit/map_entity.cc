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
 * @file mapedit/map_entity.cc
 *
 * @author Kai Sterker
 * @brief Meta data for entities used on the map.
 */

#include <cctype>
#include <gtk/gtk.h>
#include <world/renderer.h>

#include "backend/gtk/screen_gtk.h"
#include "map_entity.h"
#include "map_data.h"

// ctor
MapEntity::MapEntity (world::entity *obj, const u_int32 & count)
{
    Location = NULL;
    Entity = obj;
    Object = obj->get_object ();
    RefCount = count;
    
    update_tags ();
}

// ctor
MapEntity::MapEntity (world::placeable *obj)
{
    Location = NULL;
    Entity = NULL;
    Object = obj;
    RefCount = 0;
    
    update_tags ();
}

// create or update entity
bool MapEntity::update_entity (const world::placeable_type & obj_type, const char & entity_type, const std::string & id)
{
    // get map associated with the object
    MapData *map = (MapData*) &(Object->map());    
    world::placeable *result = NULL;
    
    // create entity?
    switch (entity_type)
    {
        case 'A':
            result = map->add_entity(obj_type);
            break;
        case 'U':
            result = map->add_entity(obj_type, id);
            break;
        case 'S':
            result = map->add_entity(obj_type);
            if (result) map->add_entity(result, id);
            break;
        default:
            fprintf (stderr, "update_entity: unsupported entity type '%c'.\n", entity_type);
            break;
    }
    
    // success?
    if (result != NULL)
    {
        // load object data
        result->load (Object->filename());

        // cleanup (only if object not yet on the map)
        if (Entity == NULL)
        {
            delete Object;
        }
        
        // update entity ...
        Entity = map->getNewestEntity();
        // and newly created object
        Object = result;
        
        return true;
    }
        
    return false;
}

// decrease reference count
void MapEntity::decRef ()
{ 
    RefCount--; 
    
    // it's no longer present on map
    Location = NULL;
    
    // TODO: if RefCount == 0: fireEntityListChanged
}

// update tags
void MapEntity::update_tags ()
{
    Tags.clear ();

    std::string path = Object->filename();
    gchar *dir_name = g_path_get_dirname (path.c_str());
    gchar **tags = g_strsplit (dir_name, "/", -1);

    for (gchar **iter = tags; *iter != NULL; *iter++)
    {
        Tags.push_back (*iter);
    }

    g_strfreev (tags);
    g_free (dir_name);
}

// name of entity
gchar* MapEntity::get_name () const
{
    std::string path = Object->filename();
    gchar* name = g_path_get_basename (path.substr (0, path.rfind(".")).c_str());
    return name;
}

gchar* MapEntity::createNewId () const
{
    std::string base;
    if (Entity && Entity->has_name ())
    {
        base = *Entity->id();
    }
    else
    {
        gchar *file_name = get_name();
        base = std::string (file_name);
        g_free (file_name);
    }
 
    // strip any trailing number from id
    size_t pos = base.find_last_of ("-_");
    if (pos != base.npos)
    {
        bool isnum = true;
        for (size_t i = pos + 1; i < base.length(); i++)
        {
            isnum &= isdigit (base[i]);
        }
        if (isnum)
        {
            base = base.substr(0, pos);
        }
    }
    
    // get map associated with the object
    MapData *map = (MapData*) &(Object->map());
    
    // try to create unique id
    u_int32 i = 0;
    std::stringstream id (std::ios::out);
    do
    {
        id.str ("");
        id << base << "_" << ++i;
    }
    while (map->exists (id.str()));

    // return this id as newly allocated string
    return g_strdup_printf ("%s_%i", base.c_str(), i);
}

// id of entity
gchar* MapEntity::get_id () const
{
    gchar *id = NULL;
    if (Entity && Entity->has_name ())
    {
        id = g_strdup (Entity->id()->c_str());
    }
    return id;
}

// type of entity: (A)nonymous, (S)hared or (U)nique
gchar* MapEntity::get_entity_type () const
{
    if (Entity)
    {
        return g_strdup ((Entity->has_name() ? (((world::named_entity*)Entity)->is_unique() ? "U" : "S") : "A"));
    }
    
    return g_strdup ("-");
}

// picture of entity
GdkPixbuf *MapEntity::get_icon (const u_int32 & size) const
{
    static world::default_renderer renderer;

    // pixmap extends
    int l = Object->length();
    int h = Object->width() + Object->height();
    
    // create pixmap
    GdkPixmap* pixmap = gdk_pixmap_new (NULL, l, h, gdk_visual_get_best_depth ());
    
    // render entity onto a pixmap
    gfx::screen_surface_gtk *surface = (gfx::screen_surface_gtk*) gfx::screen::get_surface();

    // determine background color
    u_int32 color = 0xFFFFFFFF;
    if (is_on_map())
    {
        // present on map, but unused
        if (RefCount == 0)
        {
            color = surface->map_color (0x80, 0xC8, 0xFE, 0xFF);
        }
    }
    else
    {
        // not present on map yet
        color = surface->map_color (0xFE, 0xB3, 0x80, 0xFF);
    }
    
    surface->set_drawable (pixmap);
    surface->set_alpha (255, true);
    surface->resize (l, h);
    surface->fillrect (0, 0, l, h, color);
    
    // properly render the object
    world::vector3<s_int32> min (0, 0, 0), max (Object->length(), Object->width(), Object->height());
    world::named_entity ety (Object, "", false);
    world::chunk_info ci (&ety, min, max);
    std::list <world::chunk_info*> object_list;
    object_list.push_back (&ci);
    gfx::drawing_area da (0, 0, l, h);
    renderer.render (0, Object->height(), object_list, da, surface);
    
    // thumbnail of entity
    GdkPixbuf *pixbuf = gdk_pixbuf_get_from_drawable (NULL, pixmap, gdk_colormap_get_system (), 0, 0, 0, 0, l, h);
    int nl = l > h ? size : ((float) l / h) * size;
    int nh = h > l ? size : ((float) h / l) * size;
    GdkPixbuf *icon = gdk_pixbuf_scale_simple (pixbuf, nl, nh, GDK_INTERP_BILINEAR);
    
    // cleanup
    g_object_unref (pixmap);
    g_object_unref (pixbuf);
    
    return icon;
}

// return type of placeable
world::placeable_type MapEntity::get_object_type () const
{
    if (Entity)
    {
        // we know the type
        return Entity->get_object()->type();
    }
    
    // we don't know the type yet, but we can make an educated guess
    if (std::find (Tags.begin(), Tags.end(), std::string ("char")) != Tags.end())
    {
        return world::CHARACTER;
    }
    if (std::find (Tags.begin(), Tags.end(), std::string ("item")) != Tags.end())
    {
        return world::ITEM;
    }
    
    return world::OBJECT;
}
