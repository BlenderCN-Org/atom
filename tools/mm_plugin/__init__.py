bl_info = {
    "name": "MM Engine Plugin",
    "description": "Utilities for MM Engine",
    "author": "majo33@gmail.com",
    "version": (0, 20),
    "blender": (2, 6, 3),
    "location": "View3D > UI panel > MM Engine",
    "category": "Plugins",
    "warning": "Beta"
}

# To support reload properly, try to access a package var
# if it's there, reload everything
if "bpy" in locals():
    import imp
    if "mm_export_object" in locals():
        imp.reload(mm_export_object)
    print("Reloaded MMPlugin")
else:
    from . import mm_export_object
    print("Imported MMPlugin")

import os
import bpy
import bpy.utils
import bpy.props
from . import mm_export_object
from bpy.props import BoolProperty, StringProperty


default_base_dir = "~/dev/mm/data/"


class MMSceneSettings(bpy.types.PropertyGroup):
    base_dir = bpy.props.StringProperty(name="Data directory",
                                        default=default_base_dir)


class MMObjectSettings(bpy.types.PropertyGroup):
    export_uv = bpy.props.BoolProperty(name="Export UV",
        description="True or False?", default=True)
    export_normals = bpy.props.BoolProperty(name="Export normals",
        description="True or False?", default=True)
    export_bones = bpy.props.BoolProperty(name="Export skeleton",
        description="True or False?", default=True)
    type_items = [
        ('Mesh 3D', 'Mesh3D', 'This object will be exported to the mm3 file'),
        ('Mesh 2D', 'Mesh2D', 'This object will be exported to the mm2 file'),
        ('Physic', 'Physic', 'This object will be exporte to the mph file')]
    object_type = bpy.props.EnumProperty(name="Type",
        description="Object type", items=type_items)


class MMPluginPanel(bpy.types.Panel):
    bl_label = "MM Engine Panel"
    bl_idname = "OBJECT_OT_mm_plugin_panel"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"

    @classmethod
    def poll(cls, context):
        ob = context.object
        return (ob is not None and ob.type == "MESH")

    def draw(self, context):
        if (context.object is not None):
            layout = self.layout
            layout.prop(context.object, "name")
            layout.prop(context.scene.mm, "base_dir")
            layout.prop(context.object.mm, "object_type")
            layout.prop(context.object.mm, "export_normals")

            if mm_export_object.has_texcoords(context.object.data.uv_layers):
                layout.prop(context.object.mm, "export_uv")
            if mm_export_object.has_bones(context.object):
                layout.prop(context.object.mm, "export_bones")
            layout.operator("mm.export_object")


def register():
    bpy.utils.register_module(__name__)
    bpy.types.Object.mm = bpy.props.PointerProperty(type=MMObjectSettings)
    bpy.types.Scene.mm = bpy.props.PointerProperty(type=MMSceneSettings)


def unregister():
    bpy.utils.unregister_module(__name__)

if __name__ == "__main__":
    register()
