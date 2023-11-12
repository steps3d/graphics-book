import struct
from OpenGL.GL import *
from OpenGL.GL.EXT import texture_compression_s3tc
import numpy

DDSD_CAPS    	= 0x00000001
DDSD_HEIGHT  	= 0x00000002
DDSD_WIDTH   	= 0x00000004
DDSD_PITCH   	= 0x00000008
DDSD_PIXELFORMAT = 0x00001000
DDSD_MIPMAPCOUNT = 0x00020000
DDSD_LINEARSIZE  = 0x00080000
DDSD_DEPTH   	= 0x00800000

	# DirectDraw Pixel Format
DDPF_ALPHAPIXELS = 0x00000001
DDPF_FOURCC  	= 0x00000004
DDPF_RGB     	= 0x00000040

	# The dwCaps1 member of the DDSCAPS2 structure can be
	# set to one or more of the following values.
DDSCAPS_COMPLEX  = 0x00000008
DDSCAPS_TEXTURE  = 0x00001000
DDSCAPS_MIPMAP   = 0x00400000

DDSCAPS2_CUBEMAP       	= 0x00000200
DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400
DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800
DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000
DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000
DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000
DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000
DDSCAPS2_VOLUME        	   = 0x00200000

D3D10_RESOURCE_DIMENSION_UNKNOWN   = 0
D3D10_RESOURCE_DIMENSION_BUFFER	   = 1
D3D10_RESOURCE_DIMENSION_TEXTURE1D = 2
D3D10_RESOURCE_DIMENSION_TEXTURE2D = 3
D3D10_RESOURCE_DIMENSION_TEXTURE3D = 4

DXGI_FORMAT_R8G8B8A8_UNORM = 28


BC4U = b'BC4U'
BC4S = b'BC4S'
#DXT1 = b'0x31545844'
DXT1 = b'DXT1'  	#0x33545844;
DXT3 = b'DXT3'  	#0x35545844;
DXT5 = b'DXT5'  	#0x35545844;
#DX10 = b'DXT5'  	# 0x30315844;


	# not found in OpenGL.GL
#GL_COMPRESSED_RGB_S3TC_DXT1_EXT  = 0x83F0
#GL_COMPRESSED_RGBA_S3TC_DXT1_EXT = 0x83F1
#GL_COMPRESSED_RGBA_S3TC_DXT3_EXT = 0x83F2
#GL_COMPRESSED_RGBA_S3TC_DXT5_EXT = 0x83F3

cubeFaces = (
    	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	)

	# returns hdr - dwMagic, dwSize, dwFlags, dwHeight, dwWidth, dwPitchOrLinearSize, dwDepth, dwMipMapCount
def readHeader ( b, offs ):
	return struct.unpack ( '<4sLLLLLLL', b [offs:offs + 8*4] )

	# returns dwSize, dwFLags, dwFourCC, dwRGBBitCount, dwR/G/B/AlphaBitMask
def readPixelFormat ( b, offs ):
	return struct.unpack ( '<LL4sLLLLL', b [offs:offs+8*4] )

	# returns dwCaps1, dwCaps2, dwDDSX
def readDdCaps2 ( b, offs ):
	return struct.unpack ( '<LLLL', b [offs:offs+4*4] )

	# returns dxgiFormat, resourceDimension, miscFlag, arraySize, miscFlags2
def readDx10Header ( b, offs ):
	return struct.unpack ( '<', b [offs:offs + 5*4] )

def readDds ( b ):
    offs = 0
    magic, size, flags, height, width, pitch, depth, mipMapCount = readHeader ( b, offs )
    if (magic != b'DDS ') or (size != 124):
        print ( 'Invalid DDS header' )
        return

    if (flags & DDSD_DEPTH) == 0:
        depth = 1

    flagsMask = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
    if (flags & flagsMask) != flagsMask:
        print ( 'Missoing required flags' )
        return

    offs += 76
    size, formatFlags, fourCC, rgbBitCount, rMask, gMask, bMask, aMask = readPixelFormat ( b, offs )

    offs += 8*4   	 # size = 32

    #print ( f'Size {size} {hex(formatFlags)} {fourCC}' )
    #print ( 'Bit count', rgbBitCount, hex(rMask), hex(gMask), hex(bMask), hex(aMask) )    

    if size != 32:
        print ( 'Invalid sPixelFormat.dwSize', size, formatFlags, fourCC, rgbBitCount, rMask, gMask, bMask, aMask )
        return

    flagsMask = DDPF_FOURCC | DDPF_RGB
    if (formatFlags & flagsMask) == 0:
        print ( 'Missing required flags in sPixelFormat.flags', hex(formatFlags), formatFlags )
        return

    caps1, caps2, _, _ = readDdCaps2 ( b, offs )
    offs += 4*4 	# 4 caps (dword)
    offs += 4   	# for dwReserved2 at the end of DDS_HEADER

    if (caps1 & DDSCAPS_TEXTURE) == 0:
        print ( 'Missing required flags in sCaps.dwCaps1', hex(caps1) )
        return

    #print ( 'After headers offs is', offs )
    
    hasFourCC	= (formatFlags & DDPF_FOURCC) != 0
    hasAlpha 	= (formatFlags & DDPF_ALPHAPIXELS) != 0
    hasMipmap	= ((caps1 & DDSCAPS_MIPMAP) != 0) and (mipMapCount > 1)
    isCubeMap	= (caps2 & DDSCAPS2_CUBEMAP) != 0
    isVolume 	= (caps2 & DDSCAPS2_VOLUME) != 0
    isCompressed = False
    texelSize	= 4
    blockSize	= 4	# bytes per block
    blockWidth   = 1
    blockHeight  = 1
    layerCount   = 6 if isCubeMap else 1
    format   	= GL_RGBA8 if hasAlpha else GL_RGB8
    target   	= GL_TEXTURE_2D

    if isCubeMap:
        target = GL_TEXTURE_CUBE_MAP

    if isVolume:
        target = GL_TEXTURE_3D

    if height == 1:
        target = GL_TEXTURE_1D

    if mipMapCount < 1:
        mipMapCount = 1

    if isCubeMap and (width != height):
        print ( 'Cubemap width != height' )

    #print ( '------DDS------\n', fourCC, 'alpha:', hasAlpha, 'mipmap:', hasMipmap, 'cubemap:', isCubeMap, 'volume:', isVolume )
    
    if hasFourCC:
        if fourCC == b'30315844':	# DX10 signature
            dxgiFormat, resourceDimension, miscFlag, arraySize, miscFlags2 = readDx10Header ( b, offs )
            offs += 5*4

            if dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM:
                format = GL_RGBA8_UNORM	#R8G8B8A8_UNORM;

            if resourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE1D:
                target = GL_TEXTURE_1D	#VK_IMAGE_TYPE_1D;
            elif resourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE2D:
                target = GL_TEXTURE_2D	#VK_IMAGE_TYPE_2D;
            elif resourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE3D:
                target = GL_TEXTURE_3D	#VK_IMAGE_TYPE_3D   	 
        elif fourCC == DXT1:
            isCompressed = True
            blockSize	= 8
            blockWidth   = 4
            blockHeight  = 4
        elif fourCC == DXT3:
            isCompressed = True
            blockSize	= 16
            blockWidth   = 4
            blockHeight  = 4
        elif fourCC == DXT5:
            isCompressed = True
            blockSize	= 16
            blockWidth   = 4
            blockHeight  = 4
        elif fourCC == BC4U:
            isCompressed = True
            blockSize	= 16
            blockWidth   = 4
            blockHeight  = 4
        elif fourCC == BC4S:
            isCompressed = True
            blockSize	= 16
            blockWidth   = 4
            blockHeight  = 4
   	 
    #print ( 'Compr:', fourCC, isCompressed, blockWidth, blockHeight, blockSize, pitch )   

    w = width
    h = height
    d = depth

    if isCompressed:
        blockPitch = (width + blockWidth - 1) // blockWidth
        #numBlocks  = blockPitch * ((height + blockHeight - 1) / blockHeight)
        dxtFamily  = fourCC [3] - ord ( '1' ) + 1

        if (dxtFamily < 1) or (dxtFamily > 5):
            print ( 'Illegal dxtFamily', dxtFamily )
            return
       	 
        if dxtFamily == 1:
            format = texture_compression_s3tc.GL_COMPRESSED_RGBA_S3TC_DXT1_EXT if hasAlpha else texture_compression_s3tc.GL_COMPRESSED_RGB_S3TC_DXT1_EXT
        elif dxtFamily == 3:
            format = texture_compression_s3tc.GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
        elif dxtFamily == 5:
            format = texture_compression_s3tc.GL_COMPRESSED_RGBA_S3TC_DXT5_EXT

        # create a texture
    id = glGenTextures ( 1 )
    glBindTexture ( GL_TEXTURE_CUBE_MAP if isCubeMap else target, id )
    #print ( 'BindTexture', GL_TEXTURE_CUBE_MAP if isCubeMap else target, id, target, isCompressed, layerCount, mipMapCount )

        # Set the texture wrapping parameters
    glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE if isCubeMap else GL_REPEAT )
    glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE if isCubeMap else GL_REPEAT )
    glTexParameteri ( target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE if isCubeMap else GL_REPEAT )

        # Set texture filtering parameters
    glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR )
    glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR )

    if not isCompressed:   	 # uncompressed data, simply load layers/levels
        for layer in range ( layerCount ):
            w   = width
            h   = height
            d   = depth
            fmt = GL_RGBA

            if format == GL_RGB8:
                fmt = GL_RGB

            for level in range ( mipMapCount ):
                    # upload level
                trg       	= cubeFaces [layer] if isCubeMap else target
                numComponents = (rgbBitCount + 7) // 8
                sz        	= w * h * d * numComponents
                bits      	= b[offs:offs+sz]
       	 
                if target == GL_TEXTURE_1D:
                    glTexImage1D ( trg, level, format, w, 0, fmt, GL_UNSIGNED_BYTE, bits )
                elif target == GL_TEXTURE_2D:
                    glTexImage2D ( trg, level, format, w, h, 0, fmt, GL_UNSIGNED_BYTE, bits )
                elif target == GL_TEXTURE_3D:
                    glTexImage3D ( trg, level, format, w, h, d, 0, fmt, GL_UNSIGNED_BYTE, bits )
                elif target == GL_TEXTURE_CUBE_MAP:
                    glTexImage2D ( cubeFaces[layer], level, format, w, h, 0, format, GL_UNSIGNED_BYTE, bits )
   	 
                    # prepare for next level
                offs += sz
                w 	= max ( 1, w // 2 )
                h 	= max ( 1, h // 2 )
                d 	= max ( 1, d // 2 )

        return id, target, width, height, depth

        # now upload compressed texture data
    for layer in range ( layerCount ):
        w = width
        h = height
        d = depth

        for level in range ( mipMapCount ):

                # upload level
            sz   = ((w + blockWidth - 1) // blockWidth) * ((h + blockHeight - 1) // blockHeight) * blockSize
            trg  = cubeFaces [layer] if isCubeMap else target         	 
            bits = b [offs:offs+sz]
       	 
            if target == GL_TEXTURE_1D:
                glCompressedTexImage1D ( trg, level, format, w, 0, bits )
            elif target == GL_TEXTURE_2D:
                glCompressedTexImage2D ( trg, level, format, w, h, 0, bits )
            elif target == GL_TEXTURE_3D:
                glCompressedTexImage3D ( trg, level, format, w, h, d, 0, bits )
            elif target == GL_TEXTURE_CUBE_MAP:
                glCompressedTexImage2D ( trg, level, format, w, h, 0, bits )
   	 
                # prepare for next level
            offs += sz
            w 	  = max ( 1, w // 2 )
            h 	  = max ( 1, h // 2 )
            d 	  = max ( 1, d // 2 )

    #print ( 'OUT', id, target, width, height, depth, layerCount, isCubeMap, format )
    #print ('Init size:', len(b), offs )
    glGenerateMipmap ( target )

    return id, target, width, height, depth

def readDdsFile ( fileName ):
    data = open ( fileName, 'rb').read ()

    return readDds ( data )

