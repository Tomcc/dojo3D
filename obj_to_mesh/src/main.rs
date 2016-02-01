extern crate wavefront_obj;

use std::io::prelude::*;
use std::fs::File;

fn main() {

	let mut file = File::open("C:/Users/tommaso/DEV/vr150/VisualStudio/bin/data/tire.obj").unwrap();

	let mut content = String::new();
	file.read_to_string(&mut content).unwrap();

	match wavefront_obj::obj::parse(content) {
	    Ok(obj) => println!("{:?}", obj),
	    Err(err) => println!("{:?}", err),
	}
}
