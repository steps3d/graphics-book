"""
    Module to load GLTF models
    Returns renderable hierachical model
"""

import os
import os.path
import glm
import numpy
from OpenGL.GL import *				# glBindVertexArray
from pygltflib import GLTF2
import Texture
import Buffer

class Model:
    def __init__(self, meshes, nodes, materials, textures):
        self.meshes    = meshes
        self.nodes     = nodes
        self.materials = materials
        self.textures  = textures
        self.root      = nodes [0]

class Node:
    def __init__ ( self, node, meshes, materials ):
        self.name        = node.name
        self.mesh        = meshes.get ( self.name, None )
        self.children    = node.children
        self.parent      = None
        self.skin        = None
        self.matrix      = glm.mat4 ( 1.0 )
        self.rotation    = glm.quat ( 0, 0, 0, 1 )
        self.translation = glm.vec3 ( 0, 0, 0 )
        self.scale       = glm.vec3 ( 1, 1, 1 )
        self.materials   = materials
        self.meshes      = meshes

        if "matrix" in node.__dict__ and node.matrix:
            self.matrix = glm.mat4 ( *node.matrix )

        if "rotation" in node.__dict__ and node.rotation:
            self.rotation = glm.quat ( node.rotation )

        if "translation" in node.__dict__ and node.translation:
            self.translation = glm.vec3 ( node.translation )

        if "scale" in node.__dict__ and node.scale:
            self.scale = glm.vec3 ( node.scale )

    def addChild ( self, node ):
        self.children.append ( node )
        
    def localTransform ( self ):
        return glm.translate ( glm.mat4(1), self.translation ) * glm.mat4_cast ( self.rotation ) * glm.scale ( glm.mat4 (1), self.scale ) * self.matrix

    def globalTransform ( self ):
        m = self.localTransform ()
        p = self.parent
        while p is not None:
            m = p.localTransform() * m
            p = p.parent

        return m

    def render ( self, modelView, shader ):
        mv = modelView
        if self.parent:
            mv = modelView * self.parent.globalTransform()

        shader.setUniformMat ( "mv", mv )
        shader.setUniformMat ( "nm", glm.inverseTranspose ( glm.mat3 ( mv ) ))
        if self.mesh:
            for mesh in self.mesh:
                if mesh.material is not None:
                    self.materials[mesh.material]['colorTex'].bind ( 0 )
                    self.materials[mesh.material]['normalTex'].bind ( 1 )
                    self.materials[mesh.material]['mrTex'].bind ( 2 )

                mesh.render ()
        
        for child in self.children:
            child.render ( modelView, shader )

class NewMesh:
    def __init__(self, mode=GL_TRIANGLES):
        self.vbo         = 0
        self.ibo         = 0
        self.vao         = 0
        self.numVertices = 0
        self.numIndices  = 0
        self.stride      = 0
        self.indexType   = 0
        self.indexOffs   = 0
        self.vertices    = None  # vertex buffer as list/tuple/numpy.array
        self.indices     = None
        self.mode        = mode
        self.attributes  = {}
        self.material    = 0
        self.primitive   = 0

    def create(self):
        self.vao = glGenVertexArrays(1)
        glBindVertexArray(self.vao)

        if self.indices is not None:  # must be already be Buffer object
            self.indices.bind(GL_ELEMENT_ARRAY_BUFFER)  # glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, self.indices.id )

        for name in self.attributes:
            index, buffer, numComponents, dtype, stride, offs, normalized = self.attributes[name]

            buffer.bind(GL_ARRAY_BUFFER)
            if dtype == GL_FLOAT:
                glVertexAttribPointer(index, numComponents, dtype, normalized, stride, ctypes.c_void_p(offs))
                glEnableVertexAttribArray(index)
            else:  # integer data type
                glVertexAttribIPointer(index, numComponents, dtype, stride, ctypes.c_void_p(offs))
                glEnableVertexAttribArray(index)

        glBindVertexArray(0)

    def addAttribute(self, name, buffer, index, dtype, numComponents, stride, offs, normalized=False):
        self.attributes[name] = (index, buffer, numComponents, dtype, stride, offs, normalized)
        self.stride = stride

    def setIndexBuffer(self, buffer, offs, numIndices, dtype=GL_UNSIGNED_INT):
        self.indices    = buffer
        self.indexType  = dtype
        self.indexOffs  = offs
        self.numIndices = numIndices

    def render(self):
        glBindVertexArray(self.vao)
        if self.indices:
            glDrawElements(self.mode, self.numIndices, self.indexType, ctypes.c_void_p(self.indexOffs))
        else:
            glDrawArrays(self.mode, 0, self.numVertices)

    # element counts for GLTF complex datatypes
_dataSizes = { "SCALAR": 1, "VEC2": 2, "VEC3": 3, "VEC4": 4, "MAT2": 4, "MAT3": 9, "MAT4": 16 }

    # datatype names (GL-type, name, numpy-type, numBytes)
_dataNames = (
    ( GL_FLOAT,          "float32", numpy.float32, 4 ),
    ( GL_UNSIGNED_INT,   "uint32",  numpy.uint32,  4 ),
    ( GL_INT,            "int32",   numpy.int32,   4 ),
    ( GL_UNSIGNED_SHORT, "uint16",  numpy.uint16,  2 ),
    ( GL_SHORT,          "int16",   numpy.int16,   2 ),
    ( GL_UNSIGNED_BYTE,  "uint8",   numpy.uint8,   1 ),
    ( GL_BYTE,           "int8",    numpy.int8,    1 )
)

#
# Load all buffers into VBO (vertex and element)
#
def loadBuffers ( gltf, index ):
    indexBuffer = attrToBuffer ( gltf, index )

    for buffer in gltf.buffers:
        buffer.blob   = numpy.frombuffer ( gltf.load_file_uri ( buffer.uri ), dtype = numpy.uint8 )
        target        = GL_ARRAY_BUFFER if buffer != indexBuffer else GL_ELEMENT_ARRAY_BUFFER
        buffer.buffer = Buffer.Buffer ( target, buffer.blob, dtype = numpy.uint8 )

def attrToBuffer ( gltf, attr ):
    accessor = gltf.accessors[attr]                     # accessor.componentType, count, type ("SCALAR", "VEC3"), min, max
    view     = gltf.bufferViews[accessor.bufferView]    # view.target, byteStride for interleaving data

    return gltf.buffers [view.buffer]

def getAttrTypeSize ( gltf, attr ):
    accessor = gltf.accessors [attr]

    for type, _, _, size in _dataNames:
        if type == accessor.componentType:
            return size
    assert False, "Invalid attr type"

def attrProps ( gltf, attr ):
    if attr is None:
        return None

    accessor      = gltf.accessors[attr]                    # accessor.componentType, count, type ("SCALAR", "VEC3"), min, max
    view          = gltf.bufferViews[accessor.bufferView]	# view.target (ARRAY_BUFFER, ELEMENT_ARRAY_BUFFER)
    numComponents = _dataSizes [accessor.type]
    offs          = view.byteOffset + accessor.byteOffset
    min, max      = accessor.min, accessor.max

    return view.buffer, accessor.componentType, numComponents, offs, accessor.count, min, max

def registerMeshAttributes ( gltf, primitive, mesh ):
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
        index, attr  = attrs [name]

        if attr is None:
            continue

        bufIndex, dtype, numComponents, offs, count, min, max = attrProps ( gltf, attr )
        normalized = name in ("normal", "tangent")
        stride     = getAttrTypeSize ( gltf, attr ) * numComponents

            # buffer is an int id (view.buffer)
        buffer = gltf.buffers [bufIndex].buffer
        buffer.bind ( target = GL_ARRAY_BUFFER )
        mesh.addAttribute  ( name, buffer = buffer, index = index, dtype = dtype, numComponents = numComponents,
                             stride = stride, offs = offs, normalized = normalized )

        if name == "pos":
            mesh.min = min
            mesh.max = max

    if primitive.indices is not None:
        bufIndex, dtype, _, offs, count, _, _ = attrProps ( gltf, primitive.indices )
        buffer = gltf.buffers [bufIndex].buffer
        mesh.setIndexBuffer ( buffer, offs = offs, numIndices = count, dtype = dtype )

        # need to register all used buffers in mesh object
    mesh.create ()
    glBindVertexArray ( 0 )

#
# Load a single mesh from a GLTF primitive
#
def _loadFrom ( gltf, primitive, meshNo, primitiveNo ):

        # create mesh and set additional attributes
    m           = NewMesh ( primitive.mode )
    m.mesh      = meshNo
    m.primitive = primitiveNo
    m.material  = primitive.material
    m.vao       = glGenVertexArrays ( 1 )

    registerMeshAttributes ( gltf, primitive, m )

    return m

"""
    Actual loading function
"""
def loadGltf ( fileName ):
    gltf  = GLTF2().load ( fileName )

        # load all meshes into array
    meshes = {}
    for i, mesh in  enumerate ( gltf.meshes ):
        m = []      # just a list of primitives
        meshes [mesh.name] = m
        for j, primitive in enumerate ( mesh.primitives ):
            m.append ( _loadFrom ( gltf, primitive, i, j ) )

        # load all textures
    textures = []
    images   = []
    for tex in gltf.images:
        #uri   = tex.uri
        image = Texture.Texture ( os.path.dirname ( fileName ) + '/' +tex.uri )
        textures.append ( image )
        images.append   ( image )

        # load all materials
    materials = []
    for material in gltf.materials:
        d = { 'name': material.name }
        if material.pbrMetallicRoughness:
            d ['colorTex'] = textures [material.pbrMetallicRoughness.baseColorTexture.index]
            d ['mrTex']    = textures [material.pbrMetallicRoughness.metallicRoughnessTexture.index]
        d ['normalTex'] = textures [material.normalTexture.index]
        materials.append ( d )

        # load all nodes
    nodes = [ Node ( n, meshes, materials ) for n in gltf.nodes ]
    for node in nodes:
        node.children = [nodes [k] for k in node.children]

        # set parents for nodes
    for node in nodes:
        for child in node.children:
            child.parent = node

    return Model ( meshes, nodes, materials, textures )
