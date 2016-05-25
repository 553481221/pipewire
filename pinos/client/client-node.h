/* Pinos
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

#ifndef __PINOS_CLIENT_NODE_H__
#define __PINOS_CLIENT_NODE_H__

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _PinosClientNode PinosClientNode;
typedef struct _PinosClientNodeClass PinosClientNodeClass;
typedef struct _PinosClientNodePrivate PinosClientNodePrivate;

#include <pinos/client/introspect.h>
#include <pinos/client/node.h>

#define PINOS_TYPE_CLIENT_NODE                 (pinos_client_node_get_type ())
#define PINOS_IS_CLIENT_NODE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PINOS_TYPE_CLIENT_NODE))
#define PINOS_IS_CLIENT_NODE_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), PINOS_TYPE_CLIENT_NODE))
#define PINOS_CLIENT_NODE_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), PINOS_TYPE_CLIENT_NODE, PinosClientNodeClass))
#define PINOS_CLIENT_NODE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), PINOS_TYPE_CLIENT_NODE, PinosClientNode))
#define PINOS_CLIENT_NODE_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), PINOS_TYPE_CLIENT_NODE, PinosClientNodeClass))
#define PINOS_CLIENT_NODE_CAST(obj)            ((PinosClientNode*)(obj))
#define PINOS_CLIENT_NODE_CLASS_CAST(klass)    ((PinosClientNodeClass*)(klass))

/**
 * PinosClientNode:
 *
 * Pinos client node class.
 */
struct _PinosClientNode {
  PinosNodeClass object;

  PinosClientNodePrivate *priv;
};

/**
 * PinosClientNodeClass:
 * @set_state: called to change the current state of the client node
 *
 * Pinos client node class.
 */
struct _PinosClientNodeClass {
  PinosNodeClass parent_class;
};

/* normal GObject stuff */
GType               pinos_client_node_get_type                (void);

PinosClientNode *   pinos_client_node_new                     (PinosContext *context,
                                                               gpointer      id);

PinosContext *      pinos_client_node_get_context             (PinosClientNode *node);

G_END_DECLS

#endif /* __PINOS_CLIENT_NODE_H__ */