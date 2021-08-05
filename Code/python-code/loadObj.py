"""
    loadObj - module to load .obj geometry files into Mesh objects to be rendered
"""
import glm
import Mesh

def loadObj( filename, scale = 1.0 ):
    vert_coords   = []
    text_coords   = []
    norm_coords   = []
    vertex_index  = []
    texture_index = []
    normal_index  = []

    for line in open(filename, 'r'):
        if line.startswith('#'):
            continue
        values = line.split()
        if not values:
            continue

        if values[0] == 'v':
            vert_coords.append(values[1:4])
        elif values[0] == 'vt':
            text_coords.append(values[1:3])
        elif values[0] == 'vn':
            norm_coords.append(values[1:4])
        elif values[0] == 'f':		# f v0/t0/n0 v1/t1/n1 v2/t2/n2
            face_i = []
            text_i = []
            norm_i = []
            for v in values[1:4]:
                w = v.split('/')
                face_i.append(int(w[0])-1)
                text_i.append(int(w[1])-1)
                norm_i.append(int(w[2])-1)
            vertex_index.append  ( face_i )
            texture_index.append ( text_i )
            normal_index.append  ( norm_i )

    vertices = []		# keep here pos, tex, normal
    index    = dict ()	# map (pos,tex,normal) into index in vertices
    faces    = []		# face indices

    for i in range(len(vertex_index)):
        for j in range(3):
            vi = vert_coords[vertex_index  [i][j]]
            ti = text_coords[texture_index [i][j]]
            ni = norm_coords[normal_index  [i][j]]
            vertex = ( float(vi[0]), float(vi [1]), float(vi[2]), float(ti[0]), float(ti[1]), float(ni[0]), float(ni[1]), float(ni[2]) )
            if vertex in index:		# we already have this key, use it's index
                k = index [vertex]
            else:					# new vertex
                k = len(vertices)
                index[vertex] = k

                vertices.append ( vertex )

            faces.append ( k )

    mesh = Mesh.Mesh ()
    for v in vertices:
        #print ( '--v---', type(v), v )
        mesh.addVertex ( scale * glm.vec3 ( v[0], v[1], v[2] ), glm.vec2 ( v[3], v[4] ), glm.vec3 ( v[5], v[6], v [7] ) )

    for i in range(len(faces) // 3):
        mesh.addFace ( faces[3*i], faces[3*i + 1], faces [3*i + 2] )

    mesh.create ()
    return mesh
