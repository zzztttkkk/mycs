use std::{io, net, sync};
use std::hash::{Hash, Hasher};
use std::io::Write;

pub struct Conn {
	stream_lock: sync::Mutex<()>,
	stream: net::TcpStream,
}

impl Conn {
	pub fn new(stream: net::TcpStream) -> Self {
		return Conn {
			stream,
			stream_lock: sync::Mutex::new(()),
		};
	}

	pub fn write(&mut self, buf: &[u8]) -> io::Result<usize> {
		let _ = self.stream_lock.lock().unwrap();
		return self.stream.write(buf);
	}

	pub fn flush(&mut self) -> io::Result<()> {
		let _ = self.stream_lock.lock().unwrap();
		return self.stream.flush();
	}

	pub fn handle(&mut self) {}
}

impl PartialEq<Self> for Conn {
	fn eq(&self, other: &Self) -> bool {
		return self as (*const Self) as u64 == other as (*const Self) as u64;
	}
}

impl Eq for Conn {}

impl Hash for Conn {
	fn hash<H: Hasher>(&self, state: &mut H) {
		state.write_u64(self as (*const Self) as u64);
	}

	fn hash_slice<H: Hasher>(data: &[Self], state: &mut H) where Self: Sized {
		for datum in data {
			state.write_u64(datum as (*const Self) as u64);
		}
	}
}