/* Pulsevideo
 * Copyright (C) 2015 Wim Taymans <wim.taymans@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __PV_SOURCE_H__
#define __PV_SOURCE_H__

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

typedef struct _PvSource PvSource;
typedef struct _PvSourceClass PvSourceClass;
typedef struct _PvSourcePrivate PvSourcePrivate;

#include "client/pv-source-output.h"

#define PV_TYPE_SOURCE                 (pv_source_get_type ())
#define PV_IS_SOURCE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PV_TYPE_SOURCE))
#define PV_IS_SOURCE_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), PV_TYPE_SOURCE))
#define PV_SOURCE_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), PV_TYPE_SOURCE, PvSourceClass))
#define PV_SOURCE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), PV_TYPE_SOURCE, PvSource))
#define PV_SOURCE_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), PV_TYPE_SOURCE, PvSourceClass))
#define PV_SOURCE_CAST(obj)            ((PvSource*)(obj))
#define PV_SOURCE_CLASS_CAST(klass)    ((PvSourceClass*)(klass))

/**
 * PvSourceState:
 * @PV_SOURCE_STATE_ERROR: the source is in error
 * @PV_SOURCE_STATE_SUSPENDED: the source is suspended, the device might
 *                             be closed
 * @PV_SOURCE_STATE_INIT: the source is initializing, it opens the device
 *                        and gets the device capabilities
 * @PV_SOURCE_STATE_IDLE: the source is running but there is no active
 *                        source-output
 * @PV_SOURCE_STATE_RUNNING: the source is running.
 *
 * The different source states
 */
typedef enum {
  PV_SOURCE_STATE_ERROR = 0,
  PV_SOURCE_STATE_SUSPENDED = 1,
  PV_SOURCE_STATE_INIT = 2,
  PV_SOURCE_STATE_IDLE = 3,
  PV_SOURCE_STATE_RUNNING = 4,
} PvSourceState;
/**
 * PvSource:
 *
 * Pulsevideo source object class.
 */
struct _PvSource {
  GObject object;

  PvSourcePrivate *priv;
};

/**
 * PvSourceClass:
 * @get_capabilities: called to get a list of supported formats from the source
 * @set_state: called to change the current state of the source
 * @create_source_output: called to create a new source-output object
 * @release_source_output: called to release a source-output object
 *
 * Pulsevideo source object class.
 */
struct _PvSourceClass {
  GObjectClass parent_class;

  GVariant *       (*get_capabilities) (PvSource *source, GVariant *props);

  gboolean         (*set_state)  (PvSource *source, PvSourceState);

  PvSourceOutput * (*create_source_output)  (PvSource *source, GVariant *props, const gchar *prefix);
  gboolean         (*release_source_output) (PvSource *source, PvSourceOutput *output);
};

/* normal GObject stuff */
GType            pv_source_get_type               (void);

void             pv_source_set_manager            (PvSource *source, GDBusObjectManagerServer *manager);

GVariant *       pv_source_get_capabilities       (PvSource *source, GVariant *props);

gboolean         pv_source_set_state              (PvSource *source, PvSourceState state);
void             pv_source_update_state           (PvSource *source, PvSourceState state);

PvSourceOutput * pv_source_create_source_output   (PvSource *source, GVariant *props, const gchar *prefix);
gboolean         pv_source_release_source_output  (PvSource *source, PvSourceOutput *output);

G_END_DECLS

#endif /* __PV_SOURCE_H__ */

