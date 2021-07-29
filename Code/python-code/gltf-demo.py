import os
import os.path
import math
import glm
import numpy
from OpenGL.GL import *
from pygltflib import GLTF2, Scene
import Window
import Program
import Texture
import Mesh

	# map to numpy.array
def toArray ( blob, accessor, view, dtype, n ):
	arr=numpy.frombuffer ( blob [view.byteOffset + accessor.byteOffset : view.byteOffset + view.byteLength], dtype = dtype, count = accessor.count * n )
	return arr.reshape ((-1, n))

def attrToArray ( gltf, blob, attr, n, dtype ):
	accessor     = gltf.accessors[attr]
	buffer_view  = gltf.bufferViews[accessor.bufferView]
	return toArray ( blob, accessor, buffer_view, dtype = dtype, n = n )
	
def _toVec2 ( v ):
	return glm.vec2 ( v[0], v[1] )
	
def _toVec3 ( v ):
	return glm.vec3 ( v[0], v[1], v[2] )
	
def _loadFrom ( gltf, primitive ):
	print ( 'Attributes: ', primitive.attributes )
	blob      = gltf.binary_blob()
	points    = attrToArray ( gltf, blob, primitive.attributes.POSITION,   3, dtype = "float32" )
	normals   = attrToArray ( gltf, blob, primitive.attributes.NORMAL,     3, dtype = "float32" )
	texCoords = attrToArray ( gltf, blob, primitive.attributes.TEXCOORD_0, 2, dtype = "float32" )
	tangents  = attrToArray ( gltf, blob, primitive.attributes.TANGENT,    3, dtype = "float32" )
	indices   = attrToArray ( gltf, blob, primitive.indices,               1, dtype = "uint16" ).flatten ()

	#return numpy.hstack ((points, texCoords, normals ))		# concat to array of (pos, tex, n)

	# or this way
	m = Mesh.Mesh ()
	for i in range ( len ( points ) ):
		m.addVertex ( _toVec3(points [i])*100, _toVec2(texCoords [i]), _toVec3(normals [i]) )
		
	for i in range ( len ( indices ) // 3 ):
		m.addFace ( indices [3*i], indices [3*i+1], indices [3*i+2] )
		
	m.create ()
	return m
	
def loadGltf ( filename ):
	gltf = GLTF2().load ( filename )
	current_scene = gltf.scenes[gltf.scene]
		# we may have to load binary blob manually
	if gltf.binary_blob () is None:
		if filename.endswith ( '.gltf'  ):
			binFilename = filename [0:-5] + '.bin'
			gltf.set_binary_blob ( open(binFilename, 'rb' ).read () )
	

	#print ( gltf.meshes [0] )
	#print ( gltf.meshes [0].primitives [0] )
	#print ( 'BBox', gltf.accessors )
	for i in range(len(gltf.accessors)):
		acc = gltf.accessors [i]
		if acc.min:
			print ( 'min/max', acc.min, acc.max )

	print ( '-------------------------' )
	print ( gltf.asset )
	print ( '--- Materials' )
	materials = []
	for m in gltf.materials:
		d = { 'name': m.name }
		if m.pbrMetallicRoughness:
			d ['color']             = m.pbrMetallicRoughness.baseColorTexture.index
			d ['metallicRoughness'] = m.pbrMetallicRoughness.metallicRoughnessTexture.index
		d ['normal'] = m.normalTexture.index
		print ( d )
		materials.append ( d )
				
	m = _loadFrom ( gltf, gltf.meshes[0].primitives[0] )
	
	m.textures  = []
	m.materials = materials

	for i in range(len(gltf.accessors)):
		acc = gltf.accessors [i]
		if acc.min:
			print ( 'min/max', acc.min, acc.max )
			m.min = glm.vec3 ( acc.min )
			m.max = glm.vec3 ( acc.max )

	for t in gltf.images:
		#print ( type(t), t )
		uri = t.uri
		m.textures.append ( Texture.Texture ( os.path.dirname(filename)+'/'+uri ) ) 

	if len(m.materials) > 0:
		m.colorTex = m.textures [m.materials[0]['color']]
		m.mrTex    = m.textures [m.materials[0]['metallicRoughness']]
		m.normal   = m.textures [m.materials[0]['normal']]

	print ( m.textures )
	return m

#filename = 'glTF-Sample-Models/2.0/BoxTextured/glTF-Binary/BoxTextured.glb'
#filename = 'beretta-m9/scene.gltf'
filename = 'glTF-Sample-Models/2.0/Avocado/glTF/Avocado.gltf'

#mesh = loadGltf ( filename )
#print ( mesh )

class   MyWindow ( Window.RotationWindow ):
    def __init__ ( self, w, h, t ):
        super().__init__ ( w, h, t )
        self.eye      = glm.vec3 ( -7, 0, 0 )
        self.lightDir = glm.vec3 ( -1, 1, 1 )
        self.shader   = Program.Program ( vertex = "cook-torrance-tex.vsh", fragment = "cook-torrance-tex.fsh" )
        #self.mesh     = Mesh.Mesh.createKnot ( 1, 1, 120, 30 )
        #self.mesh   = mesh.Mesh.createBox ( glm.vec3 ( -0.5 ), glm.vec3(1) )
        self.mesh   = loadGltf ( filename )
        self.tex    = Texture.Texture ( os.path.dirname(filename)+'/Avocado_baseColor.png' ) 
        self.shader.use ()
        self.tex.bind ()
        self.shader.setTexture('baseColor', 0)
        self.idle ()

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
        self.shader.setUniformVec ( "light", self.light )

    def mouseScroll ( self, dx, dy ):
        #print ( 'wheel', dx, dy )
        self.eye += glm.vec3 ( 0.1 * ( 1 if dy >= 0 else -1 ) )
        self.reshape ( self.width, self.height )

    def idle ( self ):
        angle = 4 * self.time ()
        self.light = glm.vec3 ( 8*math.cos(angle), 8*math.sin(1.4*angle), 8+0.5*math.sin (angle/3) )
        self.shader.setUniformVec ( "eye",   self.eye   )
        self.shader.setUniformVec ( "light", self.light )


def main():
    win = MyWindow ( 800, 600, "Loading GLTF model" )

    if not win:
        glfw.terminate()
        return



    win.run ()

if __name__ == "__main__":
    main()

'''
Material(
	extensions={}, 
	extras={}, 
	pbrMetallicRoughness=PbrMetallicRoughness
	(
		extensions={}, 
		extras={}, 
		baseColorFactor          = [1.0, 1.0, 1.0, 1.0], 
		metallicFactor           = 1.0, 
		roughnessFactor          = 1.0, 
		baseColorTexture         = TextureInfo(extensions={}, extras={}, index=0, texCoord=0), 
		metallicRoughnessTexture = TextureInfo(extensions={}, extras={}, index=1, texCoord=0)
	), 
	normalTexture=NormalMaterialTexture(
		extensions={}, 
		extras={}, 
		index=2, 
		texCoord=None, 
		scale=1.0), 
	occlusionTexture=None, 
	emissiveFactor=[0.0, 0.0, 0.0], 
	emissiveTexture=None, 
	alphaMode='OPAQUE', 
	alphaCutoff=0.5, 
	doubleSided=False, 
	name='2256_Avocado_d'
)
'''