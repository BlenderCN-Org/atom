import bpy
import mathutils
import sys
import time
import os
import json

filename_ext = ".m3d"


def to_point(v):
    return mathutils.Vector((v.x / v.w, v.y / v.w, v.z / v.w)).to_3d()


def has_texcoords(ob):
    return len(ob.data.uv_layers) > 0


def has_bones(ob):
    return ob.parent != None and ob.parent.type == 'ARMATURE'


def has_triangles_only(mesh):
    for poly in mesh.polygons:
        if (poly.loop_total != 3):
            return False

    return True


def export_mesh(ob, me, export_bones):
    vertices = []
    normals = []
    indices = []

    # zjednoduseny export len vrcholov, normal a indexov
    # bez zdvojovania vrcholov (netreba, uv sa neexportuje)
    for v in me.vertices:
        vertices.append(v.co.x)
        vertices.append(v.co.y)
        vertices.append(v.co.z)
        normals.append(v.normal.x)
        normals.append(v.normal.y)
        normals.append(v.normal.z)

    for poly in me.polygons:
        for i in poly.vertices:
            indices.append(i)

    if export_bones:
        pose = ob.parent.pose

        bone_list = {}
        i = 0

        for name, bone in pose.bones.items():
            print('Exporting bone ' + name)
            start = bone.bone.head_local
            b = {}
            b['start'] = [start.x, start.y, start.z]
            b['index'] = i

            if bone.parent != None:
                b['parent'] = bone.parent.name

            bone_list[name] = b
            i = i + 1


    vertex_stream = { 'type' : 'f32', 'data' : vertices }
    normal_stream = { 'type' : 'f32', 'data' : normals }
    index_stream  = { 'type' : 'u32', 'data' : indices }

    mesh = {
      #'arrays' : {
      #  "vertices" : vertex_stream,
      #  "normals" : normal_stream,
      #  "indices" : index_stream
      #},
      'skeleton' : {
        'bones' : bone_list
      }
    }
    return mesh


def export_object_to_file(ob, filename):
    """Export object data (vertices/normals/uv/vertex bones/skeleton to file.

    This function duplicate active object, convert it to triangles, then export
    it to the file.
    Using write_* functions defined above. On exit and on exception as well
    the duplicated object is deleted and original selection is restored.
    ob is unused, why???
    """
    t1 = time.clock()
    current_object = bpy.context.active_object
    # duplicate the object
    bpy.ops.object.duplicate()

    try:
        # convert all faces (of duplicated object) to triangles
        bpy.ops.object.mode_set(mode='EDIT')
        bpy.ops.mesh.select_all(action='SELECT')
        bpy.ops.mesh.quads_convert_to_tris()
        bpy.ops.object.mode_set(mode='OBJECT')
        bpy.context.scene.update()

        # ob je kopia originalu
        ob = bpy.types.Object
        ob = bpy.context.active_object
        me = ob.data

        matrix = ob.matrix_world

        if not has_triangles_only(me):
            raise Exception("The mesh contains non triangles")

        export_status = [{'INFO'}, 'Exported ']
        status = 'OK'

        # aplikuj transformacie a prepocitaj normaly
        me.transform(matrix)
        me.calc_normals()

        mesh = export_mesh(ob, me, has_bones(ob) and ob.atom.export_bones)

        with open(filename, "w+") as output:
            # compact format
            #data = json.dumps({ 'mesh' : mesh }, separators=(',', ':'))
            data = json.dumps({ 'mesh' : mesh }, indent=2)
            output.write(data)

        t2 = time.clock()
        print("Transform {0}s".format(t2 - t1))
        export_status = [{'INFO'}, status]

    except Exception as e:
        export_status = [{'ERROR'}, e.args[0]]

    finally:
        # delete duplicate object and set previous object
        bpy.ops.object.delete()
        bpy.ops.object.select_all(action='DESELECT')
        current_object.select = True
        bpy.context.scene.objects.active = current_object

    return export_status


class AtomExportObject(bpy.types.Operator):
    bl_idname = "atom.export_object"
    bl_label = "Export selected object"

    def execute(self, context):
        ob = context.active_object
        print('Object name ', ob.name)
        filename = os.path.expanduser(context.scene.atom.base_dir + "/mesh/"
                 + ob.name + filename_ext)
        print('Filename name ', filename)
        print(filename)
        if (os.path.exists(os.path.dirname(filename)) == False):
            self.report({'ERROR'}, 'Mesh directory doesn\'t exists')
            return {'CANCELLED'}

        status = export_object_to_file(ob, filename)

        self.report(status[0], status[1])
        return {'FINISHED'}
