use std::{io, net, sync};
use std::hash::{Hash, Hasher};
use std::io::Write;
use std::sync::{Arc, Mutex, RwLock};
use crate::server::ServerInternal;

pub struct Conn {
	server: Arc<RwLock<ServerInternal>>,
	stream: Mutex<net::TcpStream>,
}

impl Conn {
	pub fn new(server: Arc<RwLock<ServerInternal>>, stream: net::TcpStream) -> Self {
		return Self {
			server,
			stream: Mutex::new(stream),
		};
	}

	pub fn handle(&self) {
		let stream = &(self.stream.lock().unwrap());
		println!("{:?}", stream.peer_addr());

		self.close();
	}

	fn close(&self) {
		let mut server = self.server.write().unwrap();
		server.remove(self);
	}
}

fn conn_stream_ptr(c: &Conn) -> u64 {
	return &(c.stream) as (*const Mutex<net::TcpStream>) as u64;
}

impl PartialEq<Self> for Conn {
	fn eq(&self, other: &Self) -> bool {
		return conn_stream_ptr(self) == conn_stream_ptr(other);
	}
}

impl Eq for Conn {}

impl Hash for Conn {
	fn hash<H: Hasher>(&self, state: &mut H) {
		state.write_u64(conn_stream_ptr(self));
	}

	fn hash_slice<H: Hasher>(data: &[Self], state: &mut H) where Self: Sized {
		for datum in data {
			state.write_u64(conn_stream_ptr(datum));
		}
	}
}