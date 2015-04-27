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

#include "client/pulsevideo.h"

#include "client/pv-enumtypes.h"
#include "client/pv-source.h"
#include "client/pv-source-output.h"

#include "server/pv-client.h"

#include "dbus/org-pulsevideo.h"

struct _PvClientPrivate
{
  PvDaemon *daemon;
  gchar *sender;
  gchar *object_path;

  PvClient1 *client1;

  GHashTable *source_outputs;
};

#define PV_CLIENT_GET_PRIVATE(obj)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((obj), PV_TYPE_CLIENT, PvClientPrivate))

G_DEFINE_TYPE (PvClient, pv_client, G_TYPE_OBJECT);

enum
{
  PROP_0,
  PROP_DAEMON,
  PROP_SENDER,
  PROP_OBJECT_PATH,
};

static void
pv_client_get_property (GObject    *_object,
                         guint       prop_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
  PvClient *client = PV_CLIENT (_object);
  PvClientPrivate *priv = client->priv;

  switch (prop_id) {
    case PROP_DAEMON:
      g_value_set_object (value, priv->daemon);
      break;

    case PROP_SENDER:
      g_value_set_string (value, priv->sender);
      break;

    case PROP_OBJECT_PATH:
      g_value_set_string (value, priv->object_path);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (client, prop_id, pspec);
      break;
  }
}

static void
pv_client_set_property (GObject      *_object,
                         guint         prop_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
  PvClient *client = PV_CLIENT (_object);
  PvClientPrivate *priv = client->priv;

  switch (prop_id) {
    case PROP_DAEMON:
      priv->daemon = g_value_dup_object (value);
      break;

    case PROP_SENDER:
      priv->sender = g_value_dup_string (value);
      break;

    case PROP_OBJECT_PATH:
      priv->object_path = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (client, prop_id, pspec);
      break;
  }
}

static void
client_register_object (PvClient *client, const gchar *prefix)
{
  PvClientPrivate *priv = client->priv;
  PvDaemon *daemon = priv->daemon;
  PvObjectSkeleton *skel;
  gchar *name;

  name = g_strdup_printf ("%s/client", prefix);
  skel = pv_object_skeleton_new (name);
  g_free (name);

  priv->client1 = pv_client1_skeleton_new ();
  pv_client1_set_name (priv->client1, priv->sender);
  pv_object_skeleton_set_client1 (skel, priv->client1);

  g_free (priv->object_path);
  priv->object_path = pv_daemon_export_uniquely (daemon, G_DBUS_OBJECT_SKELETON (skel));
}

static void
client_unregister_object (PvClient *client)
{
  PvClientPrivate *priv = client->priv;
  PvDaemon *daemon = priv->daemon;

  g_hash_table_unref (priv->source_outputs);
  g_clear_object (&priv->client1);

  pv_daemon_unexport (daemon, priv->object_path);
  g_free (priv->object_path);
}

static void
pv_client_finalize (GObject * object)
{
  PvClient *client = PV_CLIENT (object);

  client_unregister_object (client);

  G_OBJECT_CLASS (pv_client_parent_class)->finalize (object);
}

static void
pv_client_constructed (GObject * object)
{
  PvClient *client = PV_CLIENT (object);
  PvClientPrivate *priv = client->priv;

  client_register_object (client, priv->object_path);

  G_OBJECT_CLASS (pv_client_parent_class)->constructed (object);
}

static void
pv_client_class_init (PvClientClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (PvClientPrivate));

  gobject_class->finalize = pv_client_finalize;
  gobject_class->set_property = pv_client_set_property;
  gobject_class->get_property = pv_client_get_property;
  gobject_class->constructed = pv_client_constructed;

  g_object_class_install_property (gobject_class,
                                   PROP_DAEMON,
                                   g_param_spec_object ("daemon",
                                                        "Daemon",
                                                        "The daemon",
                                                        PV_TYPE_DAEMON,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT_ONLY |
                                                        G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class,
                                   PROP_SENDER,
                                   g_param_spec_string ("sender",
                                                        "Sender",
                                                        "The sender",
                                                        NULL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT_ONLY |
                                                        G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class,
                                   PROP_OBJECT_PATH,
                                   g_param_spec_string ("object-path",
                                                        "Object Path",
                                                        "The object path",
                                                        NULL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT_ONLY |
                                                        G_PARAM_STATIC_STRINGS));
}

static void
pv_client_init (PvClient * client)
{
  PvClientPrivate *priv = client->priv = PV_CLIENT_GET_PRIVATE (client);

  priv->source_outputs = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_unref);
}


/**
 * pv_client_new:
 * @daemon: a #PvDaemon
 * @prefix: a prefix
 *
 * Make a new #PvClient object and register it to @daemon under the @prefix.
 *
 * Returns: a new #PvClient
 */
PvClient *
pv_client_new (PvDaemon * daemon, const gchar *sender, const gchar *prefix)
{
  g_return_val_if_fail (PV_IS_DAEMON (daemon), NULL);
  g_return_val_if_fail (g_variant_is_object_path (prefix), NULL);

  return g_object_new (PV_TYPE_CLIENT, "daemon", daemon, "sender", sender, "object-path", prefix, NULL);
}

/**
 * pv_client_get_object_path:
 * @client: a #PvClient
 *
 * Get the object path of @client.
 *
 * Returns: the object path of @client
 */
const gchar *
pv_client_get_object_path (PvClient *client)
{
  PvClientPrivate *priv;

  g_return_val_if_fail (PV_IS_CLIENT (client), NULL);
  priv = client->priv;

  return priv->object_path;
}

