/* ide-debugger-controls.c
 *
 * Copyright 2017-2019 Christian Hergert <chergert@redhat.com>
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

#include "ide-debugger-controls.h"

struct _IdeDebuggerControls
{
  GtkBin parent_instance;
};

G_DEFINE_FINAL_TYPE (IdeDebuggerControls, ide_debugger_controls, GTK_TYPE_REVEALER)

static void
ide_debugger_controls_class_init (IdeDebuggerControlsClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/plugins/debuggerui/ide-debugger-controls.ui");
  gtk_widget_class_set_css_name (widget_class, "idedebuggercontrols");
}

static void
ide_debugger_controls_init (IdeDebuggerControls *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
