extern crate wavefront_obj;

use std::io::prelude::*;
use std::fs::File;
use wavefront_obj::obj::{ObjSet, Object, Shape, VTNIndex};
use std::mem::size_of;

#[derive(Clone, Copy)]
enum Attribute {
	Position,
	Normal,
	Tex0,
}

#[derive(Debug)]
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
				Shape::Line(v1, v2) => {
					if !has_attribute(v1, attr) || !has_attribute(v2, attr) {
						return false;
					}
				},
				Shape::Point(v1) => {
					if !has_attribute(v1, attr) {
						return false;
					}
				},
			}
		}
	}
	true
}

fn get_offset(obj: &Object, attr: Attribute, offset: &mut usize) -> Option<usize> {
	let orig_offs = *offset;
	if has_all(obj, attr) {
		*offset += size_of::<f32>() * 3; //TODO tex0
		return Some(orig_offs);
	}
	None
}

impl VertexFieldOffsets {
	fn from_object(obj: &Object) -> Self {
		let mut offset = size_of::<f32>() * 3; //size of Position

		VertexFieldOffsets {
			normal: get_offset(obj, Attribute::Normal, &mut offset),
			tex0: get_offset(obj, Attribute::Tex0, &mut offset),
		}
	}
}

fn convert_obj(obj: Object) -> Vec<u8> {

	let offsets = VertexFieldOffsets::from_object(&obj);

	println!("{:?}", offsets);

	vec![]
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

	for mesh in data {
		//write out
	}
}
