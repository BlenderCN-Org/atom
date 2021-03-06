import bpy
import mathutils
import sys
import time
import os
import json
import collections
import array

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


def get_fps():
    """Scene fps set in the Render Panel
    """
    return bpy.context.scene.render.fps / bpy.context.scene.render.fps_base


def calculate_bone_matrix(bone):
    assert bone != None
    return calculate_bone_matrix(bone.parent) * bone.matrix if bone.parent != None else bone.matrix


def export_skeleton(ob, me):
    """
    Vracia None alebo named tuple (bone_weight, bone_index, skeleton)
    """
    ar = ob.parent
    pose = ob.parent.pose
    vertex_groups = ob.vertex_groups

    # build bone mapping table name => index
    # the mapping table contains only 'deform' bones
    bone_index = dict()
    i = 0

    for name, bone in pose.bones.items():
        # skip helper bones (IK, MCM, ...)
        if bone.bone.use_deform == False:
            continue

        bone_index[name] = i
        i = i + 1

    bone_list = {}

    x_axis = mathutils.Vector((1, 0, 0))
    y_axis = mathutils.Vector((0, 1, 0))
    z_axis = mathutils.Vector((0, 0, 1))

    i = 0
    for name, bone in pose.bones.items():
        if bone.bone.use_deform == False:
            continue

        print('Exporting bone ' + name)

        head = ar.matrix_world * bone.bone.head
        tail = ar.matrix_world * bone.bone.tail
        head_local = ar.matrix_world * bone.bone.head_local
        tail_local = ar.matrix_world * bone.bone.tail_local
        b = {}
        b['head'] = [head.x, head.y, head.z]
        b['tail'] = [tail.x, tail.y, tail.z]
        b['head_local'] = [head_local.x, head_local.y, head_local.z]
        b['tail_local'] = [tail_local.x, tail_local.y, tail_local.z]
        b['index'] = i

        matrix = calculate_bone_matrix(ar.data.bones[name])

        bone_x_axis = matrix * x_axis
        bone_y_axis = matrix * y_axis
        bone_z_axis = matrix * z_axis

        b['x'] = [bone_x_axis.x, bone_x_axis.y, bone_x_axis.z]
        b['y'] = [bone_y_axis.x, bone_y_axis.y, bone_y_axis.z]
        b['z'] = [bone_z_axis.x, bone_z_axis.y, bone_z_axis.z]

        if bone.parent != None:
            b['parent'] = bone_index[bone.parent.name]

        bone_list[name] = b

        i = i + 1

    #weight = array.array('f')   # 4 hodnoty float na vertex
    #index = array.array('I')    # 4 indexy (0-255) zakodovane do u32
    weight = list()   # 4 hodnoty float na vertex
    index = list()    # 4 indexy (0-255) zakodovane do u32

    for v in me.vertices:
        packed_index = 0
        used = 0
        shift = 0

        for group in v.groups:
            vertex_group_name = vertex_groups[group.group].name
            bindex = bone_index[vertex_group_name]

            if group.weight > 0:
              weight.append(group.weight)
              packed_index = packed_index | (bindex << shift)
              shift = shift + 8
              used = used + 1

        while used < 4:
            used = used + 1
            weight.append(0)

        index.append(packed_index)

    skeleton = {
        'bones' : bone_list
    }

    Result = collections.namedtuple('ExportSkeletonResult', 'skeleton weight index')
    return Result(skeleton, weight, index)


def write_topology(ob, me, topology):
    # build mapping table: edge -> triangles (1 or 2 triangles)
    i = 0
    table = dict()
    for triangle in me.polygons:
        for edge in triangle.edge_keys:
            n = table.get(edge)
            
            if n != None:
                n.append(i)
            else:
                table[edge] = [i]
            
        i = i + 1
    # write neighbor topology using mapping table
    i = 0
    for triangle in me.polygons:
        for edge in triangle.edge_keys:
            n = table[edge]
            count = len(n)
            if count == 2:
                if n[0] == i:
                    topology.append(n[1])
                elif n[1] == i:
                    topology.append(n[0])
                else:
                    raise Exception("Can't find neighbor for edge {0}".format(edge))
            elif count == 1:
                topology.append(-1)
            else:
                raise Exception("Some edge has more than 2 adjacend faces")
        
        i = i + 1
    # mesh has closed topology when each edge has two adjacent triangles
    if ob.atom.closed_topology:
        for i in topology:
            if i < 0:
                raise Exception("The object has not closed topology")


def export_mesh(ob, me, export_bones):
    """require mesh composed of triangles, not quads, ...
    """
    vertices = []
    normals = []
    indices = []
    topology = []

    # zjednoduseny export len vrcholov, normal a indexov
    # bez zdvojovania vrcholov (netreba, uv sa neexportuje)
    for v in me.vertices:
        vertices.append(v.co.x)
        vertices.append(v.co.y)
        vertices.append(v.co.z)
        normals.append(v.normal.x)
        normals.append(v.normal.y)
        normals.append(v.normal.z)

    for triangle in me.polygons:
        for i in triangle.vertices:
            indices.append(i)

    write_topology(ob, me, topology)

    vertex_stream = { 'type' : 'f32', 'data' : vertices }
    normal_stream = { 'type' : 'f32', 'data' : normals }
    index_stream  = { 'type' : 'u32', 'data' : indices }
    topology_stream  = { 'type' : 'i32', 'data' : topology }

    arrays = dict()
    arrays['vertices'] = vertex_stream
    arrays['normals'] = normal_stream
    arrays['indices'] = index_stream
    arrays['topology'] = topology_stream

    mesh = dict()
    mesh['arrays'] = arrays

    if export_bones:
        skeleton = export_skeleton(ob, me)
        assert skeleton != None
        assert skeleton.skeleton != None
        #assert skeleton.weight != None
        #assert skeleton.index != None

        weight = { 'type' : 'f32', 'data' : skeleton.weight }
        index = { 'type' : 'u32', 'data' : skeleton.index }

        mesh['skeleton'] = skeleton.skeleton
        arrays['bone_weight'] = weight
        arrays['bone_index'] = index

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
    # apply all modifiers
    bpy.ops.object.convert(target='MESH', keep_original=False)

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
        print("Total time {0}s".format(t2 - t1))
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
    bl_label = "Export object"

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
