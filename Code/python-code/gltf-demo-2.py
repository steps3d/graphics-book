import sys
import os
import os.path
import glm
import numpy
from OpenGL.GL import *
from pygltflib import GLTF2
import Window
import Program
import Texture
import Mesh
import Buffer

class Node:
    def __init__ ( self, node ):
        self.name        = node.name
        self.mesh        = node.mesh
        self.children    = node.children
        self.mesh        = None
        self.matrix      = glm.mat4 ( 1.0 )
        self.rotation    = None
        self.translation = None
        self.scale       = None
        
        if "matrix" in node.__dict__ and node.matrix:
            self.matrix = glm.mat4 ( node.matrix )

        if "rotation" in node.__dict__ and node.rotation:
            self.rotation = glm.quat ( node.rotation )

        if "translation" in node.__dict__ and node.translation:
            self.translation = glm.vec3 ( node.translation )

        if "scale" in node.__dict__ and node.scale:
            self.scale = glm.vec3 ( node.scale )

    def addChild ( self, node ):
        self.children.append ( node )
        
    def setMesh ( self, mesh ):
        self.mesh = mesh
        
    def render ( self, shader, matrix, name ):      
        matrix = matrix * self.matrix
        shader.setUniformMat ( name, matrix )
        self.mesh.render ()
        
        for child in self.children:
            child.render ( shader, matrix, name )
        
"""
	Load all buffers into VBO (vertex and element)
"""
def loadBuffers ( gltf, index ):
    indexBuffer = attrToBuffer ( gltf, index )

    for buffer in gltf.buffers:
        buffer.blob   = gltf.load_file_uri ( buffer.uri )
        target        = GL_ARRAY_BUFFER if buffer != indexBuffer else GL_ELEMENT_ARRAY_BUFFER
        buffer.buffer = Buffer.Buffer ( target, numpy.frombuffer ( buffer.blob, dtype = numpy.uint8 ), dtype = numpy.uint8 )

def loadBufferBlob ( gltf, buffer ):
    buffer.blob = gltf.load_file_uri ( buffer.uri )

def attrToBuffer ( gltf, attr ):
    accessor = gltf.accessors[attr]                          	# acessor.componentType (5123 - GL_FLOAT, ...), count, type ("SCALAR", "VEC3"), min, max
    view     = gltf.bufferViews[accessor.bufferView]        	# view.target (34962 - ARRAY_BUFFER, 34963 - ELEMENT_ARRAY_BUFFER), byteStride for interleaving data
    
    return gltf.buffers [view.buffer]

def attrProps ( gltf, attr ):
    if attr is None:
        return None

    accessor      = gltf.accessors[attr]                          	# acessor.componentType (5123 - GL_FLOAT, ...), count, type ("SCALAR", "VEC3"), min, max
    view          = gltf.bufferViews[accessor.bufferView]        	# view.target (34962 - ARRAY_BUFFER, 34963 - ELEMENT_ARRAY_BUFFER), byteStride for interleaving data
    numComponents = getAttrNumComponents ( gltf, attr )
    offs          = view.byteOffset + accessor.byteOffset
    itemSize      = getAttrTypeSize ( gltf, attr ) * numComponents

    return view.buffer, accessor.componentType, numComponents, offs, accessor.count		# buffer, type, numComponents, offset in buffer, # of elements

def registerMeshAttrs ( gltf, primitive, mesh ):
    attrs = {   
        "pos":      ( 0, primitive.attributes.POSITION   ),
        "texCoord": ( 1, primitive.attributes.TEXCOORD_0 ),
        "normal":   ( 2, primitive.attributes.NORMAL     ),
        "tangent":  ( 3, primitive.attributes.TANGENT    ),
        "joints":   ( 5, primitive.attributes.JOINTS_0   ),
        "weights":  ( 6, primitive.attributes.WEIGHTS_0  )
    }
	
    loadBuffers ( gltf, primitive.indices )

    glBindVertexArray ( mesh.vao )

    for name in attrs:
        index, attr  = [name]

        if attr is None:
            continue

        buffer, dtype, numComponents, offs, count = attrProps ( gltf, attr )
        normalized = (name == "normal" or name == "tangent")
        stride     = getAttrTypeSize ( gltf, attr ) * numComponents
        buffer.buffer.bind ( target = GL_ARRAY_BUFFER )
        mesh.addAttribute ( name, index = index, dtype = dtype, numComponents = numComponents, stride = stride, offs = offs, normalized = normalized )

    mesh.ibo = attrToBuffer ( gltf, primitive.indices )
        # need to register all used buffer in mesh object
    glBindVertexArray ( 0 )


def attrToArray ( gltf, attr ):
    if attr is None:
        return None

    accessor = gltf.accessors[attr]                          	# acessor.componentType (5123 - GL_FLOAT, ...), count, type ("SCALAR", "VEC3"), min, max
    view     = gltf.bufferViews[accessor.bufferView]        	# view.target (34962 - ARRAY_BUFFER, 34963 - ELEMENT_ARRAY_BUFFER), byteStride for interleaving data
    
    if not "blob" in gltf.buffers [view.buffer].__dict__:
        loadBufferBlob ( gltf, gltf.buffers [view.buffer] )
        
    blob = gltf.buffers [view.buffer].blob

         # get compontent type as GL_* type constant
    if accessor.componentType == GL_FLOAT:
        dtype = "float32"
    elif accessor.componentType == GL_UNSIGNED_INT:
        dtype = "uint32"
    elif accessor.componentType == GL_INT:
        dtype = "int32"
    elif accessor.componentType == GL_UNSIGNED_SHORT:
        dtype = "uint16"
    elif accessor.componentType == GL_SHORT:
        dtype = "int16"
    elif accessor.componentType == GL_UNSIGNED_BYTE:
        dtype = "uint8"
    elif accessor.componentType == GL_BYTE:
        dtype = "int8"
    else:
        print ( f"Unknow accessor.componentType = {accessor.componentType}" )

    if accessor.type == "SCALAR":
        n = 1
    elif accessor.type == "VEC2":
        n = 2
    elif accessor.type == "VEC3":
        n = 3
    elif accessor.type == "VEC4":
        n = 4
    else:
        print ( f"Unknow accessor.type = {accessor.type}" )

    arr = numpy.frombuffer ( blob [view.byteOffset + accessor.byteOffset : view.byteOffset + view.byteLength], dtype = dtype, count = accessor.count * n )
    #print ( 'attr2Array', attr, accessor.type, n, dtype, arr )
	
    return arr.reshape ((-1, n))

# convertion table for type defs - "float32"-numpy.float32-GL_FLOAT-4 bytes size
typeTable = { 
    GL_FLOAT          : ( 4, "float32", numpy.float32 ),
    GL_UNSIGNED_INT   : ( 4, "uint32", numpy.uint32   ),
    GL_INT            : ( 4, "int32", numpy.int32     ),
    GL_UNSIGNED_SHORT : ( 2, "uint16", numpy.uint16   ),
    GL_SHORT          : ( 2, "int16", numpy.int16     ),
    GL_UNSIGNED_BYTE  : ( 1, "uint8", numpy.uint8     ),
    GL_BYTE           : ( 1, "int8", numpy.int8       ),
}

def getAttrNumComponents ( gltf, attr ):
    accessor = gltf.accessors [attr]

    if accessor.type == "SCALAR":
        return 1
    elif accessor.type == "VEC2":
        return 2
    elif accessor.type == "VEC3":
        return 3
    elif accessor.type == "VEC4":
        return 4
    else:
        print ( f"Unknow accessor.type = {accessor.type}" )

         # get compontent type as GL_* type constant
def getAttrType ( gltf, attr ):
    accessor = gltf.accessors [attr]
    return accessor.componentType

def getAttrTypeSize ( gltf, attr ):
    accessor = gltf.accessors [attr]
    
    if accessor.componentType == GL_FLOAT:
        return 4
    elif accessor.componentType == GL_UNSIGNED_INT:
        return 4
    elif accessor.componentType == GL_INT:
        return 4
    elif accessor.componentType == GL_UNSIGNED_SHORT:
        return 2
    elif accessor.componentType == GL_SHORT:
        return 2
    elif accessor.componentType == GL_UNSIGNED_BYTE:
        return 1
    elif accessor.componentType == GL_BYTE:
        return 1
    else:
        print ( f"Unknow accessor.componentType = {accessor.componentType}" )

def registerAttributes ( mesh, gltf, primitive ):
    stride = 0
    offs   = 0
    attrs = {   
        "pos":      ( 0, primitive.attributes.POSITION   ),
        "texCoord": ( 1, primitive.attributes.TEXCOORD_0 ),
        "normal":   ( 2, primitive.attributes.NORMAL     ),
        "tangent":  ( 3, primitive.attributes.TANGENT    ),
        "joints":   ( 5, primitive.attributes.JOINTS_0   ),
        "weights":  ( 6, primitive.attributes.WEIGHTS_0  )
    }
    
    for name in attrs:
        attr = attrs [name][1]
        if attr:
            stride += getAttrTypeSize ( gltf, attr ) * getAttrNumComponents ( gltf, attr )
           
    for name in attrs:
        index = attrs [name][0]
        attr  = attrs [name][1]
        #print ( ">", name, attr, index )
        if attr:
            mesh.addAttribute ( name, index = index, dtype = getAttrType ( gltf, attr ), numComponents = getAttrNumComponents ( gltf, attr ), stride = stride, offs = offs )
            offs  += getAttrTypeSize ( gltf, attr ) * getAttrNumComponents ( gltf, attr )

def _loadFrom ( gltf, primitive, meshNo, primitiveNo, material ):
    """
       Load a single mesh from a GLTF primitive
    """

    #vertexAttribs = tuple ( v for v in ( points, normals, texCoords, tangents, joints, weights) if v is not None )

    print ( 'Attributes: ', primitive.attributes.__dict__ )
    points    = attrToArray ( gltf, primitive.attributes.POSITION   )    # for simple skin we have only POSITION, JOINTS_0 and WEIGHTS_0
    normals   = attrToArray ( gltf, primitive.attributes.NORMAL     )
    texCoords = attrToArray ( gltf, primitive.attributes.TEXCOORD_0 )
    tangents  = attrToArray ( gltf, primitive.attributes.TANGENT    )
    joints    = attrToArray ( gltf, primitive.attributes.JOINTS_0   )
    weights   = attrToArray ( gltf, primitive.attributes.WEIGHTS_0  )
    indices   = attrToArray ( gltf, primitive.indices               )
    
    #print ( 'points', points )
    #print ( 'vertics', points )
    #print ( 'indices', primitive.indices )

    if indices is not None:
        indices = indices.flatten ()    # to turn from array of vecs into flat uint array
        #print ( indices )
    #return numpy.hstack ((points, texCoords, normals ))        # concat to array of (pos, tex, n)
    
        # combine all present attributs together
    vertices = numpy.hstack ( tuple ( v for v in ( points, normals, texCoords, tangents, joints, weights) if v is not None ) )
    #print ( 'points:', points [0], points [1] )
    #print ( 'normals:', normals [0], normals [1] )
    #print ( '---', vertices [0], vertices [1] )

        # create mesh and set additional attributes
    m           = Mesh.NewMesh ( primitive.mode )
    m.mesh      = meshNo
    m.primitive = primitiveNo
    m.material  = material

    registerAttributes ( m, gltf, primitive )

    m.setVertexBuffer ( vertices, len ( points ) )
    
    if indices is not None:
        m.setIndexBuffer ( indices, numIndices = len ( indices ), dtype = getAttrType ( gltf, primitive.indices ) )
        
    m.create ()
    return m

# node (name, index, matrix/rotation/translation/scale
def loadGltf ( filename ):
    gltf  = GLTF2().load ( filename )
    scene = gltf.scenes[gltf.scene]

    #print ( dir ( gltf.buffers[0] ) )
    print ( 'ASSET',      gltf.asset   )
    print ( 'BUFFERS',    gltf.buffers )
    print ( 'SCENES',     gltf.scenes  )
    print ( 'MESHES',     gltf.meshes     )
    print ( 'NODES',      gltf.nodes      )
    print ( 'MATERIALS',  gltf.materials  )
    print ( 'IMAGES',     gltf.images     )
    print ( 'SAMPLERS',   gltf.samplers   )
    print ( 'SKINS',      gltf.skins      )
    print ( 'ANIMATIONS', gltf.animations )
    print ( 'TEXTURES',   gltf.textures   )
    print ( 'IMAGES',     gltf.images     )
    print ( 'SCENES',     gltf.scenes     )
    
    materials = []
    for m in gltf.materials:
        d = { 'name': m.name }
        if m.pbrMetallicRoughness:
            d ['color']             = m.pbrMetallicRoughness.baseColorTexture.index
            d ['metallicRoughness'] = m.pbrMetallicRoughness.metallicRoughnessTexture.index
        d ['normal'] = m.normalTexture.index
        #print ( d )
        materials.append ( d )

    for primitive in gltf.meshes [0].primitives:
        print ( 'PRIM', primitive )

    m = _loadFrom ( gltf, gltf.meshes[0].primitives[0], 0, 0, gltf.meshes [0].primitives [0].material )

    m.textures  = []
    m.materials = materials

    print ( gltf.nodes [0].__dict__ )
    print ( 'NODE', Node ( gltf.nodes [0] ) )
    

    #for i in range(len(gltf.accessors)):
    #    acc = gltf.accessors [i]
    #    if acc.min:
    #        print ( 'min/max', acc.min, acc.max )
    #        m.min = glm.vec3 ( acc.min )
    #        m.max = glm.vec3 ( acc.max )

    for t in gltf.images:
        #print ( type(t), t )
        uri = t.uri
        m.textures.append ( Texture.Texture ( os.path.dirname(filename)+'/'+uri ) )

    if len(m.materials) > 0:
        m.colorTex  = m.textures [m.materials[0]['color']]
        m.mrTex     = m.textures [m.materials[0]['metallicRoughness']]
        m.normalTex = m.textures [m.materials[0]['normal']]

    #sys.exit ( 0 )
    
    print ( m.textures )
    return m

#filename = 'glTF-Sample-Models/2.0/BoxTextured/glTF-Binary/BoxTextured.glb'
#filename = 'beretta-m9/scene.gltf'
filename = 'glTF-Sample-Models/2.0/Avocado/glTF/Avocado.gltf'
#filename = 'glTF-Sample-Models/2.0/SimpleSkin/glTF/SimpleSkin.gltf'

class   MyWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.eye      = glm.vec3 ( -7, 0, 0 )
        self.lightDir = glm.vec3 ( -7, 0, 0 )
        self.mesh     = loadGltf ( filename )
        self.shader   = Program.Program ( glsl = "pbr-2.glsl" )
        self.shader.use ()
		
        if hasattr ( self.mesh, 'colorTex' ):
            self.mesh.colorTex.bind  ( 0 )
        if hasattr ( self.mesh, 'mrTex' ):
            self.mesh.mrTex.bind     ( 1 )
        if hasattr ( self.mesh, 'normalTex' ):
            self.mesh.normalTex.bind ( 2 )
			
        self.shader.setTexture ( 'albedoMap', 0 )
        self.shader.setTexture ( 'mrMap',     1 )
        self.shader.setTexture ( 'normalMap', 2 )

    def redisplay ( self ):
        glClearColor ( 0.2, 0.3, 0.2, 1.0 )
        glClear      ( GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT )
        glEnable     ( GL_DEPTH_TEST )

        self.shader.setUniformMat ( "mv",       self.getRotation () )
        self.shader.setUniformMat ( "nm",       self.normalMatrix ( self.getRotation () ) )
        self.mesh.render()

    def reshape ( self, width, height ):
        super().reshape ( width, height )
        self.shader.setUniformMat ( "proj",  self.getProjection () )
        self.shader.setUniformVec ( "eye",   self.eye )
        self.shader.setUniformVec ( "lightDir", self.lightDir )

    def mouseScroll ( self, dx, dy ):
        self.eye += glm.vec3 ( 0.1 * ( 1 if dy >= 0 else -1 ) )
        self.reshape ( self.width, self.height )

    def idle ( self ):
        angle = 4 * self.time ()
        #self.light = glm.vec3 ( 8*math.cos(angle), 8*math.sin(1.4*angle), 8+0.5*math.sin (angle/3) )
        self.shader.setUniformVec ( "eye",      self.eye   )
        self.shader.setUniformVec ( "lightDir", self.lightDir )

def main():
    win = MyWindow ( 800, 600, "Loading GLTF model" )
    win.run ()

if __name__ == "__main__":
    main()
