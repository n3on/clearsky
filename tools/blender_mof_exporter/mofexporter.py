#!BPY

#Exporter for MOF File

"""
Name: 'MOF Exporter'
Blender: '249'
Group: 'Export'
Tooltip: 'Export as MOF File'
"""

import Blender
import bpy
import struct
import os

from Blender import *
from operator import attrgetter


MOF_MAGIC_NUMBER = 0x67
MOF_VERSION		 = 0x01

MOF_MESH_CHUNK_ID = 0x00

MOF_VERTEX_CHUNK_ID 	= 0x01
MOF_VERTEX_POSITION = 0x01
MOF_VERTEX_NORMALS  = 0x02
MOF_VERTEX_TEX		= 0x03
MOF_VERTEX_COLOR	= 0x04

MOF_INDEX_CHUNK_ID  	= 0x02

MOF_MATERIAL_CHUNK_ID	= 0x03
MOF_MATERIAL_TEXTURE = 0x01
MOF_MATERIAL_DIFFUSE = 0x02
MOF_MATERIAL_SPECULAR = 0x03
MOF_MATERIAL_AMBIENT  = 0x04
MOF_MATERIAL_END_ENUMERATION = 0x10

def saveMOFHeader(file):
	scene   = bpy.data.scenes.active
	
	numObjects = 0;
	for obj in scene.objects:
		if obj.getType()=='Mesh':
			numObjects = numObjects+1
	
	file.write(struct.pack("<BBi", MOF_MAGIC_NUMBER, MOF_VERSION, numObjects))
	
def save(filename):
	out = file(filename, "wb")
	saveMOFHeader(out)
	
	scene   = bpy.data.scenes.active
	objects  = scene.objects
	
	for obj in objects:
		if obj.getType()=='Mesh':
			objectMesh = obj.getData(mesh=1)
			saveMOFMesh(out, objectMesh)
			
	out.close()
	
def saveMOFMesh(file, mesh):
	scene   = bpy.data.scenes.active
	
	size = 0;
	
	file.write(struct.pack("<Bi",MOF_MESH_CHUNK_ID,size))
			
	saveMOFVertexChunk(file,mesh)
	saveMOFIndexChunk(file,mesh)
	saveMOFMaterialChunk(file,mesh)
	
def saveMOFVertexChunk(file,mesh):
	scene   = bpy.data.scenes.active
	
	if len(mesh.verts) > 0:
		size = len(mesh.verts)*3+4+1
		numVerts = len(mesh.verts)
		numElements = 2
		
		if(mesh.vertexUV):
			numElements = numElements+1;
	
		file.write(struct.pack("<BiiB",MOF_VERTEX_CHUNK_ID,size, numVerts,numElements))
		
		for vert in mesh.verts:
			file.write(struct.pack("<Bfff",MOF_VERTEX_POSITION,vert.co.x, vert.co.z, vert.co.y))
			file.write(struct.pack("<Bfff",MOF_VERTEX_NORMALS,vert.no.x, vert.no.z, vert.no.y))
			if(mesh.vertexUV):
				file.write(struct.pack("<Bff", MOF_VERTEX_TEX, vert.uvco.x, vert.uvco.y))
		
def saveMOFIndexChunk(file,mesh):
	scene   = bpy.data.scenes.active
	
	if len(mesh.faces) > 0:
		#triangulate mesh
		for face in mesh.faces:
			face.sel=1
			
		Blender.Mesh.Mode(3) #face selection mode
		mesh.quadToTriangle(0)	
		#end mesh triangulation
		
		numIndices = len(mesh.faces)*3
		#size is number of indices plus size of numIndices entry
		size = len(mesh.faces)*3+4
	
		#indexFormat = 0
		
		#if numIndices>=65536:
		#	indexFormat = 1
			
		file.write(struct.pack("<Bii",MOF_INDEX_CHUNK_ID, size, numIndices))
		
		faces = sorted(mesh.faces, key=attrgetter('mat'))
		
		for face in faces:
			for vert in face.v:
				#if indexFormat == 0:
				#	file.write(struct.pack("h",vert.index))
				#else:
					file.write(struct.pack("i",vert.index))
	
	
def saveMOFMaterialChunk(file,mesh):
	scene   = bpy.data.scenes.active
	
	materials = mesh.materials
	
	print "materials %i" % (len(materials))
	if(len(materials)>0):
		numMaterials = len(materials)
		size = numMaterials
		
		file.write(struct.pack("<Bii",MOF_MATERIAL_CHUNK_ID,size,numMaterials))
		
		faces = sorted(mesh.faces, key=attrgetter('mat'))
		faceLocationData = {}
		
		startFace=0
		numFaces=0
		lastMat = 0;
		for face in faces:
			if lastMat!=face.mat:
				lastMat = face.mat
				startFace=startFace+numFaces
				numFaces=0
			faceLocationData[face.mat] = [startFace, numFaces]
			numFaces = numFaces+1
				
		matIndex=0;
		for material in materials:
			file.write(struct.pack("<Bii",matIndex,faceLocationData[matIndex][0],faceLocationData[matIndex][1]))
			textures = material.getTextures()
			for texture in textures:
				if texture!=None:
					if texture.tex.type == Texture.Types.IMAGE:
						name = os.path.basename(texture.tex.image.filename)
						file.write(struct.pack("<BB", MOF_MATERIAL_TEXTURE,len(name)))
						file.write(name)
			
			diffuseColor =  material.getRGBCol()
			diffuseColor.append(0)
			file.write(struct.pack("<Bffff",MOF_MATERIAL_DIFFUSE,diffuseColor[0], diffuseColor[1],diffuseColor[2],diffuseColor[3]))
			
			file.write(struct.pack("<B",MOF_MATERIAL_END_ENUMERATION))
			
			matIndex = matIndex+1	
			

		
Blender.Window.FileSelector(save, "Export")
