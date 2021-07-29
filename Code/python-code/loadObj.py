import numpy as np
import glm
import mesh

def loadObj( filename ):
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
        elif values[0] == 'f':
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

    vertex_index  = [y for x in vertex_index  for y in x]
    texture_index = [y for x in texture_index for y in x]
    normal_index  = [y for x in normal_index  for y in x]
'''
    for i in self.vertex_index:
        self.model.extend(self.vert_coords[i])

    for i in self.texture_index:
        self.model.extend(self.text_coords[i])

    for i in self.normal_index:
        self.model.extend(self.norm_coords[i])

    self.model = np.array(self.model, dtype='float32')
'''
    m = mesh.Mesh ()
    
    
    return m
