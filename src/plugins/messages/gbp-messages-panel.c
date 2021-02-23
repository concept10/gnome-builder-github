/* gbp-messages-panel.c
 *
 * Copyright 2018-2019 Christian Hergert <chergert@redhat.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#define G_LOG_DOMAIN "gbp-messages-panel"

#include <libide-editor.h>
#include <libide-terminal.h>

#include "gbp-messages-panel.h"

struct _GbpMessagesPanel
{
  DzlDockWidget parent_instance;

  DzlSignalGroup *signals;

  GtkScrollbar *scrollbar;
  IdeTerminal  *terminal;
};

G_DEFINE_TYPE (GbpMessagesPanel, gbp_messages_panel, DZL_TYPE_DOCK_WIDGET)

static char *
ensure_crlf (const char *in_s)
{
  GString *s = g_string_new (NULL);
  const char *p = in_s;
  const char *prev_p = in_s;

  g_assert (in_s != NULL);

  while ((p = strchr (p, '\n')) != NULL)
    {
      /* This is to avoid a segfault that would occur if trying to deref in_s - 1. */
      if (p == in_s)
        {
          g_string_append (s, "\r\n");
          ++p;
        }
      else
        {
          if (p[-1] == '\r')
            {
              ++p;
              g_string_append_len (s, prev_p, (gssize)(p - prev_p));
            }
          else
            {
              g_string_append_len (s, prev_p, (gssize)(p - prev_p));
              g_string_append (s, "\r\n");
              ++p;
            }
        }

      prev_p = p;
    }

  return g_string_free (s, FALSE);
}

static void
gbp_messages_panel_log_cb (GbpMessagesPanel *self,
                           GLogLevelFlags    log_level,
                           const gchar      *domain,
                           const gchar      *message,
                           IdeContext       *context)
{
  g_autofree char *out_message = NULL;

  g_assert (GBP_IS_MESSAGES_PANEL (self));
  g_assert (message != NULL);
  g_assert (IDE_IS_CONTEXT (context));

  out_message = ensure_crlf (message);

  vte_terminal_feed (VTE_TERMINAL (self->terminal), out_message, -1);
  vte_terminal_feed (VTE_TERMINAL (self->terminal), "\r\n", 2);
  dzl_dock_item_needs_attention (DZL_DOCK_ITEM (&self->parent_instance));
  gtk_widget_show (GTK_WIDGET (self));
}

#if 0
static gboolean
do_log (gpointer data)
{
  ide_context_warning (data, "(some log message here)\nfoo\nbaz");
  return G_SOURCE_CONTINUE;
}
#endif

static void
gbp_messages_panel_set_context (GtkWidget  *widget,
                                IdeContext *context)
{
  GbpMessagesPanel *self = (GbpMessagesPanel *)widget;

  g_assert (GBP_IS_MESSAGES_PANEL (self));
  g_assert (!context || IDE_IS_CONTEXT (context));

  dzl_signal_group_set_target (self->signals, context);

#if 0
  if (context != NULL)
    g_timeout_add (1000, do_log, context);

  g_assert (g_strcmp0 (ensure_crlf ("\nfoo\r\nbar\nbaz\r\r\n"), "\r\nfoo\r\nbar\r\nbaz\r\r\n") == 0);
#endif
}

static void
gbp_messages_panel_destroy (GtkWidget *widget)
{
  GbpMessagesPanel *self = (GbpMessagesPanel *)widget;

  g_clear_object (&self->signals);

  GTK_WIDGET_CLASS (gbp_messages_panel_parent_class)->destroy (widget);
}

static void
gbp_messages_panel_class_init (GbpMessagesPanelClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  widget_class->destroy = gbp_messages_panel_destroy;

  gtk_widget_class_set_template_from_resource (widget_class, "/plugins/messages/gbp-messages-panel.ui");
  gtk_widget_class_bind_template_child (widget_class, GbpMessagesPanel, scrollbar);
  gtk_widget_class_bind_template_child (widget_class, GbpMessagesPanel, terminal);
}

static void
gbp_messages_panel_init (GbpMessagesPanel *self)
{
  GtkAdjustment *vadj;

  gtk_widget_init_template (GTK_WIDGET (self));

  ide_widget_set_context_handler (GTK_WIDGET (self), gbp_messages_panel_set_context);

  vadj = gtk_scrollable_get_vadjustment (GTK_SCROLLABLE (self->terminal));
  gtk_range_set_adjustment (GTK_RANGE (self->scrollbar), vadj);

  self->signals = dzl_signal_group_new (IDE_TYPE_CONTEXT);

  dzl_signal_group_connect_object (self->signals,
                                   "log",
                                   G_CALLBACK (gbp_messages_panel_log_cb),
                                   self,
                                   G_CONNECT_SWAPPED);
}
