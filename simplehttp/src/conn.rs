use std::{net};
use std::hash::{Hash, Hasher};
use std::sync::{Arc, Mutex, RwLock};
use std::sync::atomic::{AtomicBool, Ordering};
use crate::server::ServerInternal;

pub struct Conn {
	server: Arc<RwLock<ServerInternal>>,
	stream: Mutex<net::TcpStream>,
	closed: AtomicBool,
}

impl Conn {
	pub fn new(server: Arc<RwLock<ServerInternal>>, stream: net::TcpStream) -> Self {
		return Self {
			server,
			stream: Mutex::new(stream),
			closed: AtomicBool::new(false),
		};
	}

	pub fn handle(&self) {
		let stream = self.stream.lock().unwrap();
		println!("{:?}", stream.peer_addr());
	}

	pub fn close(&self) {
		if self.closed.load(Ordering::SeqCst) {
			return;
		}
		self.closed.store(true, Ordering::SeqCst);

		let _ = &(self.stream.lock().unwrap());
		let mut server = self.server.write().unwrap();
		server.remove(self);
	}
}

fn conn_stream_ptr(c: &Conn) -> u64 {
	return &(c.stream) as *const Mutex<net::TcpStream> as u64;
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