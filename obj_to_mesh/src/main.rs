extern crate wavefront_obj;
extern crate byteorder;

use byteorder::{LittleEndian, WriteBytesExt};
use std::io::prelude::*;
use std::fs::File;
use wavefront_obj::obj::{ObjSet, Object, Shape, VTNIndex, Vertex, TVertex};
use std::mem::size_of;
use std::collections::HashMap;
use std::f64;

#[derive(Clone, Copy)]
enum Attribute {
	Position,
	Normal,
	Tex0,
}

fn size_of_attribute(attr: Attribute) -> usize {
	match attr {
		Attribute::Position => size_of::<f32>() * 3,
		Attribute::Normal => size_of::<f32>() * 3,
		Attribute::Tex0 => size_of::<f32>() * 2,
	}
}

#[derive(Debug, Clone, Copy)]
struct VertexFieldOffsets {
	normal: Option<usize>,
	tex0: Option<usize>,
}

fn has_attribute(vtni: VTNIndex, attr: Attribute) -> bool {
	let (_, tex, normal) = vtni;
	match attr {
		Attribute::Position => true,
		Attribute::Normal => normal.is_some(),
		Attribute::Tex0 => tex.is_some()
	}
}

fn has_all(obj: &Object, attr: Attribute) -> bool {

	for geo in &obj.geometry {
		for shape in &geo.shapes {
			match *shape {
				Shape::Triangle(v1, v2, v3) => {
					if !has_attribute(v1, attr) || !has_attribute(v2, attr) || !has_attribute(v3, attr) {
						return false;
					}
				},
				_=> panic!("Unsupported primitive mode")
			}
		}
	}
	true
}

fn get_offset(obj: &Object, attr: Attribute, offset: &mut usize) -> Option<usize> {
	let orig_offs = *offset;
	if has_all(obj, attr) {
		*offset += size_of_attribute(attr);
		return Some(orig_offs);
	}
	None
}

impl VertexFieldOffsets {
	fn from_object(obj: &Object) -> Self {
		let mut offset = size_of_attribute(Attribute::Position);

		VertexFieldOffsets {
			normal: get_offset(obj, Attribute::Normal, &mut offset),
			tex0: get_offset(obj, Attribute::Tex0, &mut offset),
		}
	}
}

#[derive(Clone, Copy, Debug)]
struct GPUVertex {
	pos: Vertex,
	normal: Option<Vertex>,
	tex: Option<TVertex>,
}

impl GPUVertex {
	fn from_vtni_and_obj(vtni: VTNIndex, obj: &Object, format: &VertexFieldOffsets) -> Self {
		let (pos_idx, tex_opt_idx, norm_opt_idx) = vtni;
		GPUVertex {
			pos: obj.vertices[pos_idx],
			normal: match norm_opt_idx {
			    Some(idx) if format.normal.is_some() => Some(obj.normals[idx]),
			    _ => None,
			},
			tex: match tex_opt_idx {
			    Some(idx) if format.tex0.is_some() => Some(obj.tex_vertices[idx]),
			    _ => None,
			},
		}
	}

	fn writeTo(&self, data: &mut Vec<u8>) {
		data.write_f32::<LittleEndian>(self.pos.x as f32);
		data.write_f32::<LittleEndian>(self.pos.y as f32);
		data.write_f32::<LittleEndian>(self.pos.z as f32);

		if let Some(normal) = self.normal {
			data.write_f32::<LittleEndian>(normal.x as f32);
			data.write_f32::<LittleEndian>(normal.y as f32);
			data.write_f32::<LittleEndian>(normal.z as f32);
		}

		if let Some(tex) = self.tex {
			data.write_f32::<LittleEndian>(tex.x as f32);
			data.write_f32::<LittleEndian>(tex.y as f32);
		}
	}
}

#[derive(Debug)]
struct Mesh {
	vertices: Vec<GPUVertex>,
	indices: Vec<usize>,
	map: HashMap<VTNIndex, usize>,
	format: VertexFieldOffsets,

	min: Vertex,
	max: Vertex,
}

fn flt_min(a: f64, b: f64) -> f64 {
	if a < b { a } else { b }
}

fn flt_max(a: f64, b: f64) -> f64 {
	if a > b { a } else { b }
}

fn vert_min(a: Vertex, b: Vertex) -> Vertex {
	Vertex {
		x: flt_min(a.x, b.x),
		y: flt_min(a.y, b.y),
		z: flt_min(a.z, b.z),
	}
} 

fn vert_max(a: Vertex, b: Vertex) -> Vertex {
	Vertex {
		x: flt_max(a.x, b.x),
		y: flt_max(a.y, b.y),
		z: flt_max(a.z, b.z),
	}
} 

impl Mesh {
	fn from_object(obj: &Object) -> Self {
		let format = VertexFieldOffsets::from_object(&obj);
		let mut vertex_map = Mesh {
			vertices: Vec::new(),
			indices: Vec::new(),
			map:HashMap::new(),
			min: Vertex{x: f64::MAX, y: f64::MAX, z: f64::MAX },
			max: Vertex{x: f64::MIN, y: f64::MIN, z: f64::MIN },
			format: format,
		};

		for geo in &obj.geometry {
			for shape in &geo.shapes {
				match *shape {
					Shape::Triangle(v1, v2, v3) => {
						vertex_map.add(v1, &obj, &format);
						vertex_map.add(v2, &obj, &format);
						vertex_map.add(v3, &obj, &format);
					},
					_=> panic!("Unsupported primitive mode")
				}
			}
		}

		vertex_map
	}	

	fn create_vertex(&mut self, vtni: VTNIndex, obj: &Object, format: &VertexFieldOffsets) -> usize {
		let idx = self.vertices.len();

		let v = GPUVertex::from_vtni_and_obj(vtni, obj, format);

		self.vertices.push( v );

		self.min = vert_min(self.min, v.pos);
		self.max = vert_max(self.max, v.pos);

		idx
	}

	fn add(&mut self, vtni: VTNIndex, obj: &Object, format: &VertexFieldOffsets) {
		if let Some(idx) = self.map.get(&vtni) {
			self.indices.push(*idx);
			return;
		}

		let idx = self.create_vertex(vtni, obj, format);
		self.map.insert(vtni, idx);
		self.indices.push(idx);
	}

	fn get_index_size(&self) -> usize {
		match self.vertices.len() {
		    n if n <= 0xff => 1,
		    n if n <= 0xffff => 2,
		    _ => 4
		}
	}
}

fn convert_obj(obj: Object) -> Vec<u8> {

	//build a VTNIndex => Vertex map and build actual vertices
	let vertex_map = Mesh::from_object(&obj);

	let mut data = vec![];

	//write the index size in bytes
	let index_size = vertex_map.get_index_size() as u8;
	data.write_u8(index_size).unwrap();

	data.write_u8(1); //always a triangle list

	//write the vertex fields
	data.write_u8(0);   	//Position2D
	data.write_u8(1);	//Position3D
	data.write_u8(0);	//Color
	data.write_u8( if vertex_map.format.normal.is_some() { 1 } else { 0 } );
	data.write_u8( if vertex_map.format.tex0.is_some() { 1 } else { 0 } );
	data.write_u8(0);	//Tex1

	data.write_f32::<LittleEndian>(vertex_map.max.x as f32);
	data.write_f32::<LittleEndian>(vertex_map.max.y as f32);
	data.write_f32::<LittleEndian>(vertex_map.max.z as f32);

	data.write_f32::<LittleEndian>(vertex_map.min.x as f32);
	data.write_f32::<LittleEndian>(vertex_map.min.y as f32);
	data.write_f32::<LittleEndian>(vertex_map.min.z as f32);

	data.write_u32::<LittleEndian>(vertex_map.vertices.len() as u32);
	data.write_u32::<LittleEndian>(vertex_map.indices.len() as u32);

	for v in vertex_map.vertices {
		v.writeTo(&mut data);
	}

	for idx in vertex_map.indices {
		match index_size {
			1 => data.write_u8(idx as u8).unwrap(),
			2 => data.write_u16::<LittleEndian>(idx as u16).unwrap(),
			4 => data.write_u32::<LittleEndian>(idx as u32).unwrap(),
			_ => panic!("Invalid index size"),
		}
	}

	data
}

fn convert_obj_set(set: ObjSet) -> Vec<Vec<u8>> {
	let mut data: Vec<Vec<u8>> = vec![];

	for obj in set.objects {
		data.push(convert_obj(obj));
	}

	data
}

fn main() {

	let mut file = File::open("C:/Users/tommaso/DEV/vr150/VisualStudio/bin/data/tire.obj").unwrap();

	let mut content = String::new();
	file.read_to_string(&mut content).unwrap();

	let data = match wavefront_obj::obj::parse(content) {
	    Ok(obj) => convert_obj_set(obj),
	    Err(err) => panic!("{:?}", err),
	};

	let mut file = File::create("C:/Users/tommaso/DEV/vr150/VisualStudio/bin/data/tire.mesh").unwrap();

	file.write_all(&data[0]);
}